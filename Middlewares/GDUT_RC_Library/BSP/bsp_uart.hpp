#ifndef BSP_UART_HPP
#define BSP_UART_HPP

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"

#include "bsp_type_traits.hpp"
#include "bsp_uncopyable.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <utility>

namespace gdut {
class uart : private uncopyable {
public:
  using rx_callback_t = std::function<void(const uint8_t *data, uint16_t size)>;
  using tx_callback_t = std::function<void()>;
  using error_callback_t = std::function<void(uint32_t error)>;
  using idle_callback_t = std::function<void()>;
  using dma_rx_cplt_callback_t = std::function<void()>;
  using dma_tx_cplt_callback_t = std::function<void()>;
  using dma_error_callback_t = std::function<void()>;

  uart(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx = nullptr,
       DMA_HandleTypeDef *hdma_tx = nullptr)
      : m_huart(huart), m_hdma_rx(nullptr), m_hdma_tx(nullptr) {
    init(hdma_rx, hdma_tx);
  }

  ~uart() noexcept { deinit(); }

  // 初始化
  HAL_StatusTypeDef init(DMA_HandleTypeDef *hdma_rx = nullptr,
                         DMA_HandleTypeDef *hdma_tx = nullptr) {
    if (m_huart) {
      attach_dma_rx(hdma_rx);
      attach_dma_tx(hdma_tx);
    }
    return HAL_UART_Init(m_huart);
  }

  HAL_StatusTypeDef deinit() {
    if (HAL_UART_DeInit(m_huart) != HAL_OK) {
      return HAL_ERROR;
    }
    if (m_hdma_rx) {
      m_hdma_rx->Parent = nullptr;
      m_hdma_rx->XferCpltCallback = nullptr;
      m_hdma_rx->XferHalfCpltCallback = nullptr;
      m_hdma_rx->XferErrorCallback = nullptr;
      m_hdma_rx->XferAbortCallback = nullptr;
    }
    if (m_hdma_tx) {
      m_hdma_tx->Parent = nullptr;
      m_hdma_tx->XferCpltCallback = nullptr;
      m_hdma_tx->XferHalfCpltCallback = nullptr;
      m_hdma_tx->XferErrorCallback = nullptr;
      m_hdma_tx->XferAbortCallback = nullptr;
    }
    return HAL_OK;
  }

  uart(uart &&other) noexcept
      : m_huart(std::exchange(other.m_huart, nullptr)),
        m_hdma_rx(std::exchange(other.m_hdma_rx, nullptr)),
        m_hdma_tx(std::exchange(other.m_hdma_tx, nullptr)),
        m_callbacks(std::move(other.m_callbacks)) {}
  uart &operator=(uart &&other) noexcept {
    if (this != std::addressof(other)) {
      deinit();
      m_huart = std::exchange(other.m_huart, nullptr);
      m_hdma_rx = std::exchange(other.m_hdma_rx, nullptr);
      m_hdma_tx = std::exchange(other.m_hdma_tx, nullptr);
      m_callbacks = std::move(other.m_callbacks);
    }
    return *this;
  }

  // 发送函数（阻塞模式）
  HAL_StatusTypeDef
  send(const uint8_t *data, uint16_t size,
       std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
    return HAL_UART_Transmit(m_huart, const_cast<uint8_t *>(data), size,
                             timeout.count() >
                                     std::numeric_limits<uint32_t>::max()
                                 ? std::numeric_limits<uint32_t>::max()
                                 : static_cast<uint32_t>(timeout.count()));
  }

  // 接收函数（阻塞模式）
  HAL_StatusTypeDef receive(
      uint8_t *data, uint16_t size,
      std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
    return HAL_UART_Receive(m_huart, data, size,
                            timeout.count() >
                                    std::numeric_limits<uint32_t>::max()
                                ? std::numeric_limits<uint32_t>::max()
                                : static_cast<uint32_t>(timeout.count()));
  }

  // 发送函数（中断模式）
  HAL_StatusTypeDef send_it(const uint8_t *data, uint16_t size) {
    return HAL_UART_Transmit_IT(m_huart, const_cast<uint8_t *>(data), size);
  }

  // 接收函数（中断模式）
  HAL_StatusTypeDef receive_it(uint8_t *data, uint16_t size) {
    return HAL_UART_Receive_IT(m_huart, data, size);
  }

  // 发送函数（DMA模式）
  HAL_StatusTypeDef send_dma(const uint8_t *data, uint16_t size) {
    if (!m_hdma_tx) {
      return HAL_ERROR;
    }
    return HAL_UART_Transmit_DMA(m_huart, const_cast<uint8_t *>(data), size);
  }

  // 接收函数（DMA模式）
  HAL_StatusTypeDef receive_dma(uint8_t *data, uint16_t size) {
    if (!m_hdma_rx) {
      return HAL_ERROR;
    }
    return HAL_UART_Receive_DMA(m_huart, data, size);
  }

  // 中断控制
  HAL_StatusTypeDef enable_it(uint32_t interrupt) {
    __HAL_UART_ENABLE_IT(m_huart, interrupt);
    return HAL_OK;
  }
  HAL_StatusTypeDef disable_it(uint32_t interrupt) {
    __HAL_UART_DISABLE_IT(m_huart, interrupt);
    return HAL_OK;
  }

  // 获取HAL句柄
  UART_HandleTypeDef *get_huart() const { return m_huart; }

  // 获取DMA句柄
  DMA_HandleTypeDef *get_hdma_rx() const { return m_hdma_rx; }
  DMA_HandleTypeDef *get_hdma_tx() const { return m_hdma_tx; }

  // 检查发送是否完成
  bool is_tx_complete() const {
    return __HAL_UART_GET_FLAG(m_huart, UART_FLAG_TC) != RESET;
  }

  // 检查是否有数据可读
  bool is_rx_ready() const {
    return __HAL_UART_GET_FLAG(m_huart, UART_FLAG_RXNE) != RESET;
  }

  // 配置接口
  void set_baudrate(uint32_t baudrate) { m_huart->Init.BaudRate = baudrate; }
  void set_word_length(uint32_t word_length) {
    m_huart->Init.WordLength = word_length;
  }
  void set_stop_bits(uint32_t stop_bits) { m_huart->Init.StopBits = stop_bits; }
  void set_parity(uint32_t parity) { m_huart->Init.Parity = parity; }
  void set_mode(uint32_t mode) { m_huart->Init.Mode = mode; }
  void set_hw_flow_ctl(uint32_t hw_flow_ctl) {
    m_huart->Init.HwFlowCtl = hw_flow_ctl;
  }
  void set_over_sampling(uint32_t over_sampling) {
    m_huart->Init.OverSampling = over_sampling;
  }
  HAL_StatusTypeDef apply_config() { return HAL_UART_Init(m_huart); }

  // DMA关联函数
  void attach_dma_rx(DMA_HandleTypeDef *hdma_rx) {
    m_hdma_rx = hdma_rx;
    if (!m_hdma_rx) {
      return;
    }

    // 关联DMA回调
    hdma_rx->Parent = this;
    hdma_rx->XferCpltCallback = &uart::dma_rx_xfer_cplt_cb;
    hdma_rx->XferHalfCpltCallback = &uart::dma_rx_xfer_half_cplt_cb;
    hdma_rx->XferErrorCallback = &uart::dma_error_cb;
    hdma_rx->XferAbortCallback = &uart::dma_abort_cb;

    // 设置UART的DMA接收句柄
    __HAL_LINKDMA(m_huart, hdmarx, *m_hdma_rx);
  }

  void attach_dma_tx(DMA_HandleTypeDef *hdma_tx) {
    m_hdma_tx = hdma_tx;
    if (!m_hdma_tx) {
      return;
    }
    // 关联DMA回调
    hdma_tx->Parent = this;
    hdma_tx->XferCpltCallback = &uart::dma_tx_xfer_cplt_cb;
    hdma_tx->XferHalfCpltCallback = nullptr; // 发送通常不需要半传输完成回调
    hdma_tx->XferErrorCallback = &uart::dma_error_cb;
    hdma_tx->XferAbortCallback = &uart::dma_abort_cb;
    // 设置UART的DMA发送句柄
    __HAL_LINKDMA(m_huart, hdmatx, *m_hdma_tx);
  }

  // DMA静态回调函数
  static void dma_rx_xfer_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma) {
      return;
    }
    uart *u = static_cast<uart *>(hdma->Parent);
    if (u && u->m_callbacks.dma_rx_cplt_cb) {
      std::invoke(u->m_callbacks.dma_rx_cplt_cb);
    }
  }
  static void dma_rx_xfer_half_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma) {
      return;
    }
    uart *u = static_cast<uart *>(hdma->Parent);
    // 半传输完成时复用接收完成回调，使上层可通过同一回调处理两种事件
    if (u && u->m_callbacks.dma_rx_cplt_cb) {
      std::invoke(u->m_callbacks.dma_rx_cplt_cb);
    }
  }
  static void dma_tx_xfer_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma) {
      return;
    }
    uart *u = static_cast<uart *>(hdma->Parent);
    if (u && u->m_callbacks.dma_tx_cplt_cb) {
      std::invoke(u->m_callbacks.dma_tx_cplt_cb);
    }
  }
  static void dma_error_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    uart *u = static_cast<uart *>(hdma->Parent);
    if (u && u->m_callbacks.dma_error_cb) {
      std::invoke(u->m_callbacks.dma_error_cb);
    }
  }
  static void dma_abort_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    uart *u = static_cast<uart *>(hdma->Parent);
    if (u && u->m_callbacks.dma_error_cb) {
      std::invoke(u->m_callbacks.dma_error_cb);
    }
  }

  // 回调注册接口
  void register_rx_callback(rx_callback_t cb) {
    m_callbacks.rx_cb = std::move(cb);
  }
  void register_tx_callback(tx_callback_t cb) {
    m_callbacks.tx_cb = std::move(cb);
  }
  void register_error_callback(error_callback_t cb) {
    m_callbacks.error_cb = std::move(cb);
  }
  void register_idle_callback(idle_callback_t cb) {
    m_callbacks.idle_cb = std::move(cb);
  }
  void register_dma_rx_cplt_callback(dma_rx_cplt_callback_t cb) {
    m_callbacks.dma_rx_cplt_cb = std::move(cb);
  }
  void register_dma_tx_cplt_callback(dma_tx_cplt_callback_t cb) {
    m_callbacks.dma_tx_cplt_cb = std::move(cb);
  }
  void register_dma_error_callback(dma_error_callback_t cb) {
    m_callbacks.dma_error_cb = std::move(cb);
  }

  // 回调调用接口（供中断服务例程使用）
  void call_rx_callback(const uint8_t *data, uint16_t size) {
    if (m_callbacks.rx_cb) {
      std::invoke(m_callbacks.rx_cb, data, size);
    }
  }
  void call_tx_callback() {
    if (m_callbacks.tx_cb) {
      std::invoke(m_callbacks.tx_cb);
    }
  }
  void call_error_callback(uint32_t error) {
    if (m_callbacks.error_cb) {
      std::invoke(m_callbacks.error_cb, error);
    }
  }
  void call_idle_callback() {
    if (m_callbacks.idle_cb) {
      std::invoke(m_callbacks.idle_cb);
    }
  }
  void call_dma_rx_cplt_callback() {
    if (m_callbacks.dma_rx_cplt_cb) {
      std::invoke(m_callbacks.dma_rx_cplt_cb);
    }
  }
  void call_dma_tx_cplt_callback() {
    if (m_callbacks.dma_tx_cplt_cb) {
      std::invoke(m_callbacks.dma_tx_cplt_cb);
    }
  }
  void call_dma_error_callback() {
    if (m_callbacks.dma_error_cb) {
      std::invoke(m_callbacks.dma_error_cb);
    }
  }

protected:
  struct uart_callbacks {
    rx_callback_t rx_cb;                   // 接收完成回调
    tx_callback_t tx_cb;                   // 发送完成回调
    error_callback_t error_cb;             // 错误回调
    idle_callback_t idle_cb;               // 空闲中断回调
    dma_rx_cplt_callback_t dma_rx_cplt_cb; // DMA接收完成回调
    dma_tx_cplt_callback_t dma_tx_cplt_cb; // DMA发送完成回调
    dma_error_callback_t dma_error_cb;     // DMA错误回调
  };

private:
  UART_HandleTypeDef *m_huart{nullptr};  // UART句柄
  DMA_HandleTypeDef *m_hdma_rx{nullptr}; // 接收DMA句柄
  DMA_HandleTypeDef *m_hdma_tx{nullptr}; // 发送DMA句柄
  uart_callbacks m_callbacks{};          // 回调管理器
};

/**
 * @brief UART 中断分发器与实例注册管理类。
 *
 * 维护一个静态的 @ref uart 实例注册表，并为 HAL/中断回调提供统一的分发入口。
 * 每个底层 `UART_HandleTypeDef::Instance`（如 USART1、USART2 等）被映射到
 * 对应的 @ref uart 对象指针，从而在中断服务程序中根据硬件句柄找到上层封装
 * 对象并调用其回调。
 *
 * @section uart_irq_handler_usage 使用方式
 * - 创建并初始化 @ref uart 对象后，调用 @ref register_uart 注册该实例。
 * - 在 @ref uart 对象被销毁或硬件外设反初始化之前，必须调用
 *   @ref unregister_uart 取消注册，以避免注册表中残留悬空指针。
 * - HAL 层中断服务函数应调用本类提供的静态分发函数，由本类根据
 *   `UART_HandleTypeDef::Instance` 查找对应的 @ref uart 实例并触发其回调。
 *
 * @section uart_irq_handler_thread_safety 线程安全说明
 * - 本类不包含任何同步原语，本身不是线程安全类。
 * - 建议在调度器启动前或临界区内调用 @ref register_uart / @ref unregister_uart
 *   进行静态配置，不要在 ISR 中修改注册表。
 */
class uart_irq_handler {
public:
  uart_irq_handler() = default;
  ~uart_irq_handler() noexcept = default;
  uart_irq_handler(const uart_irq_handler &) = delete;
  uart_irq_handler &operator=(const uart_irq_handler &) = delete;
  uart_irq_handler(uart_irq_handler &&) = delete;
  uart_irq_handler &operator=(uart_irq_handler &&) = delete;

  static bool register_uart(uart *uart_obj) {
    if (!uart_obj)
      return false;
    if (!uart_obj->get_huart())
      return false;
    uint8_t idx = get_uart_index(uart_obj->get_huart()->Instance);
    if (idx < 6) {
      m_uarts[idx] = uart_obj;
      return true;
    }
    return false;
  }
  static void unregister_uart(uart *uart_obj) {
    if (!uart_obj)
      return;
    if (!uart_obj->get_huart())
      return;
    uint8_t idx = get_uart_index(uart_obj->get_huart()->Instance);
    if (idx < 6) {
      m_uarts[idx] = nullptr;
    }
  }
  // 接收完成中断
  static void handle_rx_cplt(USART_TypeDef *instance, const uint8_t *data,
                             uint16_t size) {
    uint8_t idx = get_uart_index(instance);
    if (idx < 6 && m_uarts[idx]) {
      m_uarts[idx]->call_rx_callback(data, size);
    }
  }

  // 发送完成中断
  static void handle_tx_cplt(USART_TypeDef *instance) {
    uint8_t idx = get_uart_index(instance);
    if (idx < 6 && m_uarts[idx]) {
      m_uarts[idx]->call_tx_callback();
    }
  }

  // 错误中断
  static void handle_error(USART_TypeDef *instance, uint32_t error) {
    uint8_t idx = get_uart_index(instance);
    if (idx < 6 && m_uarts[idx]) {
      m_uarts[idx]->call_error_callback(error);
    }
  }

  // 空闲中断
  static void handle_idle(USART_TypeDef *instance) {
    uint8_t idx = get_uart_index(instance);
    if (idx < 6 && m_uarts[idx]) {
      m_uarts[idx]->call_idle_callback();
    }
  }

  // 接收中断（字节接收）
  static void handle_rx_byte(USART_TypeDef *instance, uint8_t data) {
    uint8_t idx = get_uart_index(instance);
    if (idx < 6 && m_uarts[idx]) {
      // 这里可以添加环形缓冲区处理
      // m_uarts[idx]->m_rx_buffer.push_back(data);
      // 或者直接回调
      m_uarts[idx]->call_rx_callback(&data, 1);
    }
  }

private:
  inline static uart *m_uarts[6] =
      {}; // 支持USART1, USART2, USART3, UART4, UART5, USART6
};

} // namespace gdut

#endif // BSP_UART_HPP