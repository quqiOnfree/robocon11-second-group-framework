#ifndef BSP_MESSAGE_QUEUE_HPP
#define BSP_MESSAGE_QUEUE_HPP

#include <chrono>
#include <cmsis_os2.h>
#include <ratio>
#include <type_traits>
#include <utility>

#include "bsp_type_traits.hpp"

namespace gdut {

template <typename Ty>
  requires std::is_trivially_copyable_v<Ty>
class message_queue {
public:
  explicit message_queue(uint32_t msg_count) {
    m_id = osMessageQueueNew(msg_count, sizeof(Ty), nullptr);
  }

  ~message_queue() {
    if (m_id != nullptr) {
      osMessageQueueDelete(m_id);
    }
  }

  message_queue(const message_queue &) = delete;
  message_queue &operator=(const message_queue &) = delete;

  message_queue(message_queue &&other) noexcept
      : m_id(std::exchange(other.m_id, nullptr)) {}
  message_queue &operator=(message_queue &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_id != nullptr) {
        osMessageQueueDelete(m_id);
      }
      m_id = std::exchange(other.m_id, nullptr);
    }
    return *this;
  }

  template <typename Rep = int64_t, typename Period = std::milli>
  bool send(const Ty &msg,
            const std::chrono::duration<Rep, Period> &timeout =
                std::chrono::milliseconds::max(),
            uint8_t priority = 0) {
    if (m_id == nullptr)
      return false;
    return osMessageQueuePut(m_id, &msg, priority, time_to_ticks(timeout)) ==
           osOK;
  }

  bool send_from_isr(const Ty &msg, uint8_t priority = 0) {
    if (m_id == nullptr)
      return false;
    return osMessageQueuePut(m_id, &msg, priority, 0) == osOK;
  }

  template <typename Rep = int64_t, typename Period = std::milli>
  bool receive(Ty &msg, const std::chrono::duration<Rep, Period> &timeout =
                            std::chrono::milliseconds::max()) {
    if (m_id == nullptr)
      return false;
    return osMessageQueueGet(m_id, &msg, nullptr, time_to_ticks(timeout)) ==
           osOK;
  }

  bool receive_from_isr(Ty &msg) {
    if (m_id == nullptr)
      return false;
    return osMessageQueueGet(m_id, &msg, nullptr, 0) == osOK;
  }

  uint32_t count() const {
    if (m_id == nullptr)
      return 0;
    return osMessageQueueGetCount(m_id);
  }
  uint32_t space() const {
    if (m_id == nullptr)
      return 0;
    return osMessageQueueGetSpace(m_id);
  }
  uint32_t capacity() const {
    if (m_id == nullptr)
      return 0;
    return osMessageQueueGetCapacity(m_id);
  }

  bool valid() const noexcept { return m_id != nullptr; }
  explicit operator bool() const noexcept { return valid(); }

protected:
private:
  osMessageQueueId_t m_id{nullptr};
};

} // namespace gdut

#endif // BSP_MESSAGE_QUEUE_HPP
