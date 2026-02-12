#ifndef BSP_ATOMIC_HPP
#define BSP_ATOMIC_HPP

#include <stdint.h>
#include <string.h>
#include <type_traits>

#define ETL_USE_ATOMIC_BUILTINS

namespace gdut {
#if defined(ETL_USE_ATOMIC_BUILTINS)

#define ETL_BUILTIN_LOCK                                                       \
  do {                                                                         \
    while (__atomic_test_and_set(&flag, gdut::memory_order_seq_cst)) {         \
    }                                                                          \
  } while (0)
#define ETL_BUILTIN_UNLOCK                                                     \
  do {                                                                         \
    __atomic_clear(&flag, gdut::memory_order_seq_cst);                         \
  } while (0)

//***************************************************************************
// Atomic type for pre C++11 GCC compilers that support the builtin '__atomic'
// functions. Only integral and pointer types are supported.
//***************************************************************************

typedef enum memory_order {
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

template <bool Is_Always_Lock_Free> struct atomic_traits {
  static constexpr bool is_always_lock_free = Is_Always_Lock_Free;
};

template <bool Is_Always_Lock_Free>
constexpr bool atomic_traits<Is_Always_Lock_Free>::is_always_lock_free;

//***************************************************************************
/// For all types except bool, pointers and types that are always lock free.
//***************************************************************************
template <typename T, bool integral_type = std::is_integral<T>::value>
class atomic : public atomic_traits<integral_type> {
public:
  atomic() : value(T()) {}

  atomic(T v) : value(v) {}

  // Assignment
  T operator=(T v) {
    store(v);

    return v;
  }

  T operator=(T v) volatile {
    store(v);

    return v;
  }

  // Pre-increment
  T operator++() {
    return __atomic_add_fetch(&value, 1, gdut::memory_order_seq_cst);
  }

  T operator++() volatile {
    return __atomic_add_fetch(&value, 1, gdut::memory_order_seq_cst);
  }

  // Post-increment
  T operator++(int) {
    return __atomic_fetch_add(&value, 1, gdut::memory_order_seq_cst);
  }

  T operator++(int) volatile {
    return __atomic_fetch_add(&value, 1, gdut::memory_order_seq_cst);
  }

  // Pre-decrement
  T operator--() {
    return __atomic_sub_fetch(&value, 1, gdut::memory_order_seq_cst);
  }

  T operator--() volatile {
    return __atomic_sub_fetch(&value, 1, gdut::memory_order_seq_cst);
  }

  // Post-decrement
  T operator--(int) {
    return __atomic_fetch_sub(&value, 1, gdut::memory_order_seq_cst);
  }

  T operator--(int) volatile {
    return __atomic_fetch_sub(&value, 1, gdut::memory_order_seq_cst);
  }

  // Add
  T operator+=(T v) {
    return __atomic_fetch_add(&value, v, gdut::memory_order_seq_cst);
  }

  T operator+=(T v) volatile {
    return __atomic_fetch_add(&value, v, gdut::memory_order_seq_cst);
  }

  // Subtract
  T operator-=(T v) {
    return __atomic_fetch_sub(&value, v, gdut::memory_order_seq_cst);
  }

  T operator-=(T v) volatile {
    return __atomic_fetch_sub(&value, v, gdut::memory_order_seq_cst);
  }

  // And
  T operator&=(T v) {
    return __atomic_fetch_and(&value, v, gdut::memory_order_seq_cst);
  }

  T operator&=(T v) volatile {
    return __atomic_fetch_and(&value, v, gdut::memory_order_seq_cst);
  }

  // Or
  T operator|=(T v) {
    return __atomic_fetch_or(&value, v, gdut::memory_order_seq_cst);
  }

  T operator|=(T v) volatile {
    return __atomic_fetch_or(&value, v, gdut::memory_order_seq_cst);
  }

  // Exclusive or
  T operator^=(T v) {
    return __atomic_fetch_xor(&value, v, gdut::memory_order_seq_cst);
  }

  T operator^=(T v) volatile {
    return __atomic_fetch_xor(&value, v, gdut::memory_order_seq_cst);
  }

  // Conversion operator
  operator T() const {
    return __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst);
  }

  operator T() volatile const {
    return __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst);
  }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    __atomic_store_n(&value, v, order);
  }

  void store(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    __atomic_store_n(&value, v, order);
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    return __atomic_load_n(&value, order);
  }

  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const volatile {
    return __atomic_load_n(&value, order);
  }

  // Fetch add
  T fetch_add(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_fetch_add(&value, v, order);
  }

  T fetch_add(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_fetch_add(&value, v, order);
  }

  // Fetch subtract
  T fetch_sub(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_fetch_sub(&value, v, order);
  }

  T fetch_sub(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_fetch_sub(&value, v, order);
  }

  // Fetch or
  T fetch_or(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_fetch_or(&value, v, order);
  }

  T fetch_or(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_fetch_or(&value, v, order);
  }

  // Fetch and
  T fetch_and(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_fetch_and(&value, v, order);
  }

  T fetch_and(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_fetch_and(&value, v, order);
  }

  // Fetch exclusive or
  T fetch_xor(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_fetch_xor(&value, v, order);
  }

  T fetch_xor(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_fetch_xor(&value, v, order);
  }

  // Exchange
  T exchange(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_exchange_n(&value, v, order);
  }

  T exchange(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_exchange_n(&value, v, order);
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T &expected, T desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_compare_exchange_n(&value, &expected, desired, true, order,
                                       order);
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_compare_exchange_n(&value, &expected, desired, true, order,
                                       order);
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) {
    return __atomic_compare_exchange_n(&value, &expected, desired, true,
                                       success, failure);
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    return __atomic_compare_exchange_n(&value, &expected, desired, true,
                                       success, failure);
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    return __atomic_compare_exchange_n(&value, &expected, desired, false, order,
                                       order);
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return __atomic_compare_exchange_n(&value, &expected, desired, false, order,
                                       order);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    return __atomic_compare_exchange_n(&value, &expected, desired, false,
                                       success, failure);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    return __atomic_compare_exchange_n(&value, &expected, desired, false,
                                       success, failure);
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable T value;
};

//***************************************************************************
/// Specialisation for pointers
//***************************************************************************
template <typename T> class atomic<T *, false> : public atomic_traits<true> {
public:
  atomic() : value(0U) {}

  atomic(T *v) : value(uintptr_t(v)) {}

  // Assignment
  T *operator=(T *v) {
    store(v);

    return v;
  }

  T *operator=(T *v) volatile {
    store(v);

    return v;
  }

  // Pre-increment
  T *operator++() {
    return reinterpret_cast<T *>(
        __atomic_add_fetch(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator++() volatile {
    return reinterpret_cast<T *>(
        __atomic_add_fetch(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  // Post-increment
  T *operator++(int) {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator++(int) volatile {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  // Pre-decrement
  T *operator--() {
    return reinterpret_cast<T *>(
        __atomic_sub_fetch(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator--() volatile {
    return reinterpret_cast<T *>(
        __atomic_sub_fetch(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  // Post-decrement
  T *operator--(int) {
    return reinterpret_cast<T *>(
        __atomic_fetch_sub(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator--(int) volatile {
    return reinterpret_cast<T *>(
        __atomic_fetch_sub(&value, sizeof(T), gdut::memory_order_seq_cst));
  }

  // Add
  T *operator+=(ptrdiff_t v) {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, v * sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator+=(ptrdiff_t v) volatile {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, v * sizeof(T), gdut::memory_order_seq_cst));
  }

  // Subtract
  T *operator-=(ptrdiff_t v) {
    return reinterpret_cast<T *>(
        __atomic_fetch_sub(&value, v * sizeof(T), gdut::memory_order_seq_cst));
  }

  T *operator-=(ptrdiff_t v) volatile {
    return reinterpret_cast<T *>(
        __atomic_fetch_sub(&value, v * sizeof(T), gdut::memory_order_seq_cst));
  }

  // Conversion operator
  operator T *() const {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst));
  }

  operator T *() volatile const {
    return reinterpret_cast<T *>(
        __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst));
  }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(T *v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    __atomic_store_n(&value, uintptr_t(v), order);
  }

  void store(T *v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    __atomic_store_n(&value, uintptr_t(v), order);
  }

  // Load
  T *load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    return reinterpret_cast<T *>(__atomic_load_n(&value, order));
  }

  T *load(gdut::memory_order order = gdut::memory_order_seq_cst) const
      volatile {
    return reinterpret_cast<T *>(__atomic_load_n(&value, order));
  }

  // Fetch add
  T *fetch_add(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(__atomic_fetch_add(&value, v, order));
  }

  T *fetch_add(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(__atomic_fetch_add(&value, v, order));
  }

  // Fetch subtract
  T *fetch_sub(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(__atomic_fetch_sub(&value, v, order));
  }

  T *fetch_sub(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(__atomic_fetch_sub(&value, v, order));
  }

  // Exchange
  T *exchange(T *v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(
        __atomic_exchange_n(&value, uintptr_t(v), order));
  }

  T *exchange(T *v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(
        __atomic_exchange_n(&value, uintptr_t(v), order));
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T *&expected, T *desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       true, order, order);
  }

  bool compare_exchange_weak(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       true, order, order);
  }

  bool compare_exchange_weak(T *&expected, T *desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       true, success, failure);
  }

  bool compare_exchange_weak(T *&expected, T *desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       true, success, failure);
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       false, order, order);
  }

  bool compare_exchange_strong(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       false, order, order);
  }

  bool compare_exchange_strong(T *&expected, T *desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       false, success, failure);
  }

  bool compare_exchange_strong(T *&expected, T *desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    uintptr_t expected_v = uintptr_t(expected);

    return __atomic_compare_exchange_n(&value, &expected_v, uintptr_t(desired),
                                       false, success, failure);
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable uintptr_t value;
};

//***************************************************************************
/// Specialisation for bool
//***************************************************************************
template <> class atomic<bool, true> : public atomic_traits<true> {
public:
  atomic() : value(0U) {}

  atomic(bool v) : value(char(v)) {}

  // Assignment
  bool operator=(bool v) {
    store(v);

    return v;
  }

  bool operator=(bool v) volatile {
    store(v);

    return v;
  }

  // Conversion operator
  operator bool() const {
    return static_cast<bool>(
        __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst));
  }

  operator bool() volatile const {
    return static_cast<bool>(
        __atomic_fetch_add(&value, 0, gdut::memory_order_seq_cst));
  }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(bool v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    __atomic_store_n(&value, char(v), order);
  }

  void store(bool v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    __atomic_store_n(&value, char(v), order);
  }

  // Load
  bool load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    return static_cast<bool>(__atomic_load_n(&value, order));
  }

  bool load(gdut::memory_order order = gdut::memory_order_seq_cst) const
      volatile {
    return static_cast<bool>(__atomic_load_n(&value, order));
  }

  // Exchange
  bool exchange(bool v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return static_cast<bool>(__atomic_exchange_n(&value, char(v), order));
  }

  bool
  exchange(bool v,
           gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return static_cast<bool>(__atomic_exchange_n(&value, char(v), order));
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(bool &expected, bool desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, true,
                                       order, order);
  }

  bool compare_exchange_weak(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, true,
                                       order, order);
  }

  bool compare_exchange_weak(bool &expected, bool desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, true,
                                       success, failure);
  }

  bool compare_exchange_weak(bool &expected, bool desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, true,
                                       success, failure);
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, false,
                                       order, order);
  }

  bool compare_exchange_strong(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, false,
                                       order, order);
  }

  bool compare_exchange_strong(bool &expected, bool desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, false,
                                       success, failure);
  }

  bool compare_exchange_strong(bool &expected, bool desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    char expected_v = char(expected);
    char desired_v = char(desired);

    return __atomic_compare_exchange_n(&value, &expected_v, desired_v, false,
                                       success, failure);
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable char value;
};

//***************************************************************************
/// Specialisation for type that are not integral, pointer or bool.
/// Uses a mutex to control access.
//***************************************************************************
template <typename T> class atomic<T, false> : public atomic_traits<false> {
public:
  atomic() : flag(0), value(T()) {}

  atomic(T v) : flag(0), value(v) {}

  // Assignment
  T operator=(T v) {
    store(v);

    return v;
  }

  T operator=(T v) volatile {
    store(v);

    return v;
  }

  // Conversion operator
  operator T() const {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  operator T() volatile const {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Is lock free?
  bool is_lock_free() const { return false; }

  bool is_lock_free() const volatile { return false; }

  // Store
  void store(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    ETL_BUILTIN_LOCK;
    value = v;
    ETL_BUILTIN_UNLOCK;
  }

  void store(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    ETL_BUILTIN_LOCK;
    value = v;
    ETL_BUILTIN_UNLOCK;
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const volatile {
    (void)order;
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    (void)order;
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Exchange
  T exchange(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    ETL_BUILTIN_LOCK;
    T result = value;
    value = v;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  T exchange(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    ETL_BUILTIN_LOCK;
    T result = value;
    value = v;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T &expected, T desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    bool result;

    (void)order;
    ETL_BUILTIN_LOCK;
    if (memcmp(&value, &expected, sizeof(T)) == 0) {
      value = desired;
      result = true;
    } else {
      result = false;
    }
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    bool result;

    (void)order;
    ETL_BUILTIN_LOCK;
    if (memcmp(&value, &expected, sizeof(T)) == 0) {
      value = desired;
      result = true;
    } else {
      result = false;
    }
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) {
    (void)success;
    (void)failure;
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    (void)success;
    (void)failure;
    return compare_exchange_weak(expected, desired);
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    (void)success;
    (void)failure;
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    (void)success;
    (void)failure;
    return compare_exchange_weak(expected, desired);
  }

private:
  mutable char flag;
  mutable T value;
};

#undef ETL_BUILTIN_LOCK
#undef ETL_BUILTIN_UNLOCK

#endif

#if defined(ETL_USE_SYNC_BUILTINS)

#define ETL_BUILTIN_LOCK                                                       \
  do {                                                                         \
    while (__sync_lock_test_and_set(&flag, 1U)) {                              \
    }                                                                          \
  } while (0)
#define ETL_BUILTIN_UNLOCK                                                     \
  do {                                                                         \
    __sync_lock_release(&flag);                                                \
  } while (0)

//***************************************************************************
// Atomic type for pre C++11 GCC compilers that support the builtin '__sync'
// functions. Only integral and pointer types are supported.
//***************************************************************************

typedef enum memory_order {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
} memory_order;

template <bool Is_Always_Lock_Free> struct atomic_traits {
  static constexpr bool is_always_lock_free = Is_Always_Lock_Free;
};

template <bool Is_Always_Lock_Free>
constexpr bool atomic_traits<Is_Always_Lock_Free>::is_always_lock_free;

//***************************************************************************
/// For all types except bool and pointers
//***************************************************************************
template <typename T, bool integral_type = gdut::is_integral<T>::value>
class atomic : public atomic_traits<integral_type> {
public:
  ETL_STATIC_ASSERT(gdut::is_integral<T>::value,
                    "Only integral types are supported");

  atomic() : value(0) {}

  atomic(T v) : value(v) {}

  // Assignment
  T operator=(T v) {
    store(v);

    return v;
  }

  T operator=(T v) volatile {
    store(v);

    return v;
  }

  // Pre-increment
  T operator++() { return __sync_add_and_fetch(&value, 1); }

  T operator++() volatile { return __sync_add_and_fetch(&value, 1); }

  // Post-increment
  T operator++(int) { return __sync_fetch_and_add(&value, 1); }

  T operator++(int) volatile { return __sync_fetch_and_add(&value, 1); }

  // Pre-decrement
  T operator--() { return __sync_sub_and_fetch(&value, 1); }

  T operator--() volatile { return __sync_sub_and_fetch(&value, 1); }

  // Post-decrement
  T operator--(int) { return __sync_fetch_and_sub(&value, 1); }

  T operator--(int) volatile { return __sync_fetch_and_sub(&value, 1); }

  // Add
  T operator+=(T v) { return __sync_fetch_and_add(&value, v); }

  T operator+=(T v) volatile { return __sync_fetch_and_add(&value, v); }

  // Subtract
  T operator-=(T v) { return __sync_fetch_and_sub(&value, v); }

  T operator-=(T v) volatile { return __sync_fetch_and_sub(&value, v); }

  // And
  T operator&=(T v) { return __sync_fetch_and_and(&value, v); }

  T operator&=(T v) volatile { return __sync_fetch_and_and(&value, v); }

  // Or
  T operator|=(T v) { return __sync_fetch_and_or(&value, v); }

  T operator|=(T v) volatile { return __sync_fetch_and_or(&value, v); }

  // Exclusive or
  T operator^=(T v) { return __sync_fetch_and_xor(&value, v); }

  T operator^=(T v) volatile { return __sync_fetch_and_xor(&value, v); }

  // Conversion operator
  operator T() const { return __sync_fetch_and_add(&value, 0); }

  operator T() volatile const { return __sync_fetch_and_add(&value, 0); }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    (void)__sync_lock_test_and_set(&value, v);
  }

  void store(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    (void)__sync_lock_test_and_set(&value, v);
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    (void)order;
    return __sync_fetch_and_add(&value, 0);
  }

  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const volatile {
    (void)order;
    return __sync_fetch_and_add(&value, 0);
  }

  // Fetch add
  T fetch_add(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_fetch_and_add(&value, v);
  }

  T fetch_add(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_fetch_and_add(&value, v);
  }

  // Fetch subtract
  T fetch_sub(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_fetch_and_sub(&value, v);
  }

  T fetch_sub(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_fetch_and_sub(&value, v);
  }

  // Fetch or
  T fetch_or(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_fetch_and_or(&value, v);
  }

  T fetch_or(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_fetch_and_or(&value, v);
  }

  // Fetch and
  T fetch_and(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_fetch_and_and(&value, v);
  }

  T fetch_and(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_fetch_and_and(&value, v);
  }

  // Fetch exclusive or
  T fetch_xor(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_fetch_and_xor(&value, v);
  }

  T fetch_xor(T v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_fetch_and_xor(&value, v);
  }

  // Exchange
  T exchange(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    return __sync_lock_test_and_set(&value, v);
  }

  T exchange(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    return __sync_lock_test_and_set(&value, v);
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T &expected, T desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    T old = __sync_val_compare_and_swap(&value, expected, desired);

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    T old = __sync_val_compare_and_swap(&value, expected, desired);

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) {
    (void)success;
    (void)failure;
    T old = __sync_val_compare_and_swap(&value, expected, desired);

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    (void)success;
    (void)failure;
    T old = __sync_val_compare_and_swap(&value, expected, desired);

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    (void)order;
    T old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    (void)order;
    T old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    (void)success;
    (void)failure;
    T old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    (void)success;
    (void)failure;
    T old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable volatile T value;
};

//***************************************************************************
/// Specialisation for pointers
//***************************************************************************
template <typename T> class atomic<T *, false> : public atomic_traits<true> {
public:
  atomic() : value(0U) {}

  atomic(T *v) : value(uintptr_t(v)) {}

  // Assignment
  T *operator=(T *v) {
    store(v);

    return v;
  }

  T *operator=(T *v) volatile {
    store(v);

    return v;
  }

  // Pre-increment
  T *operator++() {
    return reinterpret_cast<T *>(__sync_add_and_fetch(&value, sizeof(T)));
  }

  T *operator++() volatile {
    return reinterpret_cast<T *>(__sync_add_and_fetch(&value, sizeof(T)));
  }

  // Post-increment
  T *operator++(int) {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, sizeof(T)));
  }

  T *operator++(int) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, sizeof(T)));
  }

  // Pre-decrement
  T *operator--() {
    return reinterpret_cast<T *>(__sync_sub_and_fetch(&value, sizeof(T)));
  }

  T *operator--() volatile {
    return reinterpret_cast<T *>(__sync_sub_and_fetch(&value, sizeof(T)));
  }

  // Post-decrement
  T *operator--(int) {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, sizeof(T)));
  }

  T *operator--(int) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, sizeof(T)));
  }

  // Add
  T *operator+=(ptrdiff_t v) {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, v * sizeof(T)));
  }

  T *operator+=(ptrdiff_t v) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, v * sizeof(T)));
  }

  // Subtract
  T *operator-=(ptrdiff_t v) {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, v * sizeof(T)));
  }

  T *operator-=(ptrdiff_t v) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, v * sizeof(T)));
  }

  // Conversion operator
  operator T *() const {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, 0));
  }

  operator T *() volatile const {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, 0));
  }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(T *v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    __sync_lock_test_and_set(&value, uintptr_t(v));
  }

  void store(T *v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    __sync_lock_test_and_set(&value, uintptr_t(v));
  }

  // Load
  T *load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, 0));
  }

  T *load(gdut::memory_order order = gdut::memory_order_seq_cst) const
      volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, 0));
  }

  // Fetch add
  T *fetch_add(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, v));
  }

  T *fetch_add(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_add(&value, v));
  }

  // Fetch subtract
  T *fetch_sub(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, v));
  }

  T *fetch_sub(ptrdiff_t v,
               gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(__sync_fetch_and_sub(&value, v));
  }

  // Exchange
  T *exchange(T *v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return reinterpret_cast<T *>(
        __sync_lock_test_and_set(&value, uintptr_t(v)));
  }

  T *exchange(T *v,
              gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return reinterpret_cast<T *>(
        __sync_lock_test_and_set(&value, uintptr_t(v)));
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T *&expected, T *desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    T *old = reinterpret_cast<T *>(__sync_val_compare_and_swap(
        &value, uintptr_t(expected), uintptr_t(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    T *old = reinterpret_cast<T *>(__sync_val_compare_and_swap(
        &value, uintptr_t(expected), uintptr_t(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(T *&expected, T *desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) {
    T *old = reinterpret_cast<T *>(__sync_val_compare_and_swap(
        &value, uintptr_t(expected), uintptr_t(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(T *&expected, T *desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    T *old = reinterpret_cast<T *>(__sync_val_compare_and_swap(
        &value, uintptr_t(expected), uintptr_t(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    T *old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T *))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(
      T *&expected, T *desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    T *old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T *))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(T *&expected, T *desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    T *old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T *))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(T *&expected, T *desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    T *old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(T *))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable uintptr_t value;
};

//***************************************************************************
/// Specialisation for bool
//***************************************************************************
template <> class atomic<bool, true> : public atomic_traits<true> {
public:
  atomic() : value(0U) {}

  atomic(bool v) : value(char(v)) {}

  // Assignment
  bool operator=(bool v) {
    store(v);

    return v;
  }

  bool operator=(bool v) volatile {
    store(v);

    return v;
  }

  // Conversion operator
  operator bool() const {
    return static_cast<bool>(__sync_fetch_and_add(&value, 0));
  }

  operator bool() volatile const {
    return static_cast<bool>(__sync_fetch_and_add(&value, 0));
  }

  // Is lock free?
  bool is_lock_free() const { return true; }

  bool is_lock_free() const volatile { return true; }

  // Store
  void store(bool v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    __sync_lock_test_and_set(&value, char(v));
  }

  void store(bool v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    __sync_lock_test_and_set(&value, char(v));
  }

  // Load
  bool load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    return static_cast<bool>(__sync_fetch_and_add(&value, 0));
  }

  bool load(gdut::memory_order order = gdut::memory_order_seq_cst) const
      volatile {
    return static_cast<bool>(__sync_fetch_and_add(&value, 0));
  }

  // Exchange
  bool exchange(bool v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    return static_cast<bool>(__sync_lock_test_and_set(&value, char(v)));
  }

  bool
  exchange(bool v,
           gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return static_cast<bool>(__sync_lock_test_and_set(&value, char(v)));
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(bool &expected, bool desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    bool old = static_cast<bool>(
        __sync_val_compare_and_swap(&value, char(expected), char(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    bool old = static_cast<bool>(
        __sync_val_compare_and_swap(&value, char(expected), char(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(bool &expected, bool desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) {
    bool old = static_cast<bool>(
        __sync_val_compare_and_swap(&value, char(expected), char(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  bool compare_exchange_weak(bool &expected, bool desired,
                             gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    bool old = static_cast<bool>(
        __sync_val_compare_and_swap(&value, char(expected), char(desired)));

    if (old == expected) {
      return true;
    } else {
      expected = old;
      return false;
    }
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    bool old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(bool))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(
      bool &expected, bool desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    bool old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(bool))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(bool &expected, bool desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    bool old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(bool))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

  bool compare_exchange_strong(bool &expected, bool desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    bool old = expected;

    while (!compare_exchange_weak(old, desired)) {
      if (memcmp(&old, &expected, sizeof(bool))) {
        expected = old;
        return false;
      }
    }

    return true;
  }

private:
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  mutable char value;
};

//***************************************************************************
/// Specialisation for type that are not integral, pointer or bool.
/// Uses a mutex to control access.
//***************************************************************************
template <typename T> class atomic<T, false> : public atomic_traits<false> {
public:
  atomic() : flag(0), value(T()) {}

  atomic(T v) : flag(0), value(v) {}

  // Assignment
  T operator=(T v) {
    store(v);

    return v;
  }

  T operator=(T v) volatile {
    store(v);

    return v;
  }

  // Conversion operator
  operator T() const {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  operator T() volatile const {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Is lock free?
  bool is_lock_free() const { return false; }

  bool is_lock_free() const volatile { return false; }

  // Store
  void store(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    ETL_BUILTIN_LOCK;
    value = v;
    ETL_BUILTIN_UNLOCK;
  }

  void store(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    ETL_BUILTIN_LOCK;
    value = v;
    ETL_BUILTIN_UNLOCK;
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const volatile {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Load
  T load(gdut::memory_order order = gdut::memory_order_seq_cst) const {
    ETL_BUILTIN_LOCK;
    T result = value;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Exchange
  T exchange(T v, gdut::memory_order order = gdut::memory_order_seq_cst) {
    ETL_BUILTIN_LOCK;
    T result = value;
    value = v;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  T exchange(T v,
             gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    ETL_BUILTIN_LOCK;
    T result = value;
    value = v;
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  // Compare exchange weak
  bool
  compare_exchange_weak(T &expected, T desired,
                        gdut::memory_order order = gdut::memory_order_seq_cst) {
    bool result;

    ETL_BUILTIN_LOCK;
    if (memcmp(&value, &expected, sizeof(T)) == 0) {
      value = desired;
      result = true;
    } else {
      result = false;
    }
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    bool result;

    ETL_BUILTIN_LOCK;
    if (memcmp(&value, &expected, sizeof(T)) == 0) {
      value = desired;
      result = true;
    } else {
      result = false;
    }
    ETL_BUILTIN_UNLOCK;

    return result;
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) {
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_weak(T &expected, T desired, gdut::memory_order success,
                             gdut::memory_order failure) volatile {
    return compare_exchange_weak(expected, desired);
  }

  // Compare exchange strong
  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) {
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      gdut::memory_order order = gdut::memory_order_seq_cst) volatile {
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) {
    return compare_exchange_weak(expected, desired);
  }

  bool compare_exchange_strong(T &expected, T desired,
                               gdut::memory_order success,
                               gdut::memory_order failure) volatile {
    return compare_exchange_weak(expected, desired);
  }

private:
  mutable char flag;
  mutable T value;
};

#undef ETL_SYNC_BUILTIN_LOCK
#undef ETL_SYNC_BUILTIN_UNLOCK

#endif

typedef gdut::atomic<bool> atomic_bool;
typedef gdut::atomic<char> atomic_char;
typedef gdut::atomic<signed char> atomic_schar;
typedef gdut::atomic<unsigned char> atomic_uchar;
typedef gdut::atomic<short> atomic_short;
typedef gdut::atomic<unsigned short> atomic_ushort;
typedef gdut::atomic<int> atomic_int;
typedef gdut::atomic<unsigned int> atomic_uint;
typedef gdut::atomic<long> atomic_long;
typedef gdut::atomic<unsigned long> atomic_ulong;
typedef gdut::atomic<long long> atomic_llong;
typedef gdut::atomic<unsigned long long> atomic_ullong;
typedef gdut::atomic<wchar_t> atomic_wchar_t;
#if ETL_HAS_NATIVE_CHAR8_T
typedef gdut::atomic<char8_t> atomic_char8_t;
#endif
#if ETL_HAS_NATIVE_CHAR16_T
typedef gdut::atomic<char16_t> atomic_char16_t;
#endif
#if ETL_HAS_NATIVE_CHAR32_T
typedef gdut::atomic<char32_t> atomic_char32_t;
#endif
#if ETL_USING_8BIT_TYPES
typedef gdut::atomic<uint8_t> atomic_uint8_t;
typedef gdut::atomic<int8_t> atomic_int8_t;
#endif
typedef gdut::atomic<uint16_t> atomic_uint16_t;
typedef gdut::atomic<int16_t> atomic_int16_t;
typedef gdut::atomic<uint32_t> atomic_uint32_t;
typedef gdut::atomic<int32_t> atomic_int32_t;
#if ETL_USING_64BIT_TYPES
typedef gdut::atomic<uint64_t> atomic_uint64_t;
typedef gdut::atomic<int64_t> atomic_int64_t;
#endif
typedef gdut::atomic<int_least8_t> atomic_int_least8_t;
typedef gdut::atomic<uint_least8_t> atomic_uint_least8_t;
typedef gdut::atomic<int_least16_t> atomic_int_least16_t;
typedef gdut::atomic<uint_least16_t> atomic_uint_least16_t;
typedef gdut::atomic<int_least32_t> atomic_int_least32_t;
typedef gdut::atomic<uint_least32_t> atomic_uint_least32_t;
#if ETL_USING_64BIT_TYPES
typedef gdut::atomic<int_least64_t> atomic_int_least64_t;
typedef gdut::atomic<uint_least64_t> atomic_uint_least64_t;
#endif
typedef gdut::atomic<int_fast8_t> atomic_int_fast8_t;
typedef gdut::atomic<uint_fast8_t> atomic_uint_fast8_t;
typedef gdut::atomic<int_fast16_t> atomic_int_fast16_t;
typedef gdut::atomic<uint_fast16_t> atomic_uint_fast16_t;
typedef gdut::atomic<int_fast32_t> atomic_int_fast32_t;
typedef gdut::atomic<uint_fast32_t> atomic_uint_fast32_t;
#if ETL_USING_64BIT_TYPES
typedef gdut::atomic<int_fast64_t> atomic_int_fast64_t;
typedef gdut::atomic<uint_fast64_t> atomic_uint_fast64_t;
#endif
typedef gdut::atomic<intptr_t> atomic_intptr_t;
typedef gdut::atomic<uintptr_t> atomic_uintptr_t;
typedef gdut::atomic<size_t> atomic_size_t;
typedef gdut::atomic<ptrdiff_t> atomic_ptrdiff_t;
typedef gdut::atomic<intmax_t> atomic_intmax_t;
typedef gdut::atomic<uintmax_t> atomic_uintmax_t;
} // namespace gdut

#endif
