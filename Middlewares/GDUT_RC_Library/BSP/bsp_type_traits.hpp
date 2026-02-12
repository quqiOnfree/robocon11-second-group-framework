#ifndef BSP_TYPE_TRAITS_HPP
#define BSP_TYPE_TRAITS_HPP

#include "stm32f407xx.h"

namespace gdut {

enum class gpio_port : uint8_t { A = 1, B, C, D, E, F, G, H, I };

constexpr GPIO_TypeDef *get_gpio_port_ptr(uint32_t port) {
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

constexpr GPIO_TypeDef *get_gpio_port_ptr(gpio_port port) {
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

} // namespace gdut

#endif // BSP_TYPE_TRAITS_HPP
