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
    // STM32 HAL 的 HAL_SPI_Transmit_DMA 在语义上将 data 视为只读缓冲区，
    // 但其 C 接口未做到 const-correct，参数类型为 uint8_t* 而非 const
    // uint8_t*。 此处使用 const_cast 仅用于适配该 HAL 接口；HAL 不会修改 data
    // 指向的数据。

    m_tx_dma->init();

    // HAL_SPI_Transmit_DMA(m_spi, const_cast<uint8_t *>(data),
    //                      static_cast<uint16_t>(size));
    if (!IS_SPI_DMA_HANDLE(m_spi->hdmatx) ||
        !IS_SPI_DIRECTION_2LINES_OR_1LINE(m_spi->Init.Direction)) {
      std::
          terminate(); // 这属于严重的配置错误，无法通过返回值优雅处理，直接终止程序以引起注意
    }

    if (m_spi->State != HAL_SPI_STATE_READY || data == nullptr || size == 0U) {
      return false;
    }
    /* Process Locked */
    __HAL_LOCK(m_spi);

    /* Set the transaction information */
    m_spi->State = HAL_SPI_STATE_BUSY_TX;
    m_spi->ErrorCode = HAL_SPI_ERROR_NONE;
    m_spi->pTxBuffPtr = data;
    m_spi->TxXferSize = size;
    m_spi->TxXferCount = size;

    /* Init field not used in handle to zero */
    m_spi->pRxBuffPtr = nullptr;
    m_spi->TxISR = nullptr;
    m_spi->RxISR = nullptr;
    m_spi->RxXferSize = 0U;
    m_spi->RxXferCount = 0U;

    /* Configure communication direction : 1Line */
    if (m_spi->Init.Direction == SPI_DIRECTION_1LINE) {
      /* Disable SPI Peripheral before set 1Line direction (BIDIOE bit) */
      __HAL_SPI_DISABLE(m_spi);
      SPI_1LINE_TX(m_spi);
    }

#if (USE_SPI_CRC != 0U)
    /* Reset CRC Calculation */
    if (m_spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
      SPI_RESET_CRC(m_spi);
    }
#endif /* USE_SPI_CRC */

    /* Enable the Tx DMA Stream/Channel */
    if (HAL_OK != HAL_DMA_Start_IT(m_spi->hdmatx, (uint32_t)m_spi->pTxBuffPtr,
                                   (uint32_t)&m_spi->Instance->DR,
                                   m_spi->TxXferCount)) {
      /* Update SPI error code */
      SET_BIT(m_spi->ErrorCode, HAL_SPI_ERROR_DMA);
      /* Process Unlocked */
      __HAL_UNLOCK(m_spi);
      return false;
    }

    /* Check if the SPI is already enabled */
    if ((m_spi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(m_spi);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(m_spi);

    /* Enable the SPI Error Interrupt Bit */
    __HAL_SPI_ENABLE_IT(m_spi, (SPI_IT_ERR));

    /* Enable Tx DMA Request */
    SET_BIT(m_spi->Instance->CR2, SPI_CR2_TXDMAEN);

    return true;
  }

  bool do_receive(uint8_t *buffer, std::size_t size, uint16_t address) {
    (void)address; // SPI 不使用地址参数，忽略以避免编译器警告

    m_rx_dma->init();
    // HAL_SPI_TransmitReceive_DMA(m_spi, buffer, buffer,
    // static_cast<uint16_t>(size));

    // 以下代码参考 STM32 HAL 库中 HAL_SPI_TransmitReceive_DMA 的实现，手动配置
    // DMA 以确保回调正确触发

    /* Check rx & tx dma handles */
    if (!IS_SPI_DMA_HANDLE(m_spi->hdmarx) ||
        !IS_SPI_DMA_HANDLE(m_spi->hdmatx) ||
        !IS_SPI_DIRECTION_2LINES(m_spi->Init.Direction)) {
      std::
          terminate(); // 这属于严重的配置错误，无法通过返回值优雅处理，直接终止程序以引起注意
    }

    /* Init temporary variables */
    HAL_SPI_StateTypeDef tmp_state = m_spi->State;
    uint32_t tmp_mode = m_spi->Init.Mode;

    if (!((tmp_state == HAL_SPI_STATE_READY) ||
          ((tmp_mode == SPI_MODE_MASTER) &&
           (m_spi->Init.Direction == SPI_DIRECTION_2LINES) &&
           (tmp_state == HAL_SPI_STATE_BUSY_RX)))) {
      return false;
    }

    if (buffer == nullptr || (size == 0U)) {
      return false;
    }

    /* Process locked */
    __HAL_LOCK(m_spi);

    /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
    if (m_spi->State != HAL_SPI_STATE_BUSY_RX) {
      m_spi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    /* Set the transaction information */
    m_spi->ErrorCode = HAL_SPI_ERROR_NONE;
    m_spi->pTxBuffPtr = buffer;
    m_spi->TxXferSize = size;
    m_spi->TxXferCount = size;
    m_spi->pRxBuffPtr = buffer; // Assuming buffer is used for both tx and rx
    m_spi->RxXferSize = size;
    m_spi->RxXferCount = size;

    /* Init field not used in handle to zero */
    m_spi->RxISR = NULL;
    m_spi->TxISR = NULL;

#if (USE_SPI_CRC != 0U)
    /* Reset CRC Calculation */
    if (m_spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
      SPI_RESET_CRC(m_spi);
    }
#endif /* USE_SPI_CRC */

    /* Enable the Rx DMA Stream/Channel  */
    if (HAL_OK !=
        HAL_DMA_Start_IT(m_spi->hdmarx,
                         reinterpret_cast<uint32_t>(&m_spi->Instance->DR),
                         reinterpret_cast<uint32_t>(m_spi->pRxBuffPtr),
                         m_spi->RxXferCount)) {
      /* Update SPI error code */
      SET_BIT(m_spi->ErrorCode, HAL_SPI_ERROR_DMA);
      /* Process Unlocked */
      __HAL_UNLOCK(m_spi);
      return false;
    }

    /* Enable Rx DMA Request */
    SET_BIT(m_spi->Instance->CR2, SPI_CR2_RXDMAEN);

    /* Set the SPI Tx DMA transfer complete callback as NULL because the
    communication closing is performed in DMA reception complete callback  */
    m_spi->hdmatx->XferHalfCpltCallback = NULL;
    m_spi->hdmatx->XferCpltCallback = NULL;
    m_spi->hdmatx->XferErrorCallback = NULL;
    m_spi->hdmatx->XferAbortCallback = NULL;

    /* Enable the Tx DMA Stream/Channel  */
    if (HAL_OK != HAL_DMA_Start_IT(m_spi->hdmatx, (uint32_t)m_spi->pTxBuffPtr,
                                   (uint32_t)&m_spi->Instance->DR,
                                   m_spi->TxXferCount)) {
      /* Update SPI error code */
      SET_BIT(m_spi->ErrorCode, HAL_SPI_ERROR_DMA);
      /* Process Unlocked */
      __HAL_UNLOCK(m_spi);
      return false;
    }

    /* Check if the SPI is already enabled */
    if ((m_spi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(m_spi);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(m_spi);

    /* Enable the SPI Error Interrupt Bit */
    __HAL_SPI_ENABLE_IT(m_spi, (SPI_IT_ERR));

    /* Enable Tx DMA Request */
    SET_BIT(m_spi->Instance->CR2, SPI_CR2_TXDMAEN);

    return true;
  }

  SPI_HandleTypeDef *m_spi;
  dma_proxy *m_tx_dma;
  dma_proxy *m_rx_dma;
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
    if (begin == nullptr || size == 0) {
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
    if (data == nullptr || size == 0) {
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
    if (tx_data == nullptr || rx_data == nullptr || size == 0) {
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
  SPI_HandleTypeDef *m_hspi; // SPI 外设句柄引用
};

} // namespace gdut
#endif // BSP_SPI_HPP
