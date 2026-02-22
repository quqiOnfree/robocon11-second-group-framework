#ifndef BSP_THREAD_HPP
#define BSP_THREAD_HPP

#include "bsp_memory_resource.hpp"
#include "bsp_mutex.hpp"
#include "bsp_type_traits.hpp"
#include <cmsis_os2.h>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <type_traits>
#include <utility>

namespace gdut {

// 内部内存资源，用于线程函数对象的分配
struct thread_memory_resource {
  GDUT_CCMRAM inline static gdut::pmr::fixed_block_resource<256>
      pool_resource{};
  GDUT_CCMRAM inline static gdut::mutex pool_mutex{gdut::empty_mutex};
};

struct empty_thread_t {
  explicit empty_thread_t() = default;
};
inline constexpr empty_thread_t empty_thread{};

/**
 * @brief RAII wrapper for CMSIS-RTOS2 threads
 *
 * This class provides a C++-style thread wrapper similar to std::thread.
 * Features:
 * - Automatic resource cleanup (RAII)
 * - Join semantics with semaphore-based synchronization
 * - Move semantics supported
 *
 * Thread Safety:
 * - join() can be called from any thread but only once
 * - terminate() can be called from any thread but should not be called
 *   while another thread is waiting in join()
 *
 * Usage:
 *   gdut::thread<512> t([]{ do_work(); });
 *   t.join();  // Wait for thread to complete
 *
 * @tparam StackSize Size of the thread stack in bytes
 * @tparam Priority Thread priority (default: osPriorityNormal)
 */
template <size_t StackSize, osPriority_t Priority = osPriorityNormal>
class thread {
public:
  static constexpr size_t stack_size = StackSize;
  static constexpr osPriority_t priority = Priority;

  explicit thread(empty_thread_t) {}
  thread() : thread(empty_thread) {}

  thread(osThreadId_t handle, osSemaphoreId_t semaphore)
      : m_handle(nullptr), m_semaphore(nullptr) {
    if (handle != nullptr && semaphore != nullptr) {
      m_handle = handle;
      m_semaphore = semaphore;
    }
  }

  template <typename Func, typename... Args>
  thread(Func &&func, Args &&...args) {
    static_assert(
        std::is_invocable_v<Func, Args...>,
        "gdut::thread constructor requires a callable that can be invoked "
        "with the provided argument types");
    m_semaphore = osSemaphoreNew(1, 0, nullptr);
    if (m_semaphore == nullptr) {
      return;
    }

    auto bound = [this, func = std::forward<Func>(func),
                  ... args = std::forward<Args>(args)]() mutable {
      func(args...);
      osSemaphoreRelease(this->m_semaphore);
    };
    using bound_type = decltype(bound);
    static std::pmr::polymorphic_allocator<bound_type> allocator{
        &thread_memory_resource::pool_resource};
    bound_type *data = nullptr;
    {
      std::lock_guard lock(thread_memory_resource::pool_mutex);
      data = allocator.allocate(1);
    }
    if (data == nullptr) {
      osSemaphoreDelete(m_semaphore);
      m_semaphore = nullptr;
      return;
    }
    allocator.template construct<bound_type>(data, std::move(bound));
    osThreadAttr_t attributes = {.name = "gdut_thread",
                                 .cb_mem = m_control_block,
                                 .cb_size = sizeof(StaticTask_t),
                                 .stack_mem = m_stack,
                                 .stack_size = StackSize,
                                 .priority = Priority};
    m_handle = osThreadNew(
        [](void *ptr) {
          bound_type *data = static_cast<bound_type *>(ptr);
          (*data)();
          allocator.template destroy<bound_type>(data);
          {
            std::lock_guard lock(thread_memory_resource::pool_mutex);
            allocator.deallocate(data, 1);
          }
          osThreadExit();
        },
        static_cast<void *>(data), &attributes);
    if (m_handle == nullptr) {
      allocator.template destroy<bound_type>(data);
      {
        std::lock_guard lock(thread_memory_resource::pool_mutex);
        allocator.deallocate(data, 1);
      }
      osSemaphoreDelete(m_semaphore);
      m_semaphore = nullptr;
    }
  }

  ~thread() noexcept {
    terminate();
    if (m_semaphore != nullptr) {
      osSemaphoreDelete(m_semaphore);
    }
  }

  bool joinable() const noexcept {
    return m_handle != nullptr &&
           osThreadGetState(m_handle) != osThreadTerminated;
  }

  void join() {
    if (m_handle == nullptr || m_semaphore == nullptr ||
        osThreadGetState(m_handle) == osThreadTerminated) {
      return;
    }
    osSemaphoreAcquire(m_semaphore, osWaitForever);
    // Thread has properly exited via osThreadExit(), handle is already cleaned
    // up
    m_handle = nullptr;
    osSemaphoreDelete(m_semaphore);
    m_semaphore = nullptr;
  }

  void terminate() {
    if (m_handle != nullptr) {
      osThreadTerminate(m_handle);
      m_handle = nullptr;
    }
    if (m_semaphore != nullptr) {
      osSemaphoreDelete(m_semaphore);
      m_semaphore = nullptr;
    }
  }

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread &&other) noexcept
      : m_handle(std::exchange(other.m_handle, nullptr)),
        m_semaphore(std::exchange(other.m_semaphore, nullptr)) {}

  thread &operator=(thread &&other) noexcept {
    if (this != std::addressof(other)) {
      terminate();
      m_handle = std::exchange(other.m_handle, nullptr);
      m_semaphore = std::exchange(other.m_semaphore, nullptr);
    }
    return *this;
  }

private:
  osThreadId_t m_handle{nullptr};
  osSemaphoreId_t m_semaphore{nullptr};
  alignas(std::max_align_t) char m_control_block[sizeof(StaticTask_t)]{};
  alignas(std::max_align_t) char m_stack[StackSize]{};
};

} // namespace gdut

#endif // BSP_THREAD_HPP
