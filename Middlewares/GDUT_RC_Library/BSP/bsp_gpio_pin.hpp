#ifndef GPIO_PIN_HPP
#define GPIO_PIN_HPP

#include "bsp_type_traits.hpp"
#include "stm32f4xx_hal.h"

#include "bsp_uncopyable.hpp"

namespace gdut {

template <gpio_port Port, GPIO_InitTypeDef InitStruct> struct gpio_pin_tag {
  static constexpr gpio_port port = Port;
  static constexpr GPIO_InitTypeDef init_struct = InitStruct;
};

template <gpio_port Port, GPIO_InitTypeDef InitStruct>
class gpio_pin : private uncopyable {
public:
  using tag_type = gpio_pin_tag<Port, InitStruct>;

  gpio_pin() {
    GPIO_InitTypeDef init_struct = tag_type::init_struct;
    HAL_GPIO_Init(get_gpio_port_ptr(tag_type::port), &init_struct);
  }

  ~gpio_pin() {
    HAL_GPIO_DeInit(get_gpio_port_ptr(tag_type::port),
                    tag_type::init_struct.Pin);
  }

  void write(bool state) {
    HAL_GPIO_WritePin(get_gpio_port_ptr(tag_type::port),
                      tag_type::init_struct.Pin,
                      static_cast<GPIO_PinState>(state));
  }

  bool read() const noexcept {
    return HAL_GPIO_ReadPin(get_gpio_port_ptr(tag_type::port),
                            tag_type::init_struct.Pin);
  }

  void toggle() {
    HAL_GPIO_TogglePin(get_gpio_port_ptr(tag_type::port),
                       tag_type::init_struct.Pin);
  }
};

} // namespace gdut

#endif // GPIO_PIN_HPP
