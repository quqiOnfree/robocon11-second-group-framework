#ifndef BSP_SHARED_PTR_HPP
#define BSP_SHARED_PTR_HPP

#include "bsp_atomic.hpp"
#include "bsp_memorypool.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace gdut {

// 控制块基类（扩展版）
struct control_block_base {
  // 两个原子计数
  atomic<long> shared_count{1}; // 共享引用计数（对象存活计数）
  atomic<long> weak_count{0};   // 弱引用计数（控制块存活计数）

  control_block_base() = default;

  virtual ~control_block_base() = default;

  // 销毁管理对象（由 shared_ptr 在 shared_count 归零时调用）
  virtual void dispose() = 0;

  // 释放控制块自身（由最后一个弱引用或 shared_ptr 在 weak_count 归零时调用）
  virtual void deallocate() = 0;
};

// 分离分配的控制块（对象在堆上，控制块在池中）
template <typename T, typename Deleter>
class control_block_separate : public control_block_base {
  T *m_ptr; // 指向托管对象的原始指针
  Deleter m_deleter;

public:
  control_block_separate(T *p, Deleter &&del)
      : m_ptr(p), m_deleter(std::move(del)) {
    // shared_count 已初始为 1
  }

  void dispose() override {
    m_deleter(m_ptr); // 执行删除器
  }

  void deallocate() override {
    pmr::polymorphic_allocator<>{}.delete_object(this); // 释放控制块
  }
};

// 合并分配的控制块（对象嵌入控制块，一次分配）
template <typename T> class control_block_combined : public control_block_base {
  alignas(T) unsigned char storage[sizeof(T)];

public:
  template <typename... Args> control_block_combined(Args &&...args) {
    new (storage) T(std::forward<Args>(args)...);
    // shared_count 已初始为 1
  }

  T *get() { return reinterpret_cast<T *>(storage); }

  void dispose() override {
    get()->~T(); // 显式析构对象
  }

  void deallocate() override {
    pmr::polymorphic_allocator<>{}.delete_object(this); // 释放控制块
  }
};

template <typename T> struct default_deleter {
  void operator()(T *ptr) const noexcept {
    pmr::polymorphic_allocator<T>{}.delete_object(ptr);
  }
};

template <typename T> class weak_ptr; // 前向声明

template <typename T> class enable_shared_from_this; // 前向声明

template <typename T> class shared_ptr {
  T *m_ptr;                            // 存储指针
  control_block_base *m_control_block; // 控制块指针

  friend class weak_ptr<T>; // 友元声明，以便 weak_ptr 可以访问私有成员

public:
  shared_ptr() noexcept : m_ptr(nullptr), m_control_block(nullptr) {}

  template <typename Deleter = default_deleter<T>>
    requires std::is_invocable_v<Deleter, T *>
  shared_ptr(T *ptr, Deleter &&deleter = {}) noexcept
      : m_ptr(ptr), m_control_block(nullptr) {
    if (ptr) {
      using control_block_type = control_block_separate<T, Deleter>;
      control_block_type *cb = pmr::polymorphic_allocator<control_block_type>{}
                                   .template new_object<control_block_type>(
                                       ptr, std::forward<Deleter>(deleter));
      if (cb) {
        m_control_block = cb;
      } else {
        // 分配控制块失败，调用删除器释放对象
        deleter(ptr);
        m_ptr = nullptr;
      }
      enable_shared_from_this_helper(
          ptr, this); // 处理 enable_shared_from_this 的支持
    }
  }

  template <typename U, typename Deleter = default_deleter<U>>
    requires std::is_convertible_v<std::add_pointer_t<U>,
                                   std::add_pointer_t<T>> &&
                 std::is_invocable_v<Deleter, U *>
  shared_ptr(U *ptr, Deleter &&deleter = {}) noexcept
      : m_ptr(ptr), m_control_block(nullptr) {
    if (ptr) {
      using control_block_type = control_block_separate<U, Deleter>;
      control_block_type *cb = pmr::polymorphic_allocator<control_block_type>{}
                                   .template new_object<control_block_type>(
                                       ptr, std::forward<Deleter>(deleter));
      if (cb) {
        m_control_block = cb;
      } else {
        // 分配控制块失败，调用删除器释放对象
        deleter(ptr);
        m_ptr = nullptr;
      }
      enable_shared_from_this_helper(
          ptr, this); // 处理 enable_shared_from_this 的支持
    }
  }

  shared_ptr(T *ptr, control_block_base *cb) noexcept
      : m_ptr(ptr), m_control_block(cb) {
    // 直接构造，不增加计数（用于 weak_ptr 的 lock）
  }

  shared_ptr(const weak_ptr<T> &wp) noexcept
      : m_ptr(wp.m_ptr), m_control_block(wp.m_control_block) {
    if (m_control_block) {
      // 增加共享引用计数
      m_control_block->shared_count.fetch_add(1, memory_order_relaxed);
    }
  }

  shared_ptr(const shared_ptr &other) noexcept
      : m_ptr(other.m_ptr), m_control_block(other.m_control_block) {
    if (m_control_block) {
      m_control_block->shared_count.fetch_add(1, memory_order_relaxed);
    }
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    if (this != &other) {
      // 增加新对象的引用计数
      if (other.m_control_block) {
        other.m_control_block->shared_count.fetch_add(1, memory_order_relaxed);
      }
      // 释放当前对象
      release();
      // 复制指针和控制块
      m_ptr = other.m_ptr;
      m_control_block = other.m_control_block;
    }
    return *this;
  }

  shared_ptr(shared_ptr &&other) noexcept
      : m_ptr(std::exchange(other.m_ptr, nullptr)),
        m_control_block(std::exchange(other.m_control_block, nullptr)) {
    // 不改变计数，因为只是移动
  }

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if (this != &other) {
      // 释放当前对象
      release();
      // 移动指针和控制块
      m_ptr = std::exchange(other.m_ptr, nullptr);
      m_control_block = std::exchange(other.m_control_block, nullptr);
      // 不改变计数
    }
    return *this;
  }

  ~shared_ptr() noexcept { release(); }

  T *get() const noexcept { return m_ptr; }
  T &operator*() const noexcept { return *m_ptr; }
  T *operator->() const noexcept { return m_ptr; }
  std::size_t use_count() const noexcept {
    return m_control_block
               ? m_control_block->shared_count.load(memory_order_relaxed)
               : 0;
  }
  explicit operator bool() const noexcept { return m_ptr != nullptr; }

private:
  void release() noexcept {
    if (m_control_block) {
      // 减少共享引用计数
      if (m_control_block->shared_count.fetch_sub(1, memory_order_release) ==
          1) {
        // 使用 acquire 内存序确保对象销毁前的所有写入对其他线程可见
        __atomic_thread_fence(memory_order_acquire);
        m_control_block->dispose(); // 销毁托管对象
        // 减少弱引用计数，如果 weak_count 已经为 0，则释放控制块
        if (m_control_block->weak_count.fetch_sub(1, memory_order_acq_rel) ==
            0) {
          m_control_block->deallocate();
        }
      }
      m_ptr = nullptr;
      m_control_block = nullptr;
    }
  }

  template <typename U>
  void enable_shared_from_this_helper(U *ptr, shared_ptr *sp) {
    if constexpr (std::is_base_of<enable_shared_from_this<U>, U>::value) {
      // 调用基类的 _internal_accept_owner
      ptr->internal_accept_owner_(sp);
    }
  }
};

template <typename T> class weak_ptr {
private:
  T *m_ptr;                            // 存储指针（与 shared_ptr 保持一致）
  control_block_base *m_control_block; // 控制块指针

  // 辅助函数：增加弱引用计数
  void increment_weak() {
    if (m_control_block) {
      m_control_block->weak_count.fetch_add(1, gdut::memory_order_relaxed);
    }
  }

  // 辅助函数：减少弱引用计数，若归零则释放控制块
  void decrement_weak() {
    if (m_control_block) {
      if (m_control_block->weak_count.fetch_sub(
              1, gdut::memory_order_acq_rel) == 1) {
        // 此时 weak_count 从 1 减到 0，且 shared_count 应该已经为 0
        m_control_block->deallocate();
      }
    }
  }

public:
  // 默认构造函数
  weak_ptr() noexcept : m_ptr(nullptr), m_control_block(nullptr) {}

  // 从 shared_ptr 构造（关键！）
  weak_ptr(const shared_ptr<T> &sp) noexcept
      : m_ptr(sp.m_ptr), m_control_block(sp.m_control_block) {
    increment_weak(); // 增加弱引用计数
  }

  // 拷贝构造函数
  weak_ptr(const weak_ptr &other) noexcept
      : m_ptr(other.m_ptr), m_control_block(other.m_control_block) {
    increment_weak();
  }

  // 移动构造函数
  weak_ptr(weak_ptr &&other) noexcept
      : m_ptr(std::exchange(other.m_ptr, nullptr)),
        m_control_block(std::exchange(other.m_control_block, nullptr)) {
    // 不改变计数，因为只是移动
  }

  // 析构函数
  ~weak_ptr() { decrement_weak(); }

  // 拷贝赋值
  weak_ptr &operator=(const weak_ptr &other) noexcept {
    if (this != &other) {
      decrement_weak(); // 释放当前引用
      m_ptr = other.m_ptr;
      m_control_block = other.m_control_block;
      increment_weak(); // 引用新控制块
    }
    return *this;
  }

  // 移动赋值
  weak_ptr &operator=(weak_ptr &&other) noexcept {
    if (this != &other) {
      decrement_weak(); // 释放当前引用
      m_ptr = std::exchange(other.m_ptr, nullptr);
      m_control_block = std::exchange(other.m_control_block, nullptr);
      // 不改变计数
    }
    return *this;
  }

  // 从 shared_ptr 赋值
  weak_ptr &operator=(const shared_ptr<T> &sp) noexcept {
    decrement_weak();
    m_ptr = sp.m_ptr;
    m_control_block = sp.m_control_block;
    increment_weak();
    return *this;
  }

  // --- 核心观察函数 ---

  // 检查对象是否已过期（shared_count == 0）
  bool expired() const noexcept {
    if (m_control_block) {
      return m_control_block->shared_count.load(gdut::memory_order_acquire) ==
             0;
    }
    return true; // 无控制块，视为过期
  }

  // 尝试获取 shared_ptr（原子操作）
  shared_ptr<T> lock() const noexcept {
    if (!expired()) {
      // 关键：需要原子地增加 shared_count，且确保对象未被销毁
      // 这里采用“尝试增加”的方式
      long old_count =
          m_control_block->shared_count.load(gdut::memory_order_relaxed);
      while (old_count != 0) {
        if (m_control_block->shared_count.compare_exchange_weak(
                old_count, old_count + 1, gdut::memory_order_acq_rel,
                gdut::memory_order_relaxed)) {
          // 成功增加引用计数，返回 shared_ptr
          return shared_ptr<T>(m_ptr, m_control_block);
        }
        // CAS 失败，说明 shared_count 可能已变化，重新加载
        old_count =
            m_control_block->shared_count.load(gdut::memory_order_relaxed);
      }
    }
    return shared_ptr<T>(); // 已过期，返回空 shared_ptr
  }

  // 获取共享引用计数（用于调试）
  long use_count() const noexcept {
    if (m_control_block) {
      return m_control_block->shared_count.load(gdut::memory_order_relaxed);
    }
    return 0;
  }

  // 重置
  void reset() noexcept {
    decrement_weak();
    m_ptr = nullptr;
    m_control_block = nullptr;
  }

  // 交换
  void swap(weak_ptr &other) noexcept {
    std::swap(m_ptr, other.m_ptr);
    std::swap(m_control_block, other.m_control_block);
  }

  // 友元声明，以便 shared_ptr 可以访问私有成员
  template <typename U> friend class shared_ptr;
};

// 非成员 swap
template <typename T> void swap(weak_ptr<T> &lhs, weak_ptr<T> &rhs) noexcept {
  lhs.swap(rhs);
}

template <typename T> class enable_shared_from_this {
  mutable weak_ptr<T> weak_this; // 假设你已有 weak_ptr<T>

protected:
  enable_shared_from_this() = default;

  // 禁止拷贝，保持语义正确
  enable_shared_from_this(const enable_shared_from_this &) {}
  enable_shared_from_this &operator=(const enable_shared_from_this &) {
    return *this;
  }

public:
  shared_ptr<T> shared_from_this() {
    return shared_ptr<T>(weak_this); // 从 weak_ptr 提升
  }

  shared_ptr<const T> shared_from_this() const {
    return shared_ptr<const T>(weak_this);
  }

  // 供 shared_ptr 构造函数调用的内部初始化函数
  void internal_accept_owner_(shared_ptr<T> *ptr) const {
    // 如果 weak_this 还没有被初始化，则从 ptr 复制一份 weak_ptr
    if (weak_this.expired()) {
      weak_this = *ptr; // 假设 weak_ptr 可以从 shared_ptr 赋值
    }
  }
};

namespace deprecated {

template <typename Ty> class shared_ptr {
public:
  shared_ptr() = default;

  template <typename Deleter>
    requires std::is_invocable_v<Deleter, std::add_pointer_t<Ty>>
  shared_ptr(std::add_pointer_t<Ty> ptr, Deleter &&deleter) noexcept
      : m_ptr(nullptr), m_deleter(nullptr), m_ref_count(nullptr) {
    if (ptr) {
      m_deleter = pmr::polymorphic_allocator<>{}
                      .new_object<deleter_wrapper_impl<Deleter>>(
                          std::forward<Deleter>(deleter));
      if (m_deleter) {
        m_ref_count =
            pmr::polymorphic_allocator<>{}.new_object<atomic<std::size_t>>();
        if (m_ref_count) {
          m_ref_count->store(1, memory_order_relaxed);
          m_ptr = ptr;
        } else {
          // Cleanup: delete the deleter wrapper
          pmr::polymorphic_allocator<>{}.delete_object(m_deleter);
          m_deleter = nullptr;
          // Call the deleter on the ptr since we can't manage it
          deleter(ptr);
        }
      } else {
        // Cleanup: call deleter on ptr since we can't manage it
        deleter(ptr);
      }
    }
  }

  template <typename U, typename Deleter>
    requires std::is_convertible_v<std::add_pointer_t<U>,
                                   std::add_pointer_t<Ty>> &&
                 std::is_invocable_v<Deleter, std::add_pointer_t<U>>
  shared_ptr(U *ptr, Deleter &&deleter) noexcept
      : m_ptr(nullptr), m_deleter(nullptr), m_ref_count(nullptr) {
    if (ptr) {
      m_deleter = pmr::polymorphic_allocator<>{}
                      .new_object<deleter_wrapper_impl<Deleter>>(
                          std::forward<Deleter>(deleter));
      if (m_deleter) {
        m_ref_count =
            pmr::polymorphic_allocator<>{}.new_object<atomic<std::size_t>>();
        if (m_ref_count) {
          m_ref_count->store(1, memory_order_relaxed);
          m_ptr = ptr;
        } else {
          // Call the deleter on the ptr since we can't manage it
          (*m_deleter)(ptr);
          // Cleanup: delete the deleter wrapper
          pmr::polymorphic_allocator<>{}.delete_object(m_deleter);
          m_deleter = nullptr;
        }
      } else {
        // Cleanup: call deleter on ptr since we can't manage it
        deleter(ptr);
      }
    }
  }

  template <typename U, typename Deleter>
    requires std::is_convertible_v<std::add_pointer_t<U>,
                                   std::add_pointer_t<Ty>> &&
                 std::is_invocable_v<Deleter, std::add_pointer_t<U>>
  explicit shared_ptr(std::unique_ptr<U, Deleter> &&other) noexcept
      : m_ptr(nullptr), m_deleter(nullptr), m_ref_count(nullptr) {
    auto deleter = other.get_deleter();
    m_ptr = other.release();

    if (m_ptr) {
      m_deleter =
          pmr::polymorphic_allocator<>{}
              .new_object<deleter_wrapper_impl<Deleter>>(std::move(deleter));
      if (m_deleter) {
        m_ref_count =
            pmr::polymorphic_allocator<>{}.new_object<atomic<std::size_t>>();
        if (m_ref_count) {
          m_ref_count->store(1, memory_order_relaxed);
        } else {
          // Cleanup: call deleter on ptr and delete deleter wrapper
          (*m_deleter)(m_ptr);
          pmr::polymorphic_allocator<>{}.delete_object(m_deleter);
          m_ptr = nullptr;
          m_deleter = nullptr;
        }
      } else {
        // Cleanup: call deleter on ptr since we can't manage it
        deleter(m_ptr);
        m_ptr = nullptr;
      }
    }
  }

  shared_ptr(const shared_ptr &other) noexcept
      : m_ptr(other.m_ptr), m_deleter(other.m_deleter),
        m_ref_count(other.m_ref_count) {
    if (m_ref_count) {
      // Relaxed is safe for increment - no synchronization needed
      m_ref_count->fetch_add(1, memory_order_relaxed);
    }
  }

  shared_ptr(shared_ptr &&other) noexcept
      : m_ptr(other.m_ptr), m_deleter(other.m_deleter),
        m_ref_count(other.m_ref_count) {
    other.m_ptr = nullptr;
    other.m_deleter = nullptr;
    other.m_ref_count = nullptr;
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    // Use copy-and-swap idiom for strong exception safety and avoid races
    shared_ptr(other).swap(*this);
    return *this;
  }

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    // Use move-and-swap
    shared_ptr(std::move(other)).swap(*this);
    return *this;
  }

  void swap(shared_ptr &other) noexcept {
    std::swap(m_ptr, other.m_ptr);
    std::swap(m_deleter, other.m_deleter);
    std::swap(m_ref_count, other.m_ref_count);
  }

  ~shared_ptr() noexcept { release(); }

  std::add_pointer_t<Ty> get() const noexcept { return m_ptr; }
  Ty &operator*() const noexcept { return *m_ptr; }
  std::add_pointer_t<Ty> operator->() const noexcept { return m_ptr; }
  std::size_t use_count() const noexcept {
    return m_ref_count ? m_ref_count->load(memory_order_relaxed) : 0;
  }
  explicit operator bool() const noexcept { return m_ptr != nullptr; }

private:
  struct deleter_wrapper {
    virtual std::size_t size() const noexcept = 0;
    virtual std::size_t alignment() const noexcept = 0;
    virtual void operator()(Ty *ptr) = 0;
    virtual ~deleter_wrapper() = default;
  };

  template <typename Deleter> struct deleter_wrapper_impl : deleter_wrapper {
    Deleter deleter;

    deleter_wrapper_impl(Deleter &&d) : deleter(std::forward<Deleter>(d)) {}
    virtual std::size_t size() const noexcept override {
      return sizeof(deleter_wrapper_impl<Deleter>);
    }
    virtual std::size_t alignment() const noexcept override {
      return alignof(deleter_wrapper_impl<Deleter>);
    }
    virtual void operator()(Ty *ptr) override { deleter(ptr); }
  };

  void release() noexcept {
    if (m_ref_count) {
      // Use release memory order for the decrement to synchronize with other
      // threads This ensures all writes to the managed object are visible
      // before deletion
      if (m_ref_count->fetch_sub(1, memory_order_release) == 1) {
        // Use acquire fence to synchronize with all releases from other threads
        __atomic_thread_fence(memory_order_acquire);

        if (m_deleter && m_ptr) {
          (*m_deleter)(m_ptr);
        }
        if (m_deleter) {
          // Use polymorphic deletion with proper size since the concrete
          // deleter type is unknown
          std::size_t deleter_size = m_deleter->size();
          m_deleter->~deleter_wrapper();
          pmr::polymorphic_allocator<char>{}.deallocate(
              reinterpret_cast<char *>(m_deleter), deleter_size);
        }
        pmr::polymorphic_allocator<>{}.delete_object<atomic<std::size_t>>(
            m_ref_count);
      }
    }
  }

  std::add_pointer_t<Ty> m_ptr{nullptr};
  std::add_pointer_t<deleter_wrapper> m_deleter{nullptr};
  atomic<std::size_t> *m_ref_count{nullptr};
};
} // namespace deprecated
} // namespace gdut

#endif // BSP_SHARED_PTR_HPP
