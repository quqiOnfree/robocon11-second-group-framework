#ifndef BSP_SPI_HPP
#define BSP_SPI_HPP

#include "bsp_dma.hpp"
#include "bsp_type_traits.hpp"
#include "bsp_uncopyable.hpp"
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include <chrono>
#include <cstdint>

namespace gdut {

/**
 * @brief 针对 SPI 外设的 DMA 操作封装
 *
 * 通过 CRTP 继承 dma_transfer_base，将 TX/RX dma_proxy 与 SPI HAL 句柄关联，
 * 并通过 HAL_SPI_Transmit_DMA / HAL_SPI_Receive_DMA 发起传输。
 *
 * @note address 参数对 SPI 无意义，所有传输均忽略该参数。
 */
class dma_spi : public dma_transfer_base<dma_spi> {
public:
  explicit dma_spi(SPI_HandleTypeDef *m_spi) : m_spi(m_spi) {}

  ~dma_spi() = default;

  dma_spi(dma_spi &&other) noexcept
      : m_spi(std::exchange(other.m_spi, nullptr)),
        m_tx_dma(std::exchange(other.m_tx_dma, nullptr)),
        m_rx_dma(std::exchange(other.m_rx_dma, nullptr)) {}

  dma_spi &operator=(dma_spi &&other) noexcept {
    if (this != std::addressof(other)) {
      m_spi = std::exchange(other.m_spi, nullptr);
      m_tx_dma = std::exchange(other.m_tx_dma, nullptr);
      m_rx_dma = std::exchange(other.m_rx_dma, nullptr);
    }
    return *this;
  }

private:
  friend class dma_transfer_base<dma_spi>;

  void do_bind_tx(dma_proxy *tx_dma) {
    if (tx_dma && m_spi) {
      m_tx_dma = tx_dma;
      __HAL_LINKDMA(m_spi, hdmatx, *tx_dma->get_handle());
    }
  }

  void do_bind_rx(dma_proxy *rx_dma) {
    if (rx_dma && m_spi) {
      m_rx_dma = rx_dma;
      __HAL_LINKDMA(m_spi, hdmarx, *rx_dma->get_handle());
    }
  }

  bool do_transmit(const uint8_t *data, std::size_t size, uint16_t address) {
    (void)address; // SPI 不使用地址参数，忽略以避免编译器警告

    // 传输大小超过 HAL uint16_t 范围则拒绝，避免截断导致错误传输
    if (m_spi == nullptr || m_tx_dma == nullptr || data == nullptr ||
        size == 0U || size > 65535U) {
      return false;
    }

    if (!IS_SPI_DMA_HANDLE(m_spi->hdmatx) ||
        !IS_SPI_DIRECTION_2LINES_OR_1LINE(m_spi->Init.Direction)) {
      std::
          terminate(); // 这属于严重的配置错误，无法通过返回值优雅处理，直接终止程序以引起注意
    }

    // 初始化 DMA 代理（设置 HAL DMA 句柄参数）
    m_tx_dma->init();

    // 直接调用 HAL_SPI_Transmit_DMA，让 HAL 内部的 DMA 回调负责
    // 设置/恢复 State，避免手动实现不完整导致状态无法恢复。
    // STM32 HAL 的接口未做到 const-correct，此处 const_cast 仅用于适配，
    // HAL 不会修改 data 指向的数据。
    return HAL_SPI_Transmit_DMA(m_spi, const_cast<uint8_t *>(data),
                                static_cast<uint16_t>(size)) == HAL_OK;
  }

  bool do_receive(uint8_t *buffer, std::size_t size, uint16_t address) {
    (void)address; // SPI 不使用地址参数，忽略以避免编译器警告

    // 传输大小超过 HAL uint16_t 范围则拒绝，避免截断导致错误传输
    if (m_spi == nullptr || m_rx_dma == nullptr || buffer == nullptr ||
        size == 0U || size > 65535U) {
      return false;
    }

    /* 全双工模式需要同时配置 TX/RX DMA 句柄 */
    if (!IS_SPI_DMA_HANDLE(m_spi->hdmarx) ||
        !IS_SPI_DMA_HANDLE(m_spi->hdmatx) ||
        !IS_SPI_DIRECTION_2LINES(m_spi->Init.Direction)) {
      std::
          terminate(); // 这属于严重的配置错误，无法通过返回值优雅处理，直接终止程序以引起注意
    }

    // 初始化 DMA 代理（设置 HAL DMA 句柄参数）
    m_rx_dma->init();

    // 直接调用 HAL_SPI_TransmitReceive_DMA（全双工），让 HAL 内部的 DMA 回调负责
    // 设置/恢复 State，避免手动实现不完整导致状态无法恢复。
    // 使用同一缓冲区进行发送和接收：先将 buffer 内容发送出去，接收数据写回 buffer。
    return HAL_SPI_TransmitReceive_DMA(m_spi, buffer, buffer,
                                       static_cast<uint16_t>(size)) == HAL_OK;
  }

  SPI_HandleTypeDef *m_spi{nullptr};
  dma_proxy *m_tx_dma{nullptr};
  dma_proxy *m_rx_dma{nullptr};
};

// SPI 代理类：封装 HAL SPI 阻塞式传输/接收操作，支持 C++20 chrono 超时
class spi_proxy : private uncopyable {
public:
  spi_proxy(SPI_HandleTypeDef *hspi) : m_hspi(hspi) {}

  ~spi_proxy() = default;

  void init() {
    if (!m_hspi) {
      return;
    }
    HAL_SPI_Init(m_hspi);
  }

  void deinit() {
    if (!m_hspi) {
      return;
    }
    HAL_SPI_DeInit(m_hspi);
  }

  void set_mode(spi_mode mode) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.Mode = std::to_underlying(mode);
  }
  void set_direction(spi_direction direction) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.Direction = std::to_underlying(direction);
  }
  void set_data_size(spi_data_size data_size) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.DataSize = std::to_underlying(data_size);
  }
  void set_clock_polarity(spi_clock_polarity polarity) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.CLKPolarity = std::to_underlying(polarity);
  }
  void set_clock_phase(spi_clock_phase phase) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.CLKPhase = std::to_underlying(phase);
  }
  void set_nss(spi_nss nss) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.NSS = std::to_underlying(nss);
  }
  void set_baud_rate_prescaler(spi_baud_rate_prescaler prescaler) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.BaudRatePrescaler = std::to_underlying(prescaler);
  }
  void set_first_bit(spi_first_bit first_bit) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.FirstBit = std::to_underlying(first_bit);
  }
  void set_timode(spi_ti_mode ti_mode) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.TIMode = std::to_underlying(ti_mode);
  }
  void set_crc_calculation(spi_crc_calculation crc_calculation) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.CRCCalculation = std::to_underlying(crc_calculation);
  }
  void set_crc_polynomial(uint32_t crc_polynomial) {
    if (!m_hspi) {
      return;
    }
    m_hspi->Init.CRCPolynomial = crc_polynomial;
  }

  // SPI 发送数据（阻塞模式）
  // 参数：begin - 发送数据指针，size - 字节数，timeout - 超时时间（默认最大值）
  bool transmit(
      const uint8_t *begin, uint16_t size,
      std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
    if (m_hspi == nullptr || begin == nullptr || size == 0) {
      return false; // 参数非法
    }
    auto timeout_ms = timeout.count();
    // HAL_SPI_Transmit 要求 uint8_t* 而非 const uint8_t*，需要 const_cast
    return HAL_SPI_Transmit(m_hspi, const_cast<uint8_t *>(begin), size,
                            timeout_ms >= osWaitForever
                                ? osWaitForever
                                : static_cast<uint32_t>(timeout_ms)) == HAL_OK;
  }

  // SPI 接收数据（阻塞模式）
  // 参数：data - 接收缓冲区，size - 字节数，timeout - 超时时间（默认最大值）
  bool receive(
      uint8_t *data, uint16_t size,
      std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
    if (m_hspi == nullptr || data == nullptr || size == 0) {
      return false; // 参数非法
    }
    auto timeout_ms = timeout.count();
    return HAL_SPI_Receive(m_hspi, data, size,
                           timeout_ms >= osWaitForever
                               ? osWaitForever
                               : static_cast<uint32_t>(timeout_ms)) == HAL_OK;
  }

  // SPI 全双工传输（同时发送和接收，阻塞模式）
  // 参数：tx_data - 发送数据，rx_data - 接收缓冲区，size - 字节数，timeout -
  // 超时
  bool transmit_receive(
      const uint8_t *tx_data, uint8_t *rx_data, uint16_t size,
      std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
    if (m_hspi == nullptr || tx_data == nullptr || rx_data == nullptr ||
        size == 0) {
      return false; // 参数非法
    }
    auto timeout_ms = timeout.count();
    // HAL_SPI_TransmitReceive 要求 uint8_t* 而非 const uint8_t*，需要
    // const_cast
    return HAL_SPI_TransmitReceive(
               m_hspi, const_cast<uint8_t *>(tx_data), rx_data, size,
               timeout_ms >= osWaitForever
                   ? osWaitForever
                   : static_cast<uint32_t>(timeout_ms)) == HAL_OK;
  }

  SPI_HandleTypeDef *get_handle() const { return m_hspi; }

private:
  SPI_HandleTypeDef *m_hspi{nullptr}; // SPI 外设句柄引用
};

} // namespace gdut
#endif // BSP_SPI_HPP
