#ifndef BSP_SHARED_PTR_HPP
#define BSP_SHARED_PTR_HPP

#include "bsp_atomic.hpp"
#include "bsp_memorypool.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace gdut {

template <typename Ty> class shared_ptr {
public:
  shared_ptr() = default;

  template <typename Deleter>
  shared_ptr(Ty *ptr, Deleter &&deleter) noexcept
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
                                   std::add_pointer_t<Ty>>
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
                                   std::add_pointer_t<Ty>>
  explicit shared_ptr(const std::unique_ptr<U, Deleter> &other) noexcept {
    auto deleter = other.get_deleter();
    m_ptr = other.release();

    if (m_ptr) {
      m_deleter =
          pmr::polymorphic_allocator<>{}
              .new_object<deleter_wrapper_impl<Deleter>>(std::move(deleter));
      m_ref_count =
          pmr::polymorphic_allocator<>{}.new_object<atomic<std::size_t>>();
      if (m_ref_count) {
        m_ref_count->store(1, memory_order_relaxed);
      }
    }
  }

  shared_ptr(const shared_ptr &other) noexcept
      : m_ptr(other.m_ptr), m_deleter(other.m_deleter),
        m_ref_count(other.m_ref_count) {
    if (m_ref_count) {
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
      if (m_ref_count->fetch_sub(1, memory_order_relaxed) == 1) {
        if (m_deleter && m_ptr) {
          (*m_deleter)(m_ptr);
        }
        if (m_deleter) {
          std::size_t deleter_size = m_deleter->size();
          std::size_t deleter_align = m_deleter->alignment();
          m_deleter->~deleter_wrapper();
          pmr::polymorphic_allocator<char>{}.deallocate(
              reinterpret_cast<char *>(m_deleter), deleter_size, deleter_align);
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

} // namespace gdut

#endif // BSP_SHARED_PTR_HPP
