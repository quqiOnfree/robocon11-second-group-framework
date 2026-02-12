#ifndef GPIO_PIN_HPP
#define GPIO_PIN_HPP

#include "bsp_type_traits.hpp"
#include "stm32f4xx_hal.h"

namespace gdut {

template <gpio_port Port, GPIO_InitTypeDef InitStruct> class gpio_pin {
public:
  gpio_pin() {
    GPIO_InitTypeDef init_struct = InitStruct;
    HAL_GPIO_Init(get_gpio_port_ptr(Port), &init_struct);
  }

  ~gpio_pin() { HAL_GPIO_DeInit(get_gpio_port_ptr(Port), InitStruct.Pin); }

  void write(bool state) {
    HAL_GPIO_WritePin(get_gpio_port_ptr(Port), InitStruct.Pin,
                      static_cast<GPIO_PinState>(state));
  }

  bool read() const {
    return HAL_GPIO_ReadPin(get_gpio_port_ptr(Port), InitStruct.Pin);
  }
};

} // namespace gdut

#endif // GPIO_PIN_HPP
