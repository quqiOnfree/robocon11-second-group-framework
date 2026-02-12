#ifndef BSP_TYPE_TRAITS_HPP
#define BSP_TYPE_TRAITS_HPP

#include "stm32f407xx.h"

namespace gdut {

enum class gpio_port : uint8_t { A = 1, B, C, D, E, F, G, H, I };

[[nodiscard]] constexpr GPIO_TypeDef *get_gpio_port_ptr(uint32_t port) {
  switch (port) {
  case GPIOA_BASE:
    return GPIOA;
  case GPIOB_BASE:
    return GPIOB;
  case GPIOC_BASE:
    return GPIOC;
  case GPIOD_BASE:
    return GPIOD;
  case GPIOE_BASE:
    return GPIOE;
  case GPIOF_BASE:
    return GPIOF;
  case GPIOG_BASE:
    return GPIOG;
  case GPIOH_BASE:
    return GPIOH;
  case GPIOI_BASE:
    return GPIOI;
  default:
    return nullptr; // Invalid port
  }
}

[[nodiscard]] constexpr GPIO_TypeDef *get_gpio_port_ptr(gpio_port port) {
  switch (port) {
  case gpio_port::A:
    return GPIOA;
  case gpio_port::B:
    return GPIOB;
  case gpio_port::C:
    return GPIOC;
  case gpio_port::D:
    return GPIOD;
  case gpio_port::E:
    return GPIOE;
  case gpio_port::F:
    return GPIOF;
  case gpio_port::G:
    return GPIOG;
  case gpio_port::H:
    return GPIOH;
  case gpio_port::I:
    return GPIOI;
  default:
    return nullptr; // Invalid port
  }
}

enum class timer_id : uint8_t {
  tim1 = 1,
  tim2,
  tim3,
  tim4,
  tim5,
  tim9,
  tim10,
  tim11
};

[[nodiscard]] constexpr TIM_TypeDef *get_timer_ptr(timer_id id) {
  switch (id) {
  case timer_id::tim1:
    return TIM1;
  case timer_id::tim2:
    return TIM2;
  case timer_id::tim3:
    return TIM3;
  case timer_id::tim4:
    return TIM4;
  case timer_id::tim5:
    return TIM5;
  case timer_id::tim9:
    return TIM9;
  case timer_id::tim10:
    return TIM10;
  case timer_id::tim11:
    return TIM11;
  default:
    return nullptr; // Invalid timer ID
  }
}

} // namespace gdut

#endif // BSP_TYPE_TRAITS_HPP
