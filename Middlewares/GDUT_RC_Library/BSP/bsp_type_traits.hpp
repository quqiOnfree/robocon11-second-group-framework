#ifndef BSP_TYPE_TRAITS_HPP
#define BSP_TYPE_TRAITS_HPP

#include "stm32f407xx.h"
#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>

/**
 * @brief 将对象放置到核心耦合存储器（.ccmram）的属性
 *
 * @note 在 STM32F407 上，CCM RAM 不能被任何 DMA 控制器访问。
 *       线程栈、控制块和函数对象可以安全地放在 CCM RAM。
 *       但任何直接或间接作为 DMA 源/目的的数据缓冲区都
 *       不能放在 CCM RAM 中（即不要使用 GDUT_CCMRAM 标注、
 *       不要从 CCMRAM 池分配，也不要捕获到存放在 CCMRAM 的 lambda）。
 */
#define GDUT_CCMRAM __attribute__((section(".ccmram")))

namespace gdut {

template <std::size_t Value> struct is_power_of_two {
  static constexpr bool value = Value && (Value & (Value - 1)) == 0;
};

template <std::size_t Value>
inline constexpr bool is_power_of_two_v = is_power_of_two<Value>::value;

template <typename> struct always_false : std::false_type {};

template <typename T>
inline constexpr bool always_false_v = always_false<T>::value;

/**
 * @brief 类型安全的 GPIO 端口枚举
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
    return nullptr; // 端口非法
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
    return nullptr; // 端口非法
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
    return nullptr; // 定时器 ID 非法
  }
}

// 获取UART实例索引
[[nodiscard]] constexpr uint8_t get_uart_index(USART_TypeDef *uart_instance) {
  switch (reinterpret_cast<uintptr_t>(uart_instance)) {
  case USART1_BASE:
    return 0;
  case USART2_BASE:
    return 1;
  case USART3_BASE:
    return 2;
  case UART4_BASE:
    return 3;
  case UART5_BASE:
    return 4;
  case USART6_BASE:
    return 5;
  default:
    return 0xFF;
  }
}

template <typename Rep, typename Period>
uint32_t time_to_ticks(const std::chrono::duration<Rep, Period> &timeout) {
  uint32_t ticks;
  if (timeout == std::chrono::duration<Rep, Period>::max()) {
    ticks = osWaitForever;
  } else {
    // 转换为毫秒（亚毫秒精度会被截断）
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    // 处理负数时长（非法状态）
    if (ms < 0) {
      return 0;
    }

    // 处理 0 或正数时长
    if (ms == 0) {
      ticks = 0;
    } else {
      // 将毫秒转换为 tick（tickFreq 单位为 Hz）
      // tick 计算：ticks = (ms * tickFreq) / 1000
      uint32_t tick_freq = osKernelGetTickFreq();
      // 夹紧到 UINT32_MAX-1 以避免溢出（UINT32_MAX 保留给 osWaitForever）
      // 为避免溢出，计算 max_ms：max_ms = (UINT32_MAX - 1) * 1000 / tick_freq
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

// 针对 std::chrono::milliseconds 的特化重载，避免不必要的转换
inline uint32_t time_to_ticks(std::chrono::milliseconds timeout) {
  if (timeout == std::chrono::milliseconds::max()) {
    return osWaitForever;
  }

  auto ms = timeout.count();
  // 处理负数时长（非法状态）
  if (ms < 0) {
    return 0;
  }

  // 将毫秒转换为 tick（tickFreq 单位为 Hz）
  uint32_t tick_freq = osKernelGetTickFreq();
  // 夹紧到 UINT32_MAX-1 以避免溢出（UINT32_MAX 保留给 osWaitForever）
  uint64_t max_ms = static_cast<uint64_t>(UINT32_MAX - 1) * 1000ULL / tick_freq;
  if (static_cast<uint64_t>(ms) >= max_ms) {
    return UINT32_MAX - 1;
  }
  return static_cast<uint32_t>((static_cast<uint64_t>(ms) * tick_freq) / 1000);
}

} // namespace gdut

#endif // BSP_TYPE_TRAITS_HPP 结束
