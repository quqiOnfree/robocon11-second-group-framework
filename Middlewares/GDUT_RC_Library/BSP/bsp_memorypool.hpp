#ifndef BSP_MEMORYPOOL_HPP
#define BSP_MEMORYPOOL_HPP

#include "FreeRTOS.h"
#include "bsp_mutex.hpp"
#include "bsp_type_traits.hpp"
#include "portable.h"
#include <chrono>
#include <cmsis_os2.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "tlsf.h"

namespace gdut::pmr {

class memory_resource {
  static constexpr size_t max_align = alignof(max_align_t);

public:
  memory_resource() = default;
  memory_resource(const memory_resource &) = default;
  virtual ~memory_resource() = default;

  memory_resource &operator=(const memory_resource &) = default;

  void *allocate(size_t bytes, size_t alignment = max_align) {
    return do_allocate(bytes, alignment);
  }
  void deallocate(void *p, size_t bytes, size_t alignment = max_align) {
    do_deallocate(p, bytes, alignment);
  }

  bool is_equal(const memory_resource &other) const noexcept {
    return do_is_equal(other);
  }

private:
  virtual void *do_allocate(size_t bytes, size_t alignment) = 0;
  virtual void do_deallocate(void *p, size_t bytes, size_t alignment) = 0;

  virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

class new_delete_resource : public memory_resource {
public:
  static memory_resource *get_instance() {
    static new_delete_resource instance;
    return &instance;
  }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    (void)alignment; // alignment is ignored in this simple implementation
    return pvPortMalloc(bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    (void)bytes;     // bytes is ignored in this simple implementation
    (void)alignment; // alignment is ignored in this simple implementation
    vPortFree(p);
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return true; // All instances of new_delete_resource are considered equal
  }
};

// 非线程安全的内存池资源
class unsynchronized_pool_resource : public memory_resource {
  struct alloc_node {
    struct alloc_node *next;
    // 可选的 size 信息
  };

  memory_resource *m_upstream_resource{nullptr};
  tlsf_t m_pool_memory{nullptr};
  size_t default_pool_block_size{0};
  alloc_node *free_list_head{nullptr}; // 空闲块链表头指针

public:
  static constexpr size_t default_block_size() {
    return 512; // Default block size for the pool
  }

  unsynchronized_pool_resource(
      memory_resource *upstream = new_delete_resource::get_instance(),
      std::size_t pool_block_size = default_block_size())
      : m_upstream_resource(upstream),
        default_pool_block_size(pool_block_size) {
    if (upstream == nullptr) {
      m_upstream_resource = new_delete_resource::get_instance();
      if (m_upstream_resource == nullptr) {
        m_pool_memory = nullptr;
        return;
      }
    }
    void *mem = m_upstream_resource->allocate(sizeof(alloc_node) +
                                                  default_pool_block_size,
                                              alignof(std::max_align_t));
    if (mem) {
      free_list_head = static_cast<alloc_node *>(mem);
      free_list_head->next = nullptr; // 初始化空闲链表
      m_pool_memory =
          tlsf_create_with_pool(static_cast<char *>(mem) + sizeof(alloc_node),
                                default_pool_block_size);
    } else {
      m_pool_memory = nullptr;
    }
  }

  unsynchronized_pool_resource(const unsynchronized_pool_resource &) = delete;
  unsynchronized_pool_resource &
  operator=(const unsynchronized_pool_resource &) = delete;

  ~unsynchronized_pool_resource() override {
    if (m_pool_memory == nullptr) {
      return;
    }
    tlsf_destroy(m_pool_memory);
    while (free_list_head != nullptr) {
      alloc_node *current = free_list_head;
      free_list_head = free_list_head->next;
      m_upstream_resource->deallocate(
          current, sizeof(alloc_node) + default_pool_block_size,
          alignof(std::max_align_t));
    }
  }

  explicit operator bool() const { return m_pool_memory != nullptr; }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    if (m_pool_memory == nullptr || bytes > default_pool_block_size) {
      return nullptr;
    }
    if (void *mem = tlsf_memalign(m_pool_memory, alignment, bytes);
        mem != nullptr) {
      return mem;
    }
    void *new_mem = m_upstream_resource->allocate(sizeof(alloc_node) +
                                                      default_pool_block_size,
                                                  alignof(std::max_align_t));
    if (new_mem == nullptr) {
      return nullptr;
    }
    if (tlsf_add_pool(m_pool_memory,
                      static_cast<char *>(new_mem) + sizeof(alloc_node),
                      default_pool_block_size) == 0) {
      m_upstream_resource->deallocate(
          new_mem, sizeof(alloc_node) + default_pool_block_size,
          alignof(std::max_align_t));
      return nullptr;
    }
    static_cast<alloc_node *>(new_mem)->next =
        free_list_head; // 将新块加入空闲链表
    free_list_head = static_cast<alloc_node *>(new_mem);
    return tlsf_memalign(m_pool_memory, alignment, bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    if (m_pool_memory != nullptr) {
      tlsf_free(m_pool_memory, p);
    }
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

class synchronized_pool_resource : public memory_resource {
  unsynchronized_pool_resource m_pool;
  mutable mutex m_mutex;

public:
  synchronized_pool_resource(
      memory_resource *upstream = new_delete_resource::get_instance(),
      std::size_t pool_block_size =
          unsynchronized_pool_resource::default_block_size())
      : m_pool(upstream, pool_block_size) {}

  synchronized_pool_resource(const synchronized_pool_resource &) = delete;
  synchronized_pool_resource &
  operator=(const synchronized_pool_resource &) = delete;

  explicit operator bool() const {
    lock_guard<mutex> lock(m_mutex);
    return static_cast<bool>(m_pool);
  }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    lock_guard<mutex> lock(m_mutex);
    return m_pool.allocate(bytes, alignment);
  }

  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    lock_guard<mutex> lock(m_mutex);
    m_pool.deallocate(p, bytes, alignment);
  }

  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

class os_memory_pool_resource : public memory_resource {
  osMemoryPoolId_t m_pool_id{nullptr};

public:
  os_memory_pool_resource(size_t block_count, size_t block_size) {
    m_pool_id = osMemoryPoolNew(block_count, block_size, nullptr);
  }

  os_memory_pool_resource(const os_memory_pool_resource &) = delete;
  os_memory_pool_resource &operator=(const os_memory_pool_resource &) = delete;

  ~os_memory_pool_resource() override {
    if (m_pool_id != nullptr) {
      osMemoryPoolDelete(m_pool_id);
    }
  }

  explicit operator bool() const { return m_pool_id != nullptr; }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    (void)alignment; // alignment is ignored in this implementation
    if (m_pool_id == nullptr || bytes == 0) {
      return nullptr;
    }
    return osMemoryPoolAlloc(m_pool_id, osWaitForever);
  }

  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    (void)bytes;     // bytes is ignored in this implementation
    (void)alignment; // alignment is ignored in this implementation
    if (m_pool_id != nullptr && p != nullptr) {
      osMemoryPoolFree(m_pool_id, p);
    }
  }

  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

template <class Ty = std::byte> class polymorphic_allocator {
public:
  using value_type = Ty;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  polymorphic_allocator(
      memory_resource *r = new_delete_resource::get_instance()) noexcept
      : m_resource(r) {}

  template <class U>
  polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept
      : m_resource(other.resource()) {}

  template <class U>
  polymorphic_allocator &
  operator=(const polymorphic_allocator<U> &other) noexcept {
    m_resource = other.resource();
    return *this;
  }

  polymorphic_allocator(const polymorphic_allocator &other) noexcept = default;
  polymorphic_allocator &
  operator=(const polymorphic_allocator &other) noexcept = default;

  template <typename U>
  polymorphic_allocator(polymorphic_allocator<U> &&other) noexcept
      : m_resource(other.resource()) {}
  template <typename U>
  polymorphic_allocator &operator=(polymorphic_allocator<U> &&other) noexcept {
    m_resource = other.resource();
    return *this;
  }

  polymorphic_allocator(polymorphic_allocator &&other) noexcept = default;
  polymorphic_allocator &
  operator=(polymorphic_allocator &&other) noexcept = default;

  memory_resource *resource() const noexcept { return m_resource; }

  Ty *allocate(std::size_t n) {
    if (n > std::size_t(-1) / sizeof(Ty)) {
      return nullptr;
    }
    void *p = m_resource->allocate(n * sizeof(Ty), alignof(Ty));
    if (!p) {
      return nullptr;
    }
    return static_cast<Ty *>(p);
  }

  void deallocate(Ty *p, std::size_t n) {
    m_resource->deallocate(p, n * sizeof(Ty), alignof(Ty));
  }

  template <typename U, typename... Args>
  static void construct(U *ptr, Args &&...args) {
    new (ptr) U(std::forward<Args>(args)...);
  }

  template <typename U> static void destroy(U *ptr) {
    if (ptr != nullptr) {
      ptr->~U();
    }
  }

  template <typename U, typename... Args>
  std::add_pointer_t<U> new_object(Args &&...args) {
    auto ptr = m_resource->allocate(sizeof(U), alignof(U));
    if (ptr == nullptr) {
      return nullptr;
    }
    construct(static_cast<std::add_pointer_t<U>>(ptr),
              std::forward<Args>(args)...);
    return static_cast<std::add_pointer_t<U>>(ptr);
  }

  template <typename U> void delete_object(U *ptr) {
    if (ptr != nullptr) {
      destroy(ptr);
      m_resource->deallocate(ptr, sizeof(U), alignof(U));
    }
  }

private:
  memory_resource *m_resource;
};

template <class T, class U>
inline bool operator==(const polymorphic_allocator<T> &a,
                       const polymorphic_allocator<U> &b) {
  return a.resource() == b.resource();
}

template <class T, class U>
inline bool operator!=(const polymorphic_allocator<T> &a,
                       const polymorphic_allocator<U> &b) {
  return a.resource() != b.resource();
}

} // namespace gdut::pmr

#endif // BSP_MEMORYPOOL_HPP
