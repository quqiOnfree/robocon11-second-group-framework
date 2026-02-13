#ifndef BSP_EVENT_HPP
#define BSP_EVENT_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <utility>

#include "bsp_type_traits.hpp"

namespace gdut {

class event_flags {
public:
  explicit event_flags() { m_id = osEventFlagsNew(nullptr); }

  ~event_flags() {
    if (m_id != nullptr) {
      osEventFlagsDelete(m_id);
    }
  }

  event_flags(const event_flags &) = delete;
  event_flags &operator=(const event_flags &) = delete;

  event_flags(event_flags &&other) noexcept
      : m_id(std::exchange(other.m_id, nullptr)) {}
  event_flags &operator=(event_flags &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_id != nullptr) {
        osEventFlagsDelete(m_id);
      }
      m_id = std::exchange(other.m_id, nullptr);
    }
    return *this;
  }

  uint32_t set(uint32_t flags) { return osEventFlagsSet(m_id, flags); }

  uint32_t clear(uint32_t flags) { return osEventFlagsClear(m_id, flags); }

  uint32_t get() const { return osEventFlagsGet(m_id); }

  template <typename Rep = int64_t, typename Period = std::milli>
  uint32_t wait(uint32_t flags,
                const std::chrono::duration<Rep, Period> &timeout =
                    std::chrono::milliseconds::max(),
                bool wait_all = false, bool no_clear = false) {

    uint32_t options = 0;
    if (wait_all)
      options |= osFlagsWaitAll;
    if (no_clear)
      options |= osFlagsNoClear;
    return osEventFlagsWait(m_id, flags, options, time_to_ticks(timeout));
  }

  bool valid() const noexcept { return m_id != nullptr; }
  explicit operator bool() const noexcept { return valid(); }

private:
  osEventFlagsId_t m_id{nullptr};
};

} // namespace gdut

#endif // BSP_EVENT_HPP
