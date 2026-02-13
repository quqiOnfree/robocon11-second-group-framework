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

  // system_clock may be adjusted by the OS, therefore not monotonic/steady
  static constexpr bool is_steady = false;

  static time_point now() noexcept {
    // Use integer arithmetic to avoid precision loss
    // Convert ticks to milliseconds: (ticks * 1000) / tick_freq
    uint32_t ticks = basic_kernel_clock::get_tick_count();
    uint32_t freq = basic_kernel_clock::get_tick_freq();
    uint64_t ms = (static_cast<uint64_t>(ticks) * duration::period::den) / freq;
    return time_point(duration(static_cast<rep>(ms)));
  }
};

struct steady_clock {
  using duration = std::chrono::microseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<steady_clock>;

  static constexpr bool is_steady = true;

  static time_point now() noexcept {
    // Use integer arithmetic to avoid precision loss
    // Convert sys timer counts to microseconds
    uint32_t counts = basic_kernel_clock::get_sys_timer_count();
    uint32_t freq = basic_kernel_clock::get_sys_timer_freq();
    uint64_t us =
        (static_cast<uint64_t>(counts) * duration::period::den) / freq;
    return time_point(duration(static_cast<rep>(us)));
  }
};

using high_resolution_clock = steady_clock;

static_assert(std::chrono::is_clock_v<system_clock>);
static_assert(std::chrono::is_clock_v<steady_clock>);

} // namespace gdut

#endif // BSP_CLOCK_HPP
