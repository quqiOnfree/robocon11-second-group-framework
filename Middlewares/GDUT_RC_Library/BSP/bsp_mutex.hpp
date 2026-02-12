#ifndef GDUT_MUTEX_CMSIS_RTOS2_INCLUDED
#define GDUT_MUTEX_CMSIS_RTOS2_INCLUDED

#include <cmsis_os2.h>
#include <memory>
#include <utility>

namespace gdut {

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
 */
class mutex {
public:
  mutex() : m_mutex_id(NULL) {
    osMutexAttr_t attr = {
        "GDUT", osMutexRecursive | osMutexPrioInherit | osMutexRobust, 0, 0};
    m_mutex_id = osMutexNew(&attr);
  }

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

  void lock() { osMutexAcquire(m_mutex_id, osWaitForever); }

  bool try_lock() { return osMutexAcquire(m_mutex_id, 0) == osOK; }

  void unlock() { osMutexRelease(m_mutex_id); }

private:
  osMutexId_t m_mutex_id;
};

/**
 * @brief Tag types for lock construction strategies
 */
struct defer_lock_t {
  explicit defer_lock_t() = default;
};
inline constexpr defer_lock_t defer_lock{};
struct try_to_lock_t {
  explicit try_to_lock_t() = default;
};
inline constexpr try_to_lock_t try_to_lock{};
struct adopt_lock_t {
  explicit adopt_lock_t() = default;
};
inline constexpr adopt_lock_t adopt_lock{};

/**
 * @brief RAII lock guard for automatic mutex locking/unlocking
 * 
 * Similar to std::lock_guard. Locks the mutex in constructor,
 * unlocks in destructor. Non-copyable and non-movable.
 * 
 * Usage:
 *   {
 *     gdut::lock_guard lock(my_mutex);
 *     // Critical section protected by mutex
 *   } // Automatically unlocks
 * 
 * @tparam Mutex The mutex type to lock
 */
template <typename Mutex> class lock_guard {
public:
  explicit lock_guard(Mutex &mtx) : m_mtx(mtx) { m_mtx.lock(); }

  lock_guard(Mutex &mtx, adopt_lock_t) : m_mtx(mtx) {}

  lock_guard(Mutex &mtx, defer_lock_t) = delete;
  lock_guard(Mutex &mtx, try_to_lock_t) = delete;

  lock_guard(const lock_guard &) = delete;
  lock_guard &operator=(const lock_guard &) = delete;
  lock_guard(lock_guard &&) = delete;
  lock_guard &operator=(lock_guard &&) = delete;

  ~lock_guard() noexcept { m_mtx.unlock(); }

private:
  Mutex &m_mtx;
};

/**
 * @brief Movable RAII lock with deferred and try-lock support
 * 
 * Similar to std::unique_lock. Provides more flexibility than lock_guard:
 * - Can be unlocked before destructor
 * - Supports deferred locking
 * - Supports try-lock
 * - Move semantics supported
 * 
 * Usage:
 *   gdut::unique_lock lock(my_mutex);  // Locks immediately
 *   // or
 *   gdut::unique_lock lock(my_mutex, gdut::defer_lock);
 *   lock.lock();  // Lock later
 * 
 * @tparam Mutex The mutex type to lock
 */
template <typename Mutex> class unique_lock {
public:
  unique_lock() noexcept = default;

  explicit unique_lock(Mutex &m) : m_mtx(std::addressof(m)), m_owns(true) { m_mtx->lock(); }

  unique_lock(Mutex &m, defer_lock_t) noexcept : m_mtx(std::addressof(m)), m_owns(false) {}

  unique_lock(Mutex &m, try_to_lock_t) : m_mtx(std::addressof(m)) { m_owns = m_mtx->try_lock(); }

  unique_lock(Mutex &m, adopt_lock_t) noexcept : m_mtx(std::addressof(m)), m_owns(true) {}

  unique_lock(unique_lock &&other) noexcept
      : m_mtx(std::exchange(other.m_mtx, nullptr)),
        m_owns(std::exchange(other.m_owns, false)) {}

  unique_lock &operator=(unique_lock &&other) noexcept {
    if (this != std::addressof(other)) {
      if (m_owns)
        m_mtx->unlock();
      m_mtx = std::exchange(other.m_mtx, nullptr);
      m_owns = std::exchange(other.m_owns, false);
    }
    return *this;
  }

  ~unique_lock() noexcept {
    if (m_owns) {
      m_mtx->unlock();
    }
  }

  unique_lock(const unique_lock &) = delete;
  unique_lock &operator=(const unique_lock &) = delete;

  void lock() {
    if (m_mtx == nullptr) {
      return;
    }
    if (m_owns) {
      return;
    }
    m_mtx->lock();
    m_owns = true;
  }

  bool try_lock() {
    if (m_mtx == nullptr || m_owns)
      return false;
    bool success = m_mtx->try_lock();
    if (success)
      m_owns = true;
    return success;
  }

  void unlock() {
    if (m_mtx != nullptr && m_owns) {
      m_mtx->unlock();
      m_owns = false;
    }
  }

  Mutex *release() noexcept {
    Mutex *ret = m_mtx;
    m_mtx = nullptr;
    m_owns = false;
    return ret;
  }

  bool owns_lock() const noexcept { return m_owns; }
  explicit operator bool() const noexcept { return m_owns; }
  const Mutex *mutex() const noexcept { return m_mtx; }
  Mutex *mutex() noexcept { return m_mtx; }

private:
  Mutex *m_mtx{nullptr};
  bool m_owns{false};
};

template <typename Lock1> constexpr bool try_lock(Lock1 &L1) {
  return L1.try_lock();
}

template <typename Lock1, typename Lock2>
constexpr bool try_lock(Lock1 &L1, Lock2 &L2) {
  if (!L1.try_lock())
    return false;
  if (!L2.try_lock()) {
    L1.unlock();
    return false;
  }
  return true;
}

template <typename Lock1, typename Lock2, typename Lock3>
constexpr bool try_lock(Lock1 &L1, Lock2 &L2, Lock3 &L3) {
  if (!L1.try_lock())
    return false;
  if (!L2.try_lock()) {
    L1.unlock();
    return false;
  }
  if (!L3.try_lock()) {
    L1.unlock();
    L2.unlock();
    return false;
  }
  return true;
}

} // namespace gdut

#endif
