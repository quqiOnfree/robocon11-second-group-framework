#ifndef BSP_SEMAPHORE_HPP
#define BSP_SEMAPHORE_HPP

#include "bsp_type_traits.hpp"
#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

namespace gdut {

struct empty_semaphore_t {
  explicit empty_semaphore_t() = default;
};
inline constexpr empty_semaphore_t empty_semaphore{};

/**
 * @brief Counting semaphore based on CMSIS-RTOS2
 *
 * This class provides a C++-style counting semaphore wrapper.
 * Features:
 * - Standard semaphore operations (acquire, release, try_acquire)
 * - Timeout support with std::chrono
 * - Move semantics supported
 *
 * Thread Safety: All methods are thread-safe.
 *
 * @tparam LeastMaxValue Maximum value the semaphore can reach
 */
template <std::size_t LeastMaxValue> class counting_semaphore {
public:
  static constexpr std::size_t max() noexcept { return LeastMaxValue; }

  explicit counting_semaphore(std::size_t desired) {
    m_semaphore_id = osSemaphoreNew(LeastMaxValue, desired, nullptr);
  }

  explicit counting_semaphore(empty_semaphore_t) {}

  explicit counting_semaphore(osSemaphoreId_t semaphore_id) : m_semaphore_id(semaphore_id) {}

  ~counting_semaphore() noexcept {
    if (m_semaphore_id != nullptr) {
      osSemaphoreDelete(m_semaphore_id);
    }
  }

  counting_semaphore(const counting_semaphore &) = delete;
  counting_semaphore &operator=(const counting_semaphore &) = delete;

  counting_semaphore(counting_semaphore &&other) noexcept
      : m_semaphore_id(std::exchange(other.m_semaphore_id, nullptr)) {}
  counting_semaphore &operator=(counting_semaphore &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_semaphore_id != nullptr) {
        osSemaphoreDelete(m_semaphore_id);
      }
      m_semaphore_id = std::exchange(other.m_semaphore_id, nullptr);
    }
    return *this;
  }

  osStatus_t release() {
    if (m_semaphore_id == nullptr) {
      return osError;
    }
    return osSemaphoreRelease(m_semaphore_id);
  }

  /**
   * @brief Acquire the semaphore
   *
   * @param timeout Maximum time to wait for the semaphore.
   *                - Use std::chrono::duration<Rep, Period>::max() for infinite
   * wait
   *                - Use std::chrono::seconds::zero() for no wait (try once)
   *                - Precision: milliseconds (sub-millisecond durations are
   * truncated)
   *
   * @return osOK if successful
   *         osErrorParameter if timeout is negative
   *         osErrorTimeout if timeout expired
   *         osError if semaphore is invalid or other error
   */
  template <typename Rep = int64_t, typename Period = std::milli>
  osStatus_t acquire(const std::chrono::duration<Rep, Period> &timeout =
                         std::chrono::milliseconds::max()) {
    if (m_semaphore_id == nullptr) {
      return osError;
    }

    return osSemaphoreAcquire(m_semaphore_id, time_to_ticks(timeout));
  }

  bool try_acquire() noexcept {
    return acquire(std::chrono::seconds::zero()) == osOK;
  }

  template <class Rep, class Period>
  bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) {
    return acquire(rel_time) == osOK;
  }

  bool valid() const noexcept { return m_semaphore_id != nullptr; }
  explicit operator bool() const noexcept { return valid(); }

private:
  osSemaphoreId_t m_semaphore_id{nullptr};
};

using binary_semaphore = counting_semaphore<1>;

} // namespace gdut

#endif // BSP_SEMAPHORE_HPP
