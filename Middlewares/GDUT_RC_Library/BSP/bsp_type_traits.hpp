#ifndef BSP_TYPE_TRAITS_HPP
#define BSP_TYPE_TRAITS_HPP

#include "stm32f407xx.h"
#include <chrono>
#include <cmsis_os2.h>
#include <cstdint>

/**
 * @brief Attribute for placing objects into Core Coupled Memory (.ccmram).
 *
 * @note On STM32F407, CCM RAM is not accessible by any DMA controller.
 *       Any object annotated with GDUT_CCMRAM must therefore NOT be:
 *       - Used directly as a DMA source or destination buffer, or
 *       - Indirectly referenced by DMA descriptors or handles.
 *
 *       In particular, thread function objects or other resources that may
 *       participate in DMA operations must not be allocated in CCM RAM.
 */
#define GDUT_CCMRAM __attribute__((section(".ccmram")))

namespace gdut {

/**
 * @brief Type-safe GPIO port enumeration
 */
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

template <typename Rep, typename Period>
uint32_t time_to_ticks(const std::chrono::duration<Rep, Period> &timeout) {
  uint32_t ticks;
  if (timeout == std::chrono::duration<Rep, Period>::max()) {
    ticks = osWaitForever;
  } else {
    // Convert to milliseconds (sub-millisecond precision is truncated)
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    // Handle negative durations (invalid state)
    if (ms < 0) {
      return 0;
    }

    // Handle zero or positive durations
    if (ms == 0) {
      ticks = 0;
    } else {
      // Convert milliseconds to ticks (tickFreq is in Hz)
      // ticks = (ms * tickFreq) / 1000
      uint32_t tick_freq = osKernelGetTickFreq();
      // Clamp to UINT32_MAX-1 to avoid overflow (reserve UINT32_MAX for
      // osWaitForever) Calculate max_ms to avoid overflow: max_ms =
      // (UINT32_MAX - 1) * 1000 / tick_freq
      uint64_t max_ms =
          static_cast<uint64_t>(UINT32_MAX - 1) * 1000ULL / tick_freq;
      if (static_cast<uint64_t>(ms) >= max_ms) {
        ticks = UINT32_MAX - 1;
      } else {
        ticks = static_cast<uint32_t>((static_cast<uint64_t>(ms) * tick_freq) /
                                      1000);
      }
    }
  }
  return ticks;
}

} // namespace gdut

#endif // BSP_TYPE_TRAITS_HPP
