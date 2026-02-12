#ifndef GDUT_MUTEX_CMSIS_RTOS2_INCLUDED
#define GDUT_MUTEX_CMSIS_RTOS2_INCLUDED

#include <cmsis_os2.h>
#include <memory>
#include <utility>

namespace gdut {

class mutex {
public:
  mutex() : id(NULL) {
    osMutexAttr_t attr = {
        "GDUT", osMutexRecursive | osMutexPrioInherit | osMutexRobust, 0, 0};
    id = osMutexNew(&attr);
  }

  mutex(const mutex &) = delete;
  mutex &operator=(const mutex &) = delete;

  mutex(mutex &&mtx) noexcept {
    id = mtx.id;
    mtx.id = nullptr;
  }
  mutex &operator=(mutex &&mtx) noexcept {
    if (this != &mtx) {
      if (id != nullptr) {
        osMutexDelete(id);
      }
      id = mtx.id;
      mtx.id = nullptr;
    }
    return *this;
  }

  ~mutex() { osMutexDelete(id); }

  void lock() { osMutexAcquire(id, osWaitForever); }

  bool try_lock() { return osMutexAcquire(id, 0) == osOK; }

  void unlock() { osMutexRelease(id); }

private:
  osMutexId_t id;
};

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

  ~lock_guard() { m_mtx.unlock(); }

private:
  Mutex &m_mtx;
};

template <typename Mutex> class unique_lock {
public:
  unique_lock() noexcept = default;

  explicit unique_lock(Mutex &m) : mtx(&m), owns(true) { mtx->lock(); }

  unique_lock(Mutex &m, defer_lock_t) noexcept : mtx(&m), owns(false) {}

  unique_lock(Mutex &m, try_to_lock_t) : mtx(&m) { owns = mtx->try_lock(); }

  unique_lock(Mutex &m, adopt_lock_t) noexcept : mtx(&m), owns(true) {}

  unique_lock(unique_lock &&other) noexcept
      : mtx(std::exchange(other.mtx, nullptr)),
        owns(std::exchange(other.owns, false)) {}

  unique_lock &operator=(unique_lock &&other) noexcept {
    if (this != &other) {
      if (owns)
        mtx->unlock();
      mtx = std::exchange(other.mtx, nullptr);
      owns = std::exchange(other.owns, false);
    }
    return *this;
  }

  ~unique_lock() {
    if (owns) {
      mtx->unlock();
    }
  }

  unique_lock(const unique_lock &) = delete;
  unique_lock &operator=(const unique_lock &) = delete;

  void lock() {
    if (mtx == nullptr) {
      return;
    }
    if (owns) {
      return;
    }
    mtx->lock();
    owns = true;
  }

  bool try_lock() {
    if (mtx == nullptr || owns)
      return false;
    bool success = mtx->try_lock();
    if (success)
      owns = true;
    return success;
  }

  void unlock() {
    if (mtx != nullptr && owns) {
      mtx->unlock();
      owns = false;
    }
  }

  Mutex *release() noexcept {
    Mutex *ret = mtx;
    mtx = nullptr;
    owns = false;
    return ret;
  }

  bool owns_lock() const noexcept { return owns; }
  explicit operator bool() const noexcept { return owns; }
  const Mutex *mutex() const noexcept { return mtx; }
  Mutex *mutex() noexcept { return mtx; }

private:
  Mutex *mtx{nullptr};
  bool owns{false};
};

template <typename Lock1> bool try_lock(Lock1 &L1) { return L1.try_lock(); }

template <typename Lock1, typename Lock2> bool try_lock(Lock1 &L1, Lock2 &L2) {
  if (!L1.try_lock())
    return false;
  if (!L2.try_lock()) {
    L1.unlock();
    return false;
  }
  return true;
}

template <typename Lock1, typename Lock2, typename Lock3>
bool try_lock(Lock1 &L1, Lock2 &L2, Lock3 &L3) {
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
