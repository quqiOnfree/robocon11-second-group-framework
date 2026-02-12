#ifndef GPIO_PIN_HPP
#define GPIO_PIN_HPP

#include "bsp_type_traits.hpp"
#include "stm32f4xx_hal.h"

#include "bsp_uncopyable.hpp"

namespace gdut {

/**
 * @brief GPIO pin tag for compile-time configuration
 * @tparam Port The GPIO port (A-I)
 * @tparam InitStruct The HAL GPIO initialization structure
 */
template <gpio_port Port, GPIO_InitTypeDef InitStruct> struct gpio_pin_tag {
  static constexpr gpio_port port = Port;
  static constexpr GPIO_InitTypeDef init_struct = InitStruct;
};

/**
 * @brief RAII wrapper for HAL GPIO pin
 *
 * This class provides compile-time configured GPIO pin management.
 * The pin is initialized in constructor and de-initialized in destructor.
 *
 * Features:
 * - Compile-time configuration via template parameters
 * - RAII resource management
 * - Type-safe port and pin selection
 * - Non-copyable (hardware resource)
 *
 * Usage:
 *   gdut::gpio_pin<gdut::gpio_port::A,
 *                  GPIO_InitTypeDef{.Pin = GPIO_PIN_5,
 *                                   .Mode = GPIO_MODE_OUTPUT_PP}> led;
 *   led.write(true);  // Turn on
 *
 * @tparam Port The GPIO port (A-I)
 * @tparam InitStruct The HAL GPIO initialization structure
 */
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
