#ifndef BSP_MEMORYPOOL_HPP
#define BSP_MEMORYPOOL_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
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
 * - Thread-safe allocation
 * - Timeout support
 * - Move semantics supported
 * 
 * Thread Safety: All methods are thread-safe.
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

  allocator() = default;
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

  template <typename Rep, typename Period>
  std::add_pointer_t<value_type>
  allocate(const std::chrono::duration<Rep, Period> &timeout =
               std::chrono::duration<Rep, Period>::max()) {
    if (m_pool_id == nullptr) {
      m_pool_id = osMemoryPoolNew(MaxSize, sizeof(value_type), nullptr);
    }
    return static_cast<std::add_pointer_t<value_type>>(osMemoryPoolAlloc(
        m_pool_id,
        timeout != std::chrono::duration<Rep, Period>::max()
            ? std::chrono::duration_cast<std::chrono::seconds>(timeout)
                      .count() *
                  osKernelGetTickFreq()
            : osWaitForever));
  }

  bool deallocate(std::add_pointer_t<value_type> ptr) {
    if (m_pool_id == nullptr || ptr == nullptr) {
      return false;
    }
    osMemoryPoolFree(m_pool_id, ptr);
    return true;
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
