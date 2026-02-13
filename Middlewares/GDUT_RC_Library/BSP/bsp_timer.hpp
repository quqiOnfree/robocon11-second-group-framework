#ifndef BSP_GPIO_TIMER_HPP
#define BSP_GPIO_TIMER_HPP

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

#include "bsp_gpio_pin.hpp"
#include "bsp_type_traits.hpp"

namespace gdut {

template <gpio_port Port, TIM_Base_InitTypeDef TimInitStruct> struct timer_tag {
  static constexpr gpio_port port = Port;
  static constexpr TIM_Base_InitTypeDef tim_init_struct = TimInitStruct;
};

} // namespace gdut

#endif // BSP_GPIO_TIMER_HPP
