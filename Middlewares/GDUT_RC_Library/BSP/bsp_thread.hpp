#ifndef BSP_THREAD_HPP
#define BSP_THREAD_HPP

#include <cmsis_os2.h>
#include <cstddef>
#include <memory>
#include <stdlib.h>
#include <utility>

namespace gdut {

template <size_t StackSize, osPriority_t Priority = osPriorityNormal>
class thread {
public:
  static constexpr size_t stack_size = StackSize;
  static constexpr osPriority_t priority = Priority;
  static constexpr osThreadAttr_t attributes = {
      .name = "gdut_thread", .stack_size = StackSize, .priority = Priority};

  thread() = default;

  template <typename Func, typename... Args>
  thread(Func &&func, Args &&...args) {
    m_sem = osSemaphoreNew(1, 0, nullptr);
    if (m_sem == nullptr) {
      return;
    }

    auto bound = [this, func = std::forward<Func>(func),
                  ... args = std::forward<Args>(args)]() mutable {
      func(args...);
      osSemaphoreRelease(this->m_sem);
    };
    using Bound = decltype(bound);
    Bound *data = new Bound{std::move(bound)};
    m_handle = osThreadNew(
        [](void *ptr) {
          Bound *data = static_cast<Bound *>(ptr);
          (*data)();
          delete data;
        },
        static_cast<void *>(data), &attributes);
    if (m_handle == nullptr) {
      delete data;
      osSemaphoreDelete(m_sem);
    }
  }

  ~thread() noexcept {
    terminate();
    if (m_sem != nullptr) {
      osSemaphoreDelete(m_sem);
    }
  }

  bool joinable() const noexcept {
    return m_handle != nullptr &&
           osThreadGetState(m_handle) != osThreadTerminated;
  }

  void join() {
    if (m_handle == nullptr || m_sem == nullptr ||
        osThreadGetState(m_handle) == osThreadTerminated) {
      return;
    }
    osSemaphoreAcquire(m_sem, osWaitForever);
    osSemaphoreDelete(m_sem);
    m_handle = nullptr;
    m_sem = nullptr;
  }

  void terminate() {
    if (m_handle != nullptr) {
      osThreadTerminate(m_handle);
      m_handle = nullptr;
    }
  }

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread &&other) noexcept
      : m_handle(std::exchange(other.m_handle, nullptr)),
        m_sem(std::exchange(other.m_sem, nullptr)) {}

  thread &operator=(thread &&other) noexcept {
    if (this != std::addressof(other)) {
      terminate();
      if (m_sem)
        osSemaphoreDelete(m_sem);
      m_handle = std::exchange(other.m_handle, nullptr);
      m_sem = std::exchange(other.m_sem, nullptr);
    }
    return *this;
  }

private:
  osThreadId_t m_handle{nullptr};
  osSemaphoreId_t m_sem{nullptr};
};

} // namespace gdut

#endif // BSP_THREAD_HPP
