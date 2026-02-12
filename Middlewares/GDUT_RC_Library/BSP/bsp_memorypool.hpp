#ifndef BSP_MEMORYPOOL_HPP
#define BSP_MEMORYPOOL_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace gdut {

/**
 * @brief Memory pool allocator based on CMSIS-RTOS2
 *
 * This class provides a fixed-size memory pool allocator.
 * Note: This is NOT a standard C++ allocator. It provides
 * raw memory allocation without calling constructors/destructors.
 *
 * Features:
 * - Fixed-size blocks (sizeof(Ty))
 * - Thread-safe allocation (once pool is created)
 * - Timeout support
 * - Move semantics supported
 *
 * Thread Safety:
 * - The pool is lazily initialized on first allocate() call.
 * - If the same allocator instance is used from multiple threads,
 *   the first call to allocate() must complete before any concurrent
 *   calls to avoid race conditions during pool creation.
 * - After the pool is created, all methods are thread-safe.
 * - Recommended: Create the pool before sharing the allocator across threads
 *   by calling allocate() once during initialization.
 *
 * Important: The caller is responsible for:
 * - Calling constructors after allocate()
 * - Calling destructors before deallocate()
 *
 * @tparam Ty The type of objects to allocate
 * @tparam MaxSize Maximum number of objects in the pool
 */
template <typename Ty, std::size_t MaxSize> class allocator {
public:
  using value_type = Ty;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  static constexpr std::size_t capacity = MaxSize;
  static constexpr std::size_t block_size = sizeof(value_type);

  allocator() {
    m_pool_id = osMemoryPoolNew(MaxSize, sizeof(value_type), nullptr);
  }
  allocator(const allocator &) = delete;
  allocator &operator=(const allocator &) = delete;

  allocator(allocator &&other) noexcept
      : m_pool_id(std::exchange(other.m_pool_id, nullptr)) {}
  allocator &operator=(allocator &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_pool_id != nullptr) {
        osMemoryPoolDelete(m_pool_id);
      }
      m_pool_id = std::exchange(other.m_pool_id, nullptr);
    }
    return *this;
  }

  /**
   * @brief Allocate a block from the memory pool
   *
   * @param timeout Maximum time to wait for a free block.
   *                - Use std::chrono::duration<Rep, Period>::max() for infinite
   * wait
   *                - Use std::chrono::seconds::zero() for no wait (try once)
   *                - Precision: milliseconds (sub-millisecond durations are
   * truncated)
   *
   * @return Pointer to allocated block, or nullptr if:
   *         - timeout is negative (invalid parameter)
   *         - timeout expired
   *         - pool creation failed
   *         - no blocks available
   */
  template <typename Rep, typename Period>
  std::add_pointer_t<value_type>
  allocate(const std::chrono::duration<Rep, Period> &timeout =
               std::chrono::duration<Rep, Period>::max()) {
    uint32_t ticks;
    if (timeout == std::chrono::duration<Rep, Period>::max()) {
      ticks = osWaitForever;
    } else {
      // Convert to milliseconds (sub-millisecond precision is truncated)
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                    .count();
      // Handle negative durations (invalid state)
      if (ms < 0) {
        return nullptr;
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
          ticks = static_cast<uint32_t>(
              (static_cast<uint64_t>(ms) * tick_freq) / 1000);
        }
      }
    }

    return static_cast<std::add_pointer_t<value_type>>(
        osMemoryPoolAlloc(m_pool_id, ticks));
  }

  bool deallocate(std::add_pointer_t<value_type> ptr) {
    if (m_pool_id == nullptr || ptr == nullptr) {
      return false;
    }
    return osMemoryPoolFree(m_pool_id, ptr) == osOK;
  }

  osMemoryPoolId_t release() { return std::exchange(m_pool_id, nullptr); }

  void reset(osMemoryPoolId_t pool_id) {
    if (m_pool_id != nullptr) {
      osMemoryPoolDelete(m_pool_id);
    }
    m_pool_id = pool_id;
  }

  ~allocator() noexcept {
    if (m_pool_id != nullptr) {
      osMemoryPoolDelete(m_pool_id);
    }
  }

  explicit operator bool() const { return m_pool_id != nullptr; }

  friend bool operator==(const allocator &a, const allocator &b) {
    return a.m_pool_id == b.m_pool_id;
  }

  friend bool operator!=(const allocator &a, const allocator &b) {
    return a.m_pool_id != b.m_pool_id;
  }

private:
  osMemoryPoolId_t m_pool_id{nullptr};
};

template <std::size_t MaxSize> class [[deprecated]] allocator<void, MaxSize> {};

} // namespace gdut

#endif // BSP_MEMORYPOOL_HPP
