#ifndef BSP_MEMORY_RESOURCE_HPP
#define BSP_MEMORY_RESOURCE_HPP

#include "FreeRTOS.h"
#include "bsp_mutex.hpp"
#include "tlsf.h"
#include <cmsis_os2.h>
#include <cstddef>
#include <memory_resource>

namespace gdut::pmr {

class portable_resource : public std::pmr::memory_resource {
public:
  static memory_resource *get_instance() {
    static portable_resource instance;
    return &instance;
  }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    // FreeRTOS guarantees portBYTE_ALIGNMENT-byte alignment for pvPortMalloc.
    // If a stronger alignment is requested, explicitly fail the allocation
    // to avoid returning misaligned memory and causing undefined behavior.
    const size_t requested_alignment =
        alignment == 0 ? alignof(max_align_t) : alignment;

    if (requested_alignment > static_cast<size_t>(portBYTE_ALIGNMENT)) {
      return nullptr;
    }
    return pvPortMalloc(bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    (void)bytes;     // bytes is ignored in this simple implementation
    (void)alignment; // alignment is ignored in this simple implementation
    vPortFree(p);
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return true; // All instances of portable_resource are considered equal
  }
};

// 非线程安全的内存池资源
class unsynchronized_tlsf_resource : public std::pmr::memory_resource {
  struct alloc_node {
    struct alloc_node *next;
  };

  memory_resource *m_upstream_resource{nullptr};
  tlsf_t m_pool_memory{nullptr};
  size_t m_default_pool_block_size{0};
  alloc_node *m_free_list_head{nullptr}; // 空闲块链表头指针

public:
  static constexpr size_t default_block_size() {
    return 512; // Default block size for the pool
  }

  unsynchronized_tlsf_resource(
      memory_resource *upstream = portable_resource::get_instance(),
      std::size_t pool_block_size = default_block_size())
      : m_upstream_resource(upstream),
        m_default_pool_block_size(pool_block_size) {
    if (upstream == nullptr) {
      m_upstream_resource = portable_resource::get_instance();
      if (m_upstream_resource == nullptr) {
        m_pool_memory = nullptr;
        return;
      }
    }
    // Allocate enough space for the node header, TLSF control structure, and
    // pool
    const size_t tlsf_overhead = tlsf_size();
    void *mem = m_upstream_resource->allocate(
        sizeof(alloc_node) + tlsf_overhead + m_default_pool_block_size,
        alignof(std::max_align_t));
    if (mem) {
      m_free_list_head = static_cast<alloc_node *>(mem);
      m_free_list_head->next = nullptr; // 初始化空闲链表
      // tlsf_create_with_pool expects: tlsf_size() + pool_bytes
      m_pool_memory =
          tlsf_create_with_pool(static_cast<char *>(mem) + sizeof(alloc_node),
                                tlsf_overhead + m_default_pool_block_size);
    } else {
      m_pool_memory = nullptr;
    }
  }

  unsynchronized_tlsf_resource(const unsynchronized_tlsf_resource &) = delete;
  unsynchronized_tlsf_resource &
  operator=(const unsynchronized_tlsf_resource &) = delete;

  ~unsynchronized_tlsf_resource() override {
    if (m_pool_memory == nullptr) {
      return;
    }
    tlsf_destroy(m_pool_memory);
    const size_t tlsf_overhead = tlsf_size();
    while (m_free_list_head != nullptr) {
      alloc_node *current = m_free_list_head;
      m_free_list_head = m_free_list_head->next;
      m_upstream_resource->deallocate(current,
                                      sizeof(alloc_node) + tlsf_overhead +
                                          m_default_pool_block_size,
                                      alignof(std::max_align_t));
    }
  }

  explicit operator bool() const { return m_pool_memory != nullptr; }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    if (m_pool_memory == nullptr) {
      return nullptr;
    }
    // For oversized requests, fall back to upstream resource
    if (bytes > m_default_pool_block_size) {
      return m_upstream_resource->allocate(bytes, alignment);
    }
    if (void *mem = tlsf_memalign(m_pool_memory, alignment, bytes);
        mem != nullptr) {
      return mem;
    }
    // Add a new pool (no tlsf_size() overhead needed for tlsf_add_pool)
    void *new_mem = m_upstream_resource->allocate(sizeof(alloc_node) +
                                                      m_default_pool_block_size,
                                                  alignof(std::max_align_t));
    if (new_mem == nullptr) {
      return nullptr;
    }
    if (tlsf_add_pool(m_pool_memory,
                      static_cast<char *>(new_mem) + sizeof(alloc_node),
                      m_default_pool_block_size) == 0) {
      m_upstream_resource->deallocate(
          new_mem, sizeof(alloc_node) + m_default_pool_block_size,
          alignof(std::max_align_t));
      return nullptr;
    }
    static_cast<alloc_node *>(new_mem)->next =
        m_free_list_head; // 将新块加入空闲链表
    m_free_list_head = static_cast<alloc_node *>(new_mem);
    return tlsf_memalign(m_pool_memory, alignment, bytes);
  }
  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    // If this was an oversized allocation, it came from upstream
    if (bytes > m_default_pool_block_size) {
      m_upstream_resource->deallocate(p, bytes, alignment);
    } else if (m_pool_memory != nullptr) {
      tlsf_free(m_pool_memory, p);
    }
  }
  bool do_is_equal(const memory_resource &other) const noexcept override {
    return this == std::addressof(other);
  }
};

class synchronized_tlsf_resource : public std::pmr::memory_resource {
  unsynchronized_tlsf_resource m_pool;
  mutable mutex m_mutex;

public:
  synchronized_tlsf_resource(
      memory_resource *upstream = portable_resource::get_instance(),
      std::size_t pool_block_size =
          unsynchronized_tlsf_resource::default_block_size())
      : m_pool(upstream, pool_block_size) {}

  synchronized_tlsf_resource(const synchronized_tlsf_resource &) = delete;
  synchronized_tlsf_resource &
  operator=(const synchronized_tlsf_resource &) = delete;

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

class os_memory_pool_resource : public std::pmr::memory_resource {
  osMemoryPoolId_t m_pool_id{nullptr};
  size_t m_block_size{0};

public:
  os_memory_pool_resource(size_t block_count, size_t block_size)
      : m_block_size(block_size) {
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
    // Verify that the requested size fits the pool's block size
    if (bytes > m_block_size) {
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

} // namespace gdut::pmr

#endif // BSP_MEMORY_RESOURCE_HPP
