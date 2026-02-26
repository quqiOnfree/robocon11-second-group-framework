#ifndef BSP_DMA_HPP
#define BSP_DMA_HPP

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_uart.h"

#include "bsp_function.hpp"
#include "bsp_uncopyable.hpp"
#include "bsp_type_traits.hpp"

#include <cstddef>
#include <system_error>
#include <utility>

namespace gdut {

/**
 * @brief DMA 错误类型的 std::error_category 实现
 *
 * 将 STM32 HAL DMA 错误码（HAL_DMA_ERROR_*）映射为可读字符串，
 * 以便通过 std::error_code 统一向上层上报 DMA 传输错误。
 */
class dma_error_category : public std::error_category {
public:
  constexpr dma_error_category() noexcept = default;

  const char *name() const noexcept override { return "dma_error_code"; }

  std::string message(int ev) const override {
    switch (ev) {
    case HAL_DMA_ERROR_NONE:
      return "No error";
    case HAL_DMA_ERROR_TE:
      return "Transfer error";
    case HAL_DMA_ERROR_FE:
      return "FIFO error";
    case HAL_DMA_ERROR_DME:
      return "Direct mode error";
    case HAL_DMA_ERROR_TIMEOUT:
      return "Timeout error";
    case HAL_DMA_ERROR_PARAM:
      return "Parameter error";
    case HAL_DMA_ERROR_NO_XFER:
      return "Abort requested with no transfer ongoing";
    case HAL_DMA_ERROR_NOT_SUPPORTED:
      return "Not supported mode";
    default:
      return "Unknown error";
    }
  }

  static const dma_error_category &instance() {
    static dma_error_category instance;
    return instance;
  }
};

/**
 * @brief 类型安全的 DMA 错误码枚举，对应 HAL_DMA_ERROR_* 系列宏
 */
enum class dma_error_code : uint32_t {
  none = HAL_DMA_ERROR_NONE,
  transfer_error = HAL_DMA_ERROR_TE,
  fifo_error = HAL_DMA_ERROR_FE,
  direct_mode_error = HAL_DMA_ERROR_DME,
  timeout_error = HAL_DMA_ERROR_TIMEOUT,
  parameter_error = HAL_DMA_ERROR_PARAM,
  no_transfer = HAL_DMA_ERROR_NO_XFER,
  not_supported = HAL_DMA_ERROR_NOT_SUPPORTED
};

/**
 * @brief 将 dma_error_code 转换为 std::error_code。
 *
 * 提供此函数后，结合下方的 std::is_error_code_enum 特化，
 * dma_error_code 枚举值可隐式转换为 std::error_code，
 * 方便在错误处理代码中直接使用枚举值而无需手动构造 error_code。
 */
inline std::error_code make_error_code(dma_error_code e) {
  return {static_cast<int>(e), dma_error_category::instance()};
}

} // namespace gdut

namespace std {

/// 启用 gdut::dma_error_code 到 std::error_code 的隐式转换
template <>
struct is_error_code_enum<gdut::dma_error_code> : true_type {};

} // namespace std

namespace gdut::dma {

/**
 * @brief HAL DMA 句柄的 RAII 代理封装
 *
 * 对 STM32 HAL 的 DMA_HandleTypeDef 进行 C++ 面向对象封装，提供
 * 安全的初始化、配置和传输启动接口，以及基于 std::error_code 的统一错误回调。
 *
 * 特性：
 * - RAII 自动管理：析构时调用 deinit()，清理 HAL 句柄和回调指针
 * - 支持移动语义，禁止复制
 * - 统一错误回调（std::error_code）：传输完成/出错均通过同一回调通知
 * - 所有 set_* 配置方法均含 nullptr 有效性检查，空句柄时为空操作
 * - valid() / operator bool() 可快速检查句柄是否可用
 *
 * 线程安全：
 * - 该类**不**提供内部同步。回调函数在中断上下文执行；
 *   若需从任务上下文并发访问，调用方须在外部加互斥保护。
 * - bind_tx/bind_rx 和 init() 应在 DMA 传输开始前完成单次配置，
 *   不支持运行时并发修改。
 * - 在 DMA 传输进行中调用 set_callback_handler() 是不安全的。
 *
 * 重要约束：
 * - DMA 传输所用的数据缓冲区**不能**放在 CCMRAM（CCM RAM 不能被 DMA 访问）。
 * - 不调用 init() 而直接调用 start() 会因为 Parent 未设置而导致回调不触发。
 *
 * 使用示例：
 * @code
 * // CubeMX 生成的全局 DMA 句柄
 * DMA_HandleTypeDef hdma_usart1_rx;
 *
 * gdut::dma::dma_proxy dma_rx(&hdma_usart1_rx);
 * dma_rx.set_callback_handler([](std::error_code ec) {
 *     if (!ec) { // 传输完成（error_code 为空表示成功）
 *         process_dma_data();
 *     } else {   // 传输出错
 *         handle_dma_error(ec);
 *     }
 * });
 * dma_rx.init();
 * @endcode
 */
class dma_proxy : uncopyable {
public:
  using callback_t = function<void(std::error_code)>;

  explicit dma_proxy(DMA_HandleTypeDef *handle) : m_handle(handle) {}

  dma_proxy(dma_proxy &&other) noexcept
      : m_handle(std::exchange(other.m_handle, nullptr)),
        m_callback_handler(std::move(other.m_callback_handler)) {}

  dma_proxy &operator=(dma_proxy &&other) noexcept {
    if (this != std::addressof(other)) {
      deinit();
      m_handle = std::exchange(other.m_handle, nullptr);
      m_callback_handler = std::move(other.m_callback_handler);
    }
    return *this;
  }

  ~dma_proxy() noexcept { deinit(); }

  /**
   * @brief 初始化 DMA 句柄，注册 HAL 内部回调并调用 HAL_DMA_Init。
   *
   * 必须在开始任何传输之前调用。若 m_handle 为 nullptr，则为空操作。
   * m_handle->Parent 被设置为 this，以便 HAL 回调可找到对应的 dma_proxy 对象。
   */
  void init() {
    if (m_handle) {
      m_handle->XferCpltCallback = dma_rx_xfer_cplt_cb;
      m_handle->XferHalfCpltCallback = nullptr;
      m_handle->XferErrorCallback = dma_error_cb;
      m_handle->XferAbortCallback = dma_error_cb;
      m_handle->Parent = this; // 关联 DMA 句柄与 dma_proxy 对象，用于回调派发
      HAL_DMA_Init(m_handle);
    }
  }

  /**
   * @brief 反初始化 DMA，清理所有回调指针及句柄关联。
   *
   * 以尽力（best-effort）方式执行清理：即使 HAL_DMA_DeInit 返回非 HAL_OK，
   * 也会继续清空回调指针并将 m_handle 置为 nullptr，以防止悬空指针。
   */
  void deinit() {
    if (m_handle) {
      HAL_DMA_DeInit(m_handle); // 尽力反初始化，不检查返回值
      m_handle->XferCpltCallback = nullptr;
      m_handle->XferHalfCpltCallback = nullptr;
      m_handle->XferErrorCallback = nullptr;
      m_handle->XferAbortCallback = nullptr;
      m_handle->Parent = nullptr;
      m_handle = nullptr;
    }
  }

  [[nodiscard]] bool valid() const noexcept { return m_handle != nullptr; }

  explicit operator bool() const noexcept { return valid(); }

  /**
   * @brief 以中断模式启动一次 DMA 传输。
   *
   * 返回底层 HAL_DMA_Start_IT 的状态码，调用方可选择检查。
   * 同时，若启动失败（句柄无效或 HAL 出错），也会通过回调向上层通知，
   * 以便仅依赖回调的调用方感知启动阶段的失败。
   *
   * @param src_address  源地址
   * @param dst_address  目标地址
   * @param data_length  数据长度（以 DMA 数据宽度单位为单位）
   * @return HAL_StatusTypeDef  HAL_OK 表示启动成功，否则为错误码
   */
  HAL_StatusTypeDef start(void *src_address, void *dst_address,
                          std::size_t data_length) {
    if (!m_handle) {
      if (m_callback_handler) {
        m_callback_handler(std::make_error_code(std::errc::invalid_argument));
      }
      return HAL_ERROR; // DMA 句柄无效
    }
    const auto status =
        HAL_DMA_Start_IT(m_handle, reinterpret_cast<uint32_t>(src_address),
                         reinterpret_cast<uint32_t>(dst_address), data_length);
    if (status != HAL_OK) {
      if (m_callback_handler) {
        // 启动失败，通过回调上报 HAL DMA 错误码（ErrorCode 含具体故障原因）
        m_callback_handler(std::error_code(
            static_cast<int>(m_handle->ErrorCode),
            gdut::dma_error_category::instance()));
      }
    }
    return status;
  }

  [[nodiscard]] DMA_HandleTypeDef *get_handle() const { return m_handle; }

  void set_callback_handler(const function<void(std::error_code)> &handler) {
    m_callback_handler = handler;
  }

  void set_callback_handler(function<void(std::error_code)> &&handler) {
    m_callback_handler = std::move(handler);
  }

  void set_instance(dma_stream_type instance) {
    if (!m_handle) return;
    m_handle->Instance = get_dma_stream(instance);
  }

  void set_channel(dma_channel channel) {
    if (!m_handle) return;
    m_handle->Init.Channel = std::to_underlying(channel);
  }

  void set_direction(dma_direction direction) {
    if (!m_handle) return;
    m_handle->Init.Direction = std::to_underlying(direction);
  }

  void set_periph_inc(bool enable) {
    if (!m_handle) return;
    m_handle->Init.PeriphInc = enable ? DMA_PINC_ENABLE : DMA_PINC_DISABLE;
  }

  void set_mem_inc(bool enable) {
    if (!m_handle) return;
    m_handle->Init.MemInc = enable ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
  }

  void set_periph_data_alignment(dma_peripheral_data_alignment alignment) {
    if (!m_handle) return;
    m_handle->Init.PeriphDataAlignment = std::to_underlying(alignment);
  }

  void set_mem_data_alignment(dma_memory_data_alignment alignment) {
    if (!m_handle) return;
    m_handle->Init.MemDataAlignment = std::to_underlying(alignment);
  }

  void set_mode(dma_mode mode) {
    if (!m_handle) return;
    m_handle->Init.Mode = std::to_underlying(mode);
  }

  void set_priority(dma_priority priority) {
    if (!m_handle) return;
    m_handle->Init.Priority = std::to_underlying(priority);
  }

  void set_fifo_mode(dma_fifo_mode mode) {
    if (!m_handle) return;
    m_handle->Init.FIFOMode = std::to_underlying(mode);
  }

  void set_fifo_threshold(dma_fifo_threshold threshold) {
    if (!m_handle) return;
    m_handle->Init.FIFOThreshold = std::to_underlying(threshold);
  }

  void set_memory_burst(dma_memory_burst burst) {
    if (!m_handle) return;
    m_handle->Init.MemBurst = std::to_underlying(burst);
  }

  void set_peripheral_burst(dma_peripheral_burst burst) {
    if (!m_handle) return;
    m_handle->Init.PeriphBurst = std::to_underlying(burst);
  }

  void call_dma_callback(std::error_code ec) {
    if (m_callback_handler) {
      m_callback_handler(ec);
    }
  }

protected:
  static void dma_rx_xfer_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    dma_proxy *d = static_cast<dma_proxy *>(hdma->Parent);
    if (d && d->m_callback_handler) {
      // 传输完成，以空的 error_code 通知上层（无错误）
      d->m_callback_handler(std::error_code());
    }
  }

  static void dma_error_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    dma_proxy *d = static_cast<dma_proxy *>(hdma->Parent);
    if (d && d->m_callback_handler) {
      d->m_callback_handler(std::error_code(
          hdma->ErrorCode, gdut::dma_error_category::instance()));
    }
  }

private:
  DMA_HandleTypeDef *m_handle{nullptr};
  callback_t m_callback_handler{}; // 显式初始化为空，防止未初始化的函数对象被调用
};

/**
 * @brief DMA 外设操作的 CRTP 基类
 *
 * 使用 CRTP（奇异递归模板模式）为具体外设（UART/I2C/SPI）提供统一的
 * DMA 操作接口（绑定 TX/RX 代理、发送、接收），避免虚函数调度开销。
 *
 * 派生类须实现以下私有方法（通过 friend class dma_base<Derived> 开放访问）：
 * - do_bind_tx(dma_proxy*)
 * - do_bind_rx(dma_proxy*)
 * - do_transmit(const uint8_t*, std::size_t, uint16_t)
 * - do_receive(uint8_t*, std::size_t, uint16_t)
 *
 * @tparam Derived  派生类类型（如 dma_uart、dma_i2c、dma_spi）
 *
 * @note 不可复制，但允许派生类提供移动操作。
 * @note address 参数含义因外设而异：
 *       - UART/SPI：忽略 address 参数（内部通过 (void)address 消除警告）
 *       - I2C：7 位从机地址，有效范围 0x08~0x77，address=0 在 I2C 总线上无效。
 *
 * 线程安全：bind_tx/bind_rx/transmit/receive 均**不**提供内部同步，
 * 应在单一上下文中依次调用（配置阶段绑定，传输阶段发起）。
 */
template <typename Derived> class dma_base : uncopyable {
public:
  ~dma_base() = default;

  void bind_tx(dma_proxy *tx_dma) {
    static_cast<Derived *>(this)->do_bind_tx(tx_dma);
  }

  void bind_rx(dma_proxy *rx_dma) {
    static_cast<Derived *>(this)->do_bind_rx(rx_dma);
  }

  /**
   * @brief 发起 DMA 发送。
   * @param data     发送数据缓冲区（HAL 接口未做到 const-correct，
   *                 内部使用 const_cast 适配，HAL 不会修改数据内容）
   * @param size     数据长度（字节数）
   * @param address  仅 I2C 有效：7 位从机地址（UART/SPI 忽略）
   */
  void transmit(const uint8_t *data, std::size_t size, uint16_t address = 0) {
    static_cast<Derived *>(this)->do_transmit(data, size, address);
  }

  /**
   * @brief 发起 DMA 接收。
   * @param buffer   接收缓冲区
   * @param size     数据长度（字节数）
   * @param address  仅 I2C 有效：7 位从机地址（UART/SPI 忽略）
   */
  void receive(uint8_t *buffer, std::size_t size, uint16_t address = 0) {
    static_cast<Derived *>(this)->do_receive(buffer, size, address);
  }

  /// 同时绑定 TX 和 RX 两个 DMA 代理
  void bind(dma_proxy *tx_dma, dma_proxy *rx_dma) {
    bind_tx(tx_dma);
    bind_rx(rx_dma);
  }

  /// @deprecated 请使用 receive() 替代
  [[deprecated("请使用 receive() 替代 start_receive()")]]
  void start_receive(uint8_t *buffer, std::size_t size) {
    receive(buffer, size);
  }
};

/**
 * @brief 针对 UART 外设的 DMA 操作封装
 *
 * 通过 CRTP 继承 dma_base，将 TX/RX dma_proxy 与 UART HAL 句柄关联，
 * 并通过 HAL_UART_Transmit_DMA / HAL_UART_Receive_DMA 发起传输。
 *
 * @note address 参数对 UART 无意义，所有传输均忽略该参数。
 */
class dma_uart : public dma_base<dma_uart> {
public:
  explicit dma_uart(UART_HandleTypeDef *huart) : m_uart(huart) {}

  ~dma_uart() = default;

  dma_uart(dma_uart &&other) noexcept
      : m_uart(std::exchange(other.m_uart, nullptr)) {}

  dma_uart &operator=(dma_uart &&other) noexcept {
    if (this != std::addressof(other)) {
      m_uart = std::exchange(other.m_uart, nullptr);
    }
    return *this;
  }

private:
  friend class dma_base<dma_uart>;

  void do_bind_tx(dma_proxy *tx_dma) {
    if (tx_dma && m_uart) {
      __HAL_LINKDMA(m_uart, hdmatx, *tx_dma->get_handle());
    }
  }

  void do_bind_rx(dma_proxy *rx_dma) {
    if (rx_dma && m_uart) {
      __HAL_LINKDMA(m_uart, hdmarx, *rx_dma->get_handle());
    }
  }

  void do_transmit(const uint8_t *data, std::size_t size, uint16_t address) {
    (void)address; // UART 不使用地址参数，忽略以避免编译器警告
    // STM32 HAL 的 HAL_UART_Transmit_DMA 在语义上将 data 视为只读缓冲区，
    // 但其 C 接口未做到 const-correct，参数类型为 uint8_t* 而非 const uint8_t*。
    // 此处使用 const_cast 仅用于适配该 HAL 接口；HAL 不会修改 data 指向的数据。
    HAL_UART_Transmit_DMA(m_uart, const_cast<uint8_t *>(data),
                          static_cast<uint16_t>(size));
  }

  void do_receive(uint8_t *buffer, std::size_t size, uint16_t address) {
    (void)address; // UART 不使用地址参数，忽略以避免编译器警告
    HAL_UART_Receive_DMA(m_uart, buffer, static_cast<uint16_t>(size));
  }

private:
  UART_HandleTypeDef *m_uart;
};

/**
 * @brief 针对 I2C 外设的 DMA 操作封装（主机模式）
 *
 * 通过 CRTP 继承 dma_base，将 TX/RX dma_proxy 与 I2C HAL 句柄关联，
 * 并通过 HAL_I2C_Master_Transmit_DMA / HAL_I2C_Master_Receive_DMA 发起传输。
 *
 * @note address 为 7 位从机地址（有效范围 0x08~0x77），传 0 在 I2C 总线上无效。
 */
class dma_i2c : public dma_base<dma_i2c> {
public:
  explicit dma_i2c(I2C_HandleTypeDef *hi2c) : m_i2c(hi2c) {}

  ~dma_i2c() = default;

  dma_i2c(dma_i2c &&other) noexcept
      : m_i2c(std::exchange(other.m_i2c, nullptr)) {}

  dma_i2c &operator=(dma_i2c &&other) noexcept {
    if (this != std::addressof(other)) {
      m_i2c = std::exchange(other.m_i2c, nullptr);
    }
    return *this;
  }

private:
  friend class dma_base<dma_i2c>;

  void do_bind_tx(dma_proxy *tx_dma) {
    if (tx_dma && m_i2c) {
      __HAL_LINKDMA(m_i2c, hdmatx, *tx_dma->get_handle());
    }
  }

  void do_bind_rx(dma_proxy *rx_dma) {
    if (rx_dma && m_i2c) {
      __HAL_LINKDMA(m_i2c, hdmarx, *rx_dma->get_handle());
    }
  }

  void do_transmit(const uint8_t *data, std::size_t size, uint16_t address) {
    // STM32 HAL 的 HAL_I2C_Master_Transmit_DMA 在语义上将 data 视为只读缓冲区，
    // 但其 C 接口未做到 const-correct，参数类型为 uint8_t* 而非 const uint8_t*。
    // 此处使用 const_cast 仅用于适配该 HAL 接口；HAL 不会修改 data 指向的数据。
    HAL_I2C_Master_Transmit_DMA(m_i2c, address, const_cast<uint8_t *>(data),
                                static_cast<uint16_t>(size));
  }

  void do_receive(uint8_t *buffer, std::size_t size, uint16_t address) {
    HAL_I2C_Master_Receive_DMA(m_i2c, address, buffer,
                               static_cast<uint16_t>(size));
  }

private:
  I2C_HandleTypeDef *m_i2c;
};

/**
 * @brief 针对 SPI 外设的 DMA 操作封装
 *
 * 通过 CRTP 继承 dma_base，将 TX/RX dma_proxy 与 SPI HAL 句柄关联，
 * 并通过 HAL_SPI_Transmit_DMA / HAL_SPI_Receive_DMA 发起传输。
 *
 * @note address 参数对 SPI 无意义，所有传输均忽略该参数。
 */
class dma_spi : public dma_base<dma_spi> {
public:
  explicit dma_spi(SPI_HandleTypeDef *hspi) : m_spi(hspi) {}

  ~dma_spi() = default;

  dma_spi(dma_spi &&other) noexcept
      : m_spi(std::exchange(other.m_spi, nullptr)) {}

  dma_spi &operator=(dma_spi &&other) noexcept {
    if (this != std::addressof(other)) {
      m_spi = std::exchange(other.m_spi, nullptr);
    }
    return *this;
  }

private:
  friend class dma_base<dma_spi>;

  void do_bind_tx(dma_proxy *tx_dma) {
    if (tx_dma && m_spi) {
      __HAL_LINKDMA(m_spi, hdmatx, *tx_dma->get_handle());
    }
  }

  void do_bind_rx(dma_proxy *rx_dma) {
    if (rx_dma && m_spi) {
      __HAL_LINKDMA(m_spi, hdmarx, *rx_dma->get_handle());
    }
  }

  void do_transmit(const uint8_t *data, std::size_t size, uint16_t address) {
    (void)address; // SPI 不使用地址参数，忽略以避免编译器警告
    // STM32 HAL 的 HAL_SPI_Transmit_DMA 在语义上将 data 视为只读缓冲区，
    // 但其 C 接口未做到 const-correct，参数类型为 uint8_t* 而非 const uint8_t*。
    // 此处使用 const_cast 仅用于适配该 HAL 接口；HAL 不会修改 data 指向的数据。
    HAL_SPI_Transmit_DMA(m_spi, const_cast<uint8_t *>(data),
                         static_cast<uint16_t>(size));
  }

  void do_receive(uint8_t *buffer, std::size_t size, uint16_t address) {
    (void)address; // SPI 不使用地址参数，忽略以避免编译器警告
    HAL_SPI_Receive_DMA(m_spi, buffer, static_cast<uint16_t>(size));
  }

private:
  SPI_HandleTypeDef *m_spi;
};

} // namespace gdut::dma

#endif // BSP_DMA_HPP
