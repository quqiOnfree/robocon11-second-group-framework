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
  static constexpr size_t pool_size = 1024; // 内存池总大小，单位字节
  GDUT_CCMRAM inline static gdut::pmr::fixed_block_resource<pool_size>
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

  thread(osThreadId_t handle, osSemaphoreId_t semaphore) {
    if (handle && semaphore) {
      m_handle =
          std::unique_ptr<std::remove_pointer_t<osThreadId_t>, thread_deleter>{
              handle};
      m_semaphore = std::unique_ptr<std::remove_pointer_t<osSemaphoreId_t>,
                                    semaphore_deleter>{semaphore};
    }
  }

  template <typename Func, typename... Args>
  thread(Func &&func, Args &&...args) {
    static_assert(
        std::is_invocable_v<Func, Args...>,
        "gdut::thread constructor requires a callable that can be invoked "
        "with the provided argument types");
    // 使用 unique_ptr 管理信号量资源，确保异常安全
    m_semaphore =
        std::unique_ptr<std::remove_pointer_t<osSemaphoreId_t>,
                        semaphore_deleter>{osSemaphoreNew(1, 0, nullptr)};
    if (!m_semaphore) {
      return;
    }

    // 捕获信号量句柄的值（而非 this），使线程对象在运行中安全移动
    osSemaphoreId_t sem_handle = m_semaphore.get();
    auto bound = [sem_handle, func = std::forward<Func>(func),
                  ... args = std::forward<Args>(args)]() mutable {
      func(args...);
      osSemaphoreRelease(sem_handle);
    };
    using bound_type = decltype(bound);
    static std::pmr::polymorphic_allocator<bound_type> allocator{
        &thread_memory_resource::pool_resource};
    bound_type *data = nullptr;
    {
      // 线程安全地从内存池分配内存用于存储绑定的函数对象
      std::lock_guard lock(thread_memory_resource::pool_mutex);
      m_control_block = std::unique_ptr<void, memory_deleter>(
          thread_memory_resource::pool_resource.allocate(sizeof(StaticTask_t),
                                                         alignof(StaticTask_t)),
          memory_deleter{sizeof(StaticTask_t), alignof(StaticTask_t)});
      m_stack = std::unique_ptr<void, memory_deleter>(
          thread_memory_resource::pool_resource.allocate(
              StackSize, alignof(std::max_align_t)),
          memory_deleter{StackSize, alignof(std::max_align_t)});
      data = allocator.allocate(1);
    }
    // fixed_block_resource::do_allocate() 在内存不足时返回 nullptr（而非抛出异常），
    // std::pmr::memory_resource::allocate() 会将该 nullptr 透传，
    // 因此此处的空指针检查是必要的
    if (!data || !m_control_block || !m_stack) {
      if (data) {
        std::lock_guard lock(thread_memory_resource::pool_mutex);
        allocator.deallocate(data, 1);
      }
      m_semaphore.reset();
      m_control_block.reset();
      m_stack.reset();
      return;
    }
    allocator.template construct<bound_type>(data, std::move(bound));
    osThreadAttr_t attributes = {.name = "gdut_thread",
                                 .cb_mem = m_control_block.get(),
                                 .cb_size = sizeof(StaticTask_t),
                                 .stack_mem = m_stack.get(),
                                 .stack_size = StackSize,
                                 .priority = Priority};
    m_handle =
        std::unique_ptr<std::remove_pointer_t<osThreadId_t>, thread_deleter>{
            osThreadNew(
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
                static_cast<void *>(data), &attributes)};
    if (!m_handle) {
      allocator.template destroy<bound_type>(data);
      {
        std::lock_guard lock(thread_memory_resource::pool_mutex);
        allocator.deallocate(data, 1);
      }
      m_semaphore.reset();
      m_control_block.reset();
      m_stack.reset();
    }
  }

  ~thread() noexcept {
    terminate();
    m_semaphore.reset();
  }

  bool joinable() const noexcept {
    return m_handle != nullptr &&
           osThreadGetState(m_handle.get()) != osThreadTerminated;
  }

  void join() {
    if (m_handle == nullptr || m_semaphore == nullptr ||
        osThreadGetState(m_handle.get()) == osThreadTerminated) {
      return;
    }
    osSemaphoreAcquire(m_semaphore.get(), osWaitForever);
    // 线程已通过 osThreadExit() 正常退出，使用 release() 而非 reset()
    // 以避免 thread_deleter 再次调用 osThreadTerminate()
    m_handle.release();
    m_semaphore.reset();
  }

  void terminate() {
    if (m_handle) {
      m_handle.reset();
    }
    if (m_semaphore) {
      m_semaphore.reset();
    }
  }

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread &&other) noexcept
      : m_handle(std::move(other.m_handle)),
        m_semaphore(std::move(other.m_semaphore)),
        m_control_block(std::move(other.m_control_block)),
        m_stack(std::move(other.m_stack)) {}

  thread &operator=(thread &&other) noexcept {
    if (this != std::addressof(other)) {
      terminate();
      m_handle = std::move(other.m_handle);
      m_semaphore = std::move(other.m_semaphore);
      m_control_block = std::move(other.m_control_block);
      m_stack = std::move(other.m_stack);
    }
    return *this;
  }

private:
  struct thread_deleter {
    void operator()(osThreadId_t thread) const {
      if (thread != nullptr) {
        osThreadTerminate(thread);
      }
    }
  };

  struct semaphore_deleter {
    void operator()(osSemaphoreId_t sem) const {
      if (sem != nullptr) {
        osSemaphoreDelete(sem);
      }
    }
  };

  struct memory_deleter {
    std::size_t block_size;
    std::size_t alignment;
    void operator()(void *ptr) const {
      if (ptr != nullptr) {
        // 这里不直接调用 osMemoryPoolFree，因为我们使用了自定义的内存资源
        // 需要通过内存资源的 allocator 来释放
        std::lock_guard lock(thread_memory_resource::pool_mutex);
        thread_memory_resource::pool_resource.deallocate(ptr, block_size,
                                                         alignment);
      }
    }
  };

private:
  std::unique_ptr<std::remove_pointer_t<osThreadId_t>, thread_deleter> m_handle{
      nullptr};
  std::unique_ptr<std::remove_pointer_t<osSemaphoreId_t>, semaphore_deleter>
      m_semaphore{nullptr};
  std::unique_ptr<void, memory_deleter> m_control_block{nullptr};
  std::unique_ptr<void, memory_deleter> m_stack{nullptr};
};

} // namespace gdut

#endif // BSP_THREAD_HPP
