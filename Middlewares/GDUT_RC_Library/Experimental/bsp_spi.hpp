#ifndef BSP_SPI_HPP
#define BSP_SPI_HPP

#include "../BSP/bsp_type_traits.hpp"
#include "../BSP/bsp_uncopyable.hpp"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include <chrono>
#include <cstdint>
#include <ratio>

namespace gdut {

// SPI 代理类：封装 HAL SPI 阻塞式传输/接收操作，支持 C++20 chrono 超时
class spi_proxy : private uncopyable {
public:
  spi_proxy(SPI_HandleTypeDef &hspi) : m_hspi(hspi) {}

  // SPI 发送数据（阻塞模式）
  // 参数：begin - 发送数据指针，size - 字节数，timeout - 超时时间（默认 1 秒）
  template <typename Rep = int64_t, typename Period = std::milli>
  bool transmit(const uint8_t *begin, uint16_t size,
                const std::chrono::duration<Rep, Period> &timeout =
                    std::chrono::milliseconds::max()) {
    if (begin == nullptr || size == 0) {
      return false; // 参数非法
    }
    // HAL_SPI_Transmit 要求 uint8_t* 而非 const uint8_t*，需要 const_cast
    return HAL_SPI_Transmit(&m_hspi, const_cast<uint8_t *>(begin), size,
                            time_to_ticks(timeout)) == HAL_OK;
  }

  // SPI 接收数据（阻塞模式）
  // 参数：data - 接收缓冲区，size - 字节数，timeout - 超时时间（默认 1 秒）
  template <typename Rep = int64_t, typename Period = std::milli>
  bool receive(uint8_t *data, uint16_t size,
               const std::chrono::duration<Rep, Period> &timeout =
                   std::chrono::milliseconds::max()) {
    if (data == nullptr || size == 0) {
      return false; // 参数非法
    }
    return HAL_SPI_Receive(&m_hspi, data, size, time_to_ticks(timeout)) ==
           HAL_OK;
  }

  // SPI 全双工传输（同时发送和接收，阻塞模式）
  // 参数：tx_data - 发送数据，rx_data - 接收缓冲区，size - 字节数，timeout -
  // 超时
  template <typename Rep = int64_t, typename Period = std::milli>
  bool transmit_receive(const uint8_t *tx_data, uint8_t *rx_data, uint16_t size,
                        const std::chrono::duration<Rep, Period> &timeout =
                            std::chrono::milliseconds::max()) {
    if (tx_data == nullptr || rx_data == nullptr || size == 0) {
      return false; // 参数非法
    }
    // HAL_SPI_TransmitReceive 要求 uint8_t* 而非 const uint8_t*，需要
    // const_cast
    return HAL_SPI_TransmitReceive(&m_hspi, const_cast<uint8_t *>(tx_data),
                                   rx_data, size,
                                   time_to_ticks(timeout)) == HAL_OK;
  }

private:
  SPI_HandleTypeDef &m_hspi; // SPI 外设句柄引用
};

} // namespace gdut
#endif // BSP_SPI_HPP
