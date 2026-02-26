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

#include <sys/types.h>
#include <system_error>
#include <utility>

namespace gdut::dma {

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

  void init() {
    if (m_handle) {
      m_handle->XferCpltCallback = dma_rx_xfer_cplt_cb;
      m_handle->XferHalfCpltCallback = nullptr;
      m_handle->XferErrorCallback = dma_error_cb;
      m_handle->XferAbortCallback = dma_error_cb;
      m_handle->Parent = this; // 关联DMA句柄和dma对象
      HAL_DMA_Init(m_handle);
    }
  }

  void deinit() {
    if (m_handle) {
      HAL_DMA_DeInit(m_handle);
      m_handle->XferCpltCallback = nullptr;
      m_handle->XferHalfCpltCallback = nullptr;
      m_handle->XferErrorCallback = nullptr;
      m_handle->XferAbortCallback = nullptr;
      m_handle->Parent = nullptr;
      m_handle = nullptr;
    }
  }

  bool valid() const noexcept { return m_handle != nullptr; }

  explicit operator bool() const noexcept { return valid(); }

  HAL_StatusTypeDef start(void *src_address, void *dst_address,
                          std::size_t data_length) {
    if (!m_handle) {
      return HAL_ERROR; // DMA句柄无效
    }
    return HAL_DMA_Start_IT(m_handle, reinterpret_cast<uint32_t>(src_address),
                            reinterpret_cast<uint32_t>(dst_address),
                            data_length);
  }

  DMA_HandleTypeDef *get_handle() const { return m_handle; }

  void set_callback_handler(const function<void(std::error_code)> &handler) {
    m_callback_handler = handler;
  }

  void set_callback_handler(function<void(std::error_code)> &&handler) {
    m_callback_handler = std::move(handler);
  }

  void set_instance(dma_stream_type instance)
  {
    m_handle->Instance = get_dma_stream(instance);
  }

  void set_channel(dma_channel channel) {
    m_handle->Init.Channel = std::to_underlying(channel);
  }

  void set_direction(dma_direction direction) {
    m_handle->Init.Direction = std::to_underlying(direction);
  }

  void set_periph_inc(bool enable) {
    m_handle->Init.PeriphInc = enable ? DMA_PINC_ENABLE : DMA_PINC_DISABLE;
  }

  void set_mem_inc(bool enable) {
    m_handle->Init.MemInc = enable ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
  }

  void set_periph_data_alignment(dma_peripheral_data_alignment alignment) {
    m_handle->Init.PeriphDataAlignment = std::to_underlying(alignment);
  }

  void set_mem_data_alignment(dma_memory_data_alignment alignment) {
    m_handle->Init.MemDataAlignment = std::to_underlying(alignment);
  }

  void set_mode(dma_mode mode) { m_handle->Init.Mode = std::to_underlying(mode); }

  void set_priority(dma_priority priority) {
    m_handle->Init.Priority = std::to_underlying(priority);
  }

  void set_fifo_mode(dma_fifo_mode mode) {
    m_handle->Init.FIFOMode = std::to_underlying(mode);
  }

  void set_fifo_threshold(dma_fifo_threshold threshold) {
    m_handle->Init.FIFOThreshold = std::to_underlying(threshold);
  }

  void set_memory_burst(dma_memory_burst burst) {
    m_handle->Init.MemBurst = std::to_underlying(burst);
  }

  void set_peripheral_burst(dma_peripheral_burst burst) {
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
      // 这里可以添加DMA完成后的处理逻辑，例如通知上层任务
      d->m_callback_handler(std::error_code());
    }
  }

  static void dma_error_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    dma_proxy *d = static_cast<dma_proxy *>(hdma->Parent);
    if (d && d->m_callback_handler) {
      d->m_callback_handler(
          std::error_code(hdma->ErrorCode, dma_error_category::instance()));
    }
  }

private:
  DMA_HandleTypeDef *m_handle{nullptr};
  callback_t m_callback_handler;
};

template <typename Derived> class dma_base : uncopyable {
public:
  ~dma_base() = default;
  void bind_tx(dma_proxy *tx_dma) {
    static_cast<Derived *>(this)->do_bind_tx(tx_dma);
  }
  void bind_rx(dma_proxy *rx_dma) {
    static_cast<Derived *>(this)->do_bind_rx(rx_dma);
  }
  void transmit(const uint8_t *data, size_t size, uint16_t address = 0) {
    static_cast<Derived *>(this)->do_transmit(data, size, address);
  }
  void receive(uint8_t *buffer, size_t size, uint16_t address = 0) {
    static_cast<Derived *>(this)->do_receive(buffer, size, address);
  }

public:
  void bind(dma_proxy *tx_dma, dma_proxy *rx_dma) {
    bind_tx(tx_dma);
    bind_rx(rx_dma);
  }

  void start_receive(uint8_t *buffer, size_t size) { receive(buffer, size); }
};

class dma_uart : public dma_base<dma_uart> {
public:
  dma_uart(UART_HandleTypeDef *huart) : m_uart(huart) {}

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

  void do_transmit(const uint8_t *data, size_t size, uint16_t address) {
    (void)address; // UART不使用地址参数，忽略它以避免编译器警告
    HAL_UART_Transmit_DMA(m_uart, const_cast<uint8_t *>(data), size);
  }

  void do_receive(uint8_t *buffer, size_t size, uint16_t address) {
    (void)address; // UART不使用地址参数，忽略它以避免编译器警告
    HAL_UART_Receive_DMA(m_uart, buffer, size);
  }

private:
  UART_HandleTypeDef *m_uart;
};

class dma_i2c : public dma_base<dma_i2c> {
public:
  dma_i2c(I2C_HandleTypeDef *hi2c) : m_i2c(hi2c) {}

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

  void do_transmit(const uint8_t *data, size_t size, uint16_t address) {
    HAL_I2C_Master_Transmit_DMA(m_i2c, address, const_cast<uint8_t *>(data),
                                size);
  }

  void do_receive(uint8_t *buffer, size_t size, uint16_t address) {
    HAL_I2C_Master_Receive_DMA(m_i2c, address, buffer, size);
  }

private:
  I2C_HandleTypeDef *m_i2c;
};

class dma_spi : public dma_base<dma_spi> {
public:
  dma_spi(SPI_HandleTypeDef *hspi) : m_spi(hspi) {}

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

  void do_transmit(const uint8_t *data, size_t size, uint16_t address) {
    (void)address; // SPI不使用地址参数，忽略它以避免编译器警告
    HAL_SPI_Transmit_DMA(m_spi, const_cast<uint8_t *>(data), size);
  }

  void do_receive(uint8_t *buffer, size_t size, uint16_t address) {
    (void)address; // SPI不使用地址参数，忽略它以避免编译器警告
    HAL_SPI_Receive_DMA(m_spi, buffer, size);
  }

private:
  SPI_HandleTypeDef *m_spi;
};

} // namespace gdut::dma

#endif // BSP_DMA_HPP
