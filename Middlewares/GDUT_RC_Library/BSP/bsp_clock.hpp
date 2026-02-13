#ifndef BSP_CLOCK_HPP
#define BSP_CLOCK_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <cstdint>

namespace gdut {

struct basic_kernel_clock {
  basic_kernel_clock() = delete;

  static std::uint32_t get_tick_count() noexcept {
    return osKernelGetTickCount();
  }

  static std::uint32_t get_tick_freq() noexcept {
    return osKernelGetTickFreq();
  }

  static std::uint32_t get_sys_timer_count() noexcept {
    return osKernelGetSysTimerCount();
  }

  static std::uint32_t get_sys_timer_freq() noexcept {
    return osKernelGetSysTimerFreq();
  }
};

struct system_clock {
  using duration = std::chrono::milliseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<system_clock>;

  static constexpr bool is_steady = true;

  static time_point now() noexcept {
    return time_point(duration(
        basic_kernel_clock::get_tick_count() /
        (basic_kernel_clock::get_tick_freq() / duration::period::den)));
  }
};

struct steady_clock {
  using duration = std::chrono::microseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<steady_clock>;

  static constexpr bool is_steady = true;

  static time_point now() noexcept {
    return time_point(duration(
        basic_kernel_clock::get_sys_timer_count() /
        (basic_kernel_clock::get_sys_timer_freq() / duration::period::den)));
  }
};

using high_resolution_clock = steady_clock;

static_assert(std::chrono::is_clock_v<system_clock>);
static_assert(std::chrono::is_clock_v<steady_clock>);

} // namespace gdut

#endif // BSP_CLOCK_HPP
