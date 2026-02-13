#ifndef BSP_MEMORYPOOL_HPP
#define BSP_MEMORYPOOL_HPP

#include "FreeRTOS.h"
#include "bsp_mutex.hpp"
#include "portable.h"
#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
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
  allocate(const std::chrono::duration<Rep, Period> &timeout) {
    if (m_pool_id == nullptr) {
      m_pool_id = osMemoryPoolNew(MaxSize, sizeof(value_type), nullptr);
    }
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

  std::add_pointer_t<value_type> allocate() {
    return allocate(std::chrono::milliseconds::max());
  }

  void deallocate(std::add_pointer_t<value_type> ptr) {
    if (m_pool_id == nullptr || ptr == nullptr) {
      return;
    }
    osMemoryPoolFree(m_pool_id, ptr);
  }

  template <typename... Args>
  static void construct(std::add_pointer_t<value_type> ptr, Args &&...args) {
    new (ptr) value_type(std::forward<Args>(args)...);
  }

  static void destroy(std::add_pointer_t<value_type> ptr) {
    if (ptr != nullptr) {
      ptr->~value_type();
    }
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
template <> class [[deprecated]] allocator<void, 0> {};

template <typename Ty, std::size_t MaxSize>
class mutexd_allocator : private allocator<Ty, MaxSize> {
public:
  using value_type = Ty;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using allocator_type = allocator<Ty, MaxSize>;
  static constexpr std::size_t capacity = MaxSize;
  static constexpr std::size_t block_size = sizeof(value_type);

  mutexd_allocator() = default;
  ~mutexd_allocator() noexcept = default;

  mutexd_allocator(const mutexd_allocator &) = delete;
  mutexd_allocator &operator=(const mutexd_allocator &) = delete;

  mutexd_allocator(mutexd_allocator &&other) = delete;
  mutexd_allocator &operator=(mutexd_allocator &&other) = delete;

  template <typename Rep, typename Period>
  std::add_pointer_t<value_type>
  allocate(const std::chrono::duration<Rep, Period> &timeout) {
    lock_guard lock(m_mutex);
    return allocator_type::allocate(timeout);
  }

  std::add_pointer_t<value_type> allocate() {
    lock_guard lock(m_mutex);
    return allocator_type::allocate(std::chrono::milliseconds::max());
  }

  void deallocate(std::add_pointer_t<value_type> ptr) {
    lock_guard lock(m_mutex);
    allocator_type::deallocate(ptr);
  }

  using allocator_type::construct;
  using allocator_type::destroy;

  explicit operator bool() const {
    lock_guard lock(m_mutex);
    return allocator_type::operator bool();
  }

private:
  mutex m_mutex;
};

namespace pmr {

class memory_resource {
  static constexpr size_t max_align = alignof(max_align_t);

public:
  memory_resource() = default;
  memory_resource(const memory_resource &) = default;
  virtual ~memory_resource() = default;

  memory_resource &operator=(const memory_resource &) = default;

  void *allocate(size_t bytes, size_t alignment = max_align) {
    return do_allocate(bytes, alignment);
  }
  void deallocate(void *p, size_t bytes, size_t alignment = max_align) {
    do_deallocate(p, bytes, alignment);
  }

  bool is_equal(const memory_resource &other) const noexcept {
    return do_is_equal(other);
  }

private:
  virtual void *do_allocate(size_t bytes, size_t alignment) = 0;
  virtual void do_deallocate(void *p, size_t bytes, size_t alignment) = 0;

  virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

class new_delete_resource : public memory_resource {
public:
  static memory_resource *get_instance() {
    static new_delete_resource instance;
    return &instance;
  }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    // Use aligned allocation if alignment exceeds default
    if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
      return ::operator new(bytes, std::align_val_t(alignment));
    }
    return ::operator new(bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    (void)bytes; // bytes is ignored in standard delete
    // Use aligned deallocation if alignment exceeds default
    if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
      ::operator delete(p, std::align_val_t(alignment));
    } else {
      ::operator delete(p);
    }
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

class default_memory_resource : public memory_resource {
public:
  static memory_resource *get_instance() {
    static default_memory_resource instance;
    return &instance;
  }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    (void)alignment; // alignment is ignored in this simple implementation
    return pvPortMalloc(bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    (void)bytes;     // bytes is ignored in this simple implementation
    (void)alignment; // alignment is ignored in this simple implementation
    vPortFree(p);
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

template <class Ty = std::byte> class polymorphic_allocator {
public:
  using value_type = Ty;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  explicit polymorphic_allocator(
      memory_resource *r = default_memory_resource::get_instance()) noexcept
      : m_resource(r) {}

  template <class U>
  polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept
      : m_resource(other.resource()) {}

  memory_resource *resource() const noexcept { return m_resource; }

  Ty *allocate(std::size_t n) {
    if (n > std::size_t(-1) / sizeof(Ty)) {
      return nullptr;
    }
    void *p = m_resource->allocate(n * sizeof(Ty), alignof(Ty));
    if (!p) {
      return nullptr;
    }
    return static_cast<Ty *>(p);
  }

  void deallocate(Ty *p, std::size_t n) {
    m_resource->deallocate(p, n * sizeof(Ty), alignof(Ty));
  }

  template <typename U, typename... Args>
  static void construct(std::add_pointer_t<U> ptr, Args &&...args) {
    new (ptr) U(std::forward<Args>(args)...);
  }

  template <typename U> static void destroy(std::add_pointer_t<U> ptr) {
    if (ptr != nullptr) {
      ptr->~U();
    }
  }

  template <typename U, typename... Args>
  std::add_pointer_t<U> new_object(Args &&...args) {
    auto ptr = m_resource->allocate(sizeof(U), alignof(U));
    if (ptr == nullptr) {
      return nullptr;
    }
    try {
      auto typed_ptr = static_cast<std::add_pointer_t<U>>(ptr);
      construct(typed_ptr, std::forward<Args>(args)...);
      return typed_ptr;
    } catch (...) {
      m_resource->deallocate(ptr, sizeof(U), alignof(U));
      throw;
    }
  }

  template <typename U> void delete_object(std::add_pointer_t<U> ptr) {
    if (ptr != nullptr) {
      destroy(ptr);
      m_resource->deallocate(ptr, sizeof(U), alignof(U));
    }
  }

private:
  memory_resource *m_resource;
};

} // namespace pmr

} // namespace gdut

#endif // BSP_MEMORYPOOL_HPP
