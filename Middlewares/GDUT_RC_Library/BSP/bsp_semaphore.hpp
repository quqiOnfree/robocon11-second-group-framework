#ifndef BSP_SEMAPHORE_HPP
#define BSP_SEMAPHORE_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

namespace gdut {

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
  static constexpr std::size_t max() noexcept {
    return LeastMaxValue;
  }

  constexpr explicit counting_semaphore(std::size_t desired) {
    m_semaphore_id = osSemaphoreNew(LeastMaxValue, desired, nullptr);
  }

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

  template <typename Rep, typename Period>
  osStatus_t acquire(const std::chrono::duration<Rep, Period> &timeout =
                         std::chrono::duration<Rep, Period>::max()) {
    if (m_semaphore_id == nullptr) {
      return osError;
    }
    
    uint32_t ticks;
    if (timeout == std::chrono::duration<Rep, Period>::max()) {
      ticks = osWaitForever;
    } else {
      // Convert to milliseconds to avoid truncation
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
      // Convert milliseconds to ticks (tickFreq is in Hz)
      // ticks = (ms * tickFreq) / 1000
      uint32_t tick_freq = osKernelGetTickFreq();
      // Clamp to UINT32_MAX-1 to avoid overflow (reserve UINT32_MAX for osWaitForever)
      // Calculate max_ms to avoid overflow: max_ms = (UINT32_MAX - 1) * 1000 / tick_freq
      uint64_t max_ms = static_cast<uint64_t>(UINT32_MAX - 1) * 1000ULL / tick_freq;
      if (static_cast<uint64_t>(ms) >= max_ms) {
        ticks = UINT32_MAX - 1;
      } else {
        ticks = static_cast<uint32_t>((static_cast<uint64_t>(ms) * tick_freq) / 1000);
      }
    }
    
    return osSemaphoreAcquire(m_semaphore_id, ticks);
  }

  bool try_acquire() noexcept {
    if (m_semaphore_id == nullptr) {
      return false;
    }
    return acquire(std::chrono::seconds::zero()) == osOK;
  }

  template <class Rep, class Period>
  bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) {
    if (m_semaphore_id == nullptr) {
      return false;
    }
    return acquire(rel_time) == osOK;
  }

private:
  osSemaphoreId_t m_semaphore_id;
};

using binary_semaphore = counting_semaphore<1>;

} // namespace gdut

#endif // BSP_SEMAPHORE_HPP
