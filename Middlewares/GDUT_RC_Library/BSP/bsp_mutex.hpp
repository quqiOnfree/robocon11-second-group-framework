#ifndef GDUT_MUTEX_CMSIS_RTOS2_INCLUDED
#define GDUT_MUTEX_CMSIS_RTOS2_INCLUDED

#include <cmsis_os2.h>
#include <memory>
#include <utility>

namespace gdut {

struct empty_mutex_t {
  explicit empty_mutex_t() = default;
};
inline constexpr empty_mutex_t empty_mutex{};

/**
 * @brief RAII wrapper for CMSIS-RTOS2 mutex
 *
 * This class provides a C++-style mutex wrapper around CMSIS-RTOS2 osMutex.
 * Features:
 * - Recursive mutex with priority inheritance
 * - Robust mutex (ownership tracking)
 * - Move semantics supported
 *
 * Thread Safety: All methods are thread-safe.
 *
 * Important: The mutex creation can fail if system resources are exhausted.
 * Use the valid() method or bool operator to check if the mutex was
 * successfully created before use. If the mutex is invalid, lock operations
 * will fail silently (lock() will block forever, try_lock() returns false).
 */
class mutex {
public:
  mutex() : m_mutex_id(nullptr) {
    osMutexAttr_t attr = {
        "GDUT", osMutexRecursive | osMutexPrioInherit | osMutexRobust, 0, 0};
    m_mutex_id = osMutexNew(&attr);
  }
  explicit mutex(empty_mutex_t) : m_mutex_id(nullptr) {}

  /**
   * @brief Construct a mutex wrapper from an existing CMSIS-RTOS2 mutex ID.
   *
   * This constructor takes @b ownership of @p mutex_id: the wrapper will call
   * @c osMutexDelete on the handle when destroyed or moved-from.
   * Do not delete or manage the mutex elsewhere after passing its handle here.
   * Passing @c nullptr is explicitly allowed and results in an invalid mutex
   * object, equivalent to constructing with ::gdut::empty_mutex. In this case,
   * valid() and operator bool() will return false and lock operations will
   * fail with error codes without causing undefined behavior.
   */
  explicit mutex(osMutexId_t mutex_id) : m_mutex_id(mutex_id) {}

  mutex(const mutex &) = delete;
  mutex &operator=(const mutex &) = delete;

  mutex(mutex &&other) noexcept
      : m_mutex_id(std::exchange(other.m_mutex_id, nullptr)) {}
  mutex &operator=(mutex &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_mutex_id != nullptr) {
        osMutexDelete(m_mutex_id);
      }
      m_mutex_id = std::exchange(other.m_mutex_id, nullptr);
    }
    return *this;
  }

  ~mutex() noexcept {
    if (m_mutex_id != nullptr) {
      osMutexDelete(m_mutex_id);
    }
  }

  osStatus_t lock() {
    if (m_mutex_id == nullptr) {
      return osError;
    }
    return osMutexAcquire(m_mutex_id, osWaitForever);
  }

  bool try_lock() {
    if (m_mutex_id == nullptr) {
      return false;
    }
    return osMutexAcquire(m_mutex_id, 0) == osOK;
  }

  osStatus_t unlock() {
    if (m_mutex_id == nullptr) {
      return osError;
    }
    return osMutexRelease(m_mutex_id);
  }

  /**
   * @brief Check if the mutex was successfully created
   * @return true if the mutex is valid and can be used
   */
  bool valid() const noexcept { return m_mutex_id != nullptr; }

  /**
   * @brief Boolean conversion operator for checking validity
   *
   * Allows usage in conditional statements like:
   *   if (mutex_obj) { ... }
   *
   * @return true if the mutex is valid
   */
  explicit operator bool() const noexcept { return valid(); }

private:
  osMutexId_t m_mutex_id;
};

} // namespace gdut

#endif
