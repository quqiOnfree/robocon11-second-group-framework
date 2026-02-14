#ifndef BSP_SHARED_PTR_HPP
#define BSP_SHARED_PTR_HPP

#include "bsp_atomic.hpp"
#include "bsp_memorypool.hpp"
#include <cstddef>
#include <memory>
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
        enable_shared_from_this_helper(
            ptr, this); // 处理 enable_shared_from_this 的支持
      } else {
        // 分配控制块失败，调用删除器释放对象
        deleter(ptr);
        m_ptr = nullptr;
      }
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
        enable_shared_from_this_helper(
            ptr, this); // 处理 enable_shared_from_this 的支持
      } else {
        // 分配控制块失败，调用删除器释放对象
        deleter(ptr);
        m_ptr = nullptr;
      }
    }
  }

  shared_ptr(T *ptr, control_block_base *cb) noexcept
      : m_ptr(ptr), m_control_block(cb) {
    // 直接构造，不修改引用计数。
    // 典型用途：weak_ptr::lock() 在通过 CAS 成功递增 shared_count 之后，
    // 使用该构造函数创建 shared_ptr，因此此处不能再次递增 shared_count。
    // 换言之，调用方必须保证在调用本构造前已经正确递增了 shared_count。
  }

  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  shared_ptr(const shared_ptr<U> &other) noexcept
      : m_ptr(other.m_ptr), m_control_block(other.m_control_block) {
    if (m_control_block) {
      m_control_block->shared_count.fetch_add(1, memory_order_relaxed);
    }
  }

  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  shared_ptr &operator=(const shared_ptr<U> &other) noexcept {
    if (this != std::addressof(other)) {
      shared_ptr<T> temp(other);
      swap(temp);
    }
    return *this;
  }

  shared_ptr(const shared_ptr &other) noexcept
      : m_ptr(other.m_ptr), m_control_block(other.m_control_block) {
    if (m_control_block) {
      // Incrementing the shared count does not establish a happens-before
      // relationship, so relaxed ordering is sufficient here. Synchronization
      // for object lifetime is provided on decrement (e.g. acquire/release).
      m_control_block->shared_count.fetch_add(1, memory_order_relaxed);
    }
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    if (this != std::addressof(other)) {
      shared_ptr<T> temp(other);
      swap(temp);
    }
    return *this;
  }

  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  shared_ptr(shared_ptr<U> &&other) noexcept
      : m_ptr(std::exchange(other.m_ptr, nullptr)),
        m_control_block(std::exchange(other.m_control_block, nullptr)) {}

  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  shared_ptr &operator=(shared_ptr<U> &&other) noexcept {
    if (this != std::addressof(other)) {
      shared_ptr<T> temp(std::move(other));
      swap(temp);
    }
    return *this;
  }

  shared_ptr(shared_ptr &&other) noexcept
      : m_ptr(std::exchange(other.m_ptr, nullptr)),
        m_control_block(std::exchange(other.m_control_block, nullptr)) {}

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if (this != std::addressof(other)) {
      shared_ptr<T> temp(std::move(other));
      swap(temp);
    }
    return *this;
  }

  template <typename U, typename Deleter>
    requires std::is_convertible_v<std::add_pointer_t<U>,
                                   std::add_pointer_t<T>> &&
                 std::is_invocable_v<Deleter, std::add_pointer_t<U>>
  shared_ptr(std::unique_ptr<U, Deleter> &&other) noexcept
      : m_ptr(nullptr), m_control_block(nullptr) {
    auto deleter = std::move(other.get_deleter());
    m_ptr = other.release();

    if (m_ptr) {
      using control_block_type = control_block_separate<U, Deleter>;
      control_block_type *cb = pmr::polymorphic_allocator<control_block_type>{}
                                   .template new_object<control_block_type>(
                                       m_ptr, std::move(deleter));
      if (cb) {
        m_control_block = cb;
        enable_shared_from_this_helper(
            m_ptr, this); // 处理 enable_shared_from_this 的支持
      } else {
        // 分配控制块失败，调用删除器释放对象
        deleter(m_ptr);
        m_ptr = nullptr;
      }
    }
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

  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  void reset(U *ptr = nullptr) noexcept {
    // 创建一个新的 shared_ptr 来管理新对象
    shared_ptr temp(ptr);
    swap(temp); // 交换当前对象与新对象，旧对象会在 temp 的析构时释放
  }

  template <typename U, typename Deleter>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  void reset(U *ptr, Deleter &&deleter) noexcept {
    shared_ptr temp;

    if (ptr != nullptr) {
      using control_block_type =
          control_block_separate<U, std::decay_t<Deleter>>;

      control_block_type *cb = pmr::polymorphic_allocator<control_block_type>{}
                                   .template new_object<control_block_type>(
                                       ptr, std::forward<Deleter>(deleter));

      if (cb != nullptr) {
        temp.m_ptr = ptr;
        temp.m_control_block = cb;
        enable_shared_from_this_helper(ptr, &temp);
      } else {
        std::forward<Deleter>(deleter)(ptr);
      }
    }

    swap(temp);
  }
  template <typename U>
    requires std::is_convertible_v<std::add_pointer_t<U>, std::add_pointer_t<T>>
  void swap(shared_ptr &other) noexcept {
    std::swap(m_ptr, other.m_ptr);
    std::swap(m_control_block, other.m_control_block);
  }

  explicit operator bool() const noexcept { return m_ptr != nullptr; }

  template <typename U>
  friend class shared_ptr; // 友元声明，以便不同类型的 shared_ptr
                           // 可以访问私有成员

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

// 合并分配模式的工厂函数，类似 std::make_shared
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args &&...args) {
  using control_block_t = control_block_combined<T>;
  pmr::polymorphic_allocator<control_block_t> alloc{};
  auto *control = alloc.new_object(std::forward<Args>(args)...);
  return shared_ptr<T>(control->get(), control);
}

template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc &alloc, Args &&...args) {
  using control_block_t = control_block_combined<T>;
  using cb_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<
      control_block_t>;
  cb_alloc cb_alloc_traits(alloc);
  auto *control =
      cb_alloc_traits.allocate(1); // 分配一个 control_block_t 的空间
  if (control) {
    std::allocator_traits<cb_alloc>::construct(cb_alloc_traits, control,
                                               std::forward<Args>(args)...);
  } else {
    return shared_ptr<T>(); // 分配失败，返回空 shared_ptr
  }
  return shared_ptr<T>(control->get(), control);
}

// 非成员 swap
template <typename T>
void swap(shared_ptr<T> &lhs, shared_ptr<T> &rhs) noexcept {
  lhs.swap(rhs);
}

// 比较运算符：与 std::shared_ptr 保持一致
template <typename T, typename U>
bool operator==(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return lhs.get() == rhs.get();
}

template <typename T, typename U>
bool operator!=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T, typename U>
bool operator<(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return lhs.get() < rhs.get();
}

template <typename T, typename U>
bool operator<=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return !(rhs < lhs);
}

template <typename T, typename U>
bool operator>(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return rhs < lhs;
}

template <typename T, typename U>
bool operator>=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
  return !(lhs < rhs);
}

// 与 nullptr 比较
template <typename T>
bool operator==(const shared_ptr<T> &ptr, std::nullptr_t) noexcept {
  return ptr.get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const shared_ptr<T> &ptr) noexcept {
  return ptr.get() == nullptr;
}

template <typename T>
bool operator!=(const shared_ptr<T> &ptr, std::nullptr_t) noexcept {
  return !(ptr == nullptr);
}

template <typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T> &ptr) noexcept {
  return !(ptr == nullptr);
}
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
    if (this != std::addressof(other)) {
      decrement_weak(); // 释放当前引用
      m_ptr = other.m_ptr;
      m_control_block = other.m_control_block;
      increment_weak(); // 引用新控制块
    }
    return *this;
  }

  // 移动赋值
  weak_ptr &operator=(weak_ptr &&other) noexcept {
    if (this != std::addressof(other)) {
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
    control_block_base *old_control_block = m_control_block;
    m_ptr = nullptr;
    m_control_block = nullptr;

    if (old_control_block != nullptr) {
      long old_weak = old_control_block->weak_count.fetch_sub(
          1, gdut::memory_order_acq_rel);
      if (old_weak == 1) {
        // 最后一个弱引用，此时 shared_count 必须已经为 0
        old_control_block->deallocate();
      }
    }
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
  mutable weak_ptr<T> weak_this;

protected:
  enable_shared_from_this() = default;

  // 禁止拷贝，保持语义正确
  enable_shared_from_this(const enable_shared_from_this &) noexcept {}
  enable_shared_from_this &operator=(const enable_shared_from_this &) noexcept {
    return *this;
  }

public:
  shared_ptr<T> shared_from_this() {
    return weak_this.lock(); // 从 weak_ptr 锁定获取 shared_ptr
  }

  shared_ptr<T> shared_from_this() const { return weak_this.lock(); }

  // 供 shared_ptr 构造函数调用的内部初始化函数
  void internal_accept_owner_(shared_ptr<T> *ptr) const {
    // 如果 weak_this 还没有被初始化，则从 ptr 复制一份 weak_ptr
    if (weak_this.expired()) {
      weak_this = *ptr; // weak_ptr 支持从 shared_ptr 赋值（由
                        // weak_ptr::operator=(const shared_ptr<T>&) 保证）
    }
  }
};

} // namespace gdut

#endif // BSP_SHARED_PTR_HPP
