///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2022 John Wellbelove

Inspired by the techniques used in https://github.com/mrshurik/poly_span
Copyright(c) 2020 Dr. Alexander Bulovyatov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef GDUT_POLY_SPAN_INCLUDED
#define GDUT_POLY_SPAN_INCLUDED

#include "array.hpp"
#include "hash.hpp"
#include "integral_limits.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "nullptr.hpp"
#include "platform.hpp"
#include "type_traits.hpp"

#include "private/dynamic_extent.hpp"

#if GDUT_USING_STL && GDUT_USING_CPP11
#include <array>
#endif

///\defgroup poly_span poly_span
///\ingroup containers

namespace gdut {
template <typename U, size_t Extent> class poly_span;

namespace private_poly_span {
//*************************************************************************
// Iterator
//*************************************************************************
template <typename TBase> class iterator {
public:
  template <typename U, size_t Extent> friend class gdut::poly_span;

  template <typename UBase> friend class const_iterator;

  typedef TBase value_type;
  typedef ptrdiff_t difference_type;
  typedef TBase *pointer;
  typedef TBase &reference;
  typedef GDUT_OR_STD::random_access_iterator_tag iterator_category;

  //*****************************************
  iterator() : ptr(GDUT_NULLPTR), element_size(0U) {}

  //*****************************************
  iterator(const iterator &other)
      : ptr(other.ptr), element_size(other.element_size) {}

  //*****************************************
  iterator &operator=(const iterator &rhs) {
    ptr = rhs.ptr;
    element_size = rhs.element_size;

    return *this;
  }

  //*****************************************
  TBase &operator*() const { return *ptr; }

  //*****************************************
  TBase *operator->() const { return ptr; }

  //*****************************************
  iterator &operator++() {
    ptr =
        reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) + element_size);
    return *this;
  }

  //*****************************************
  iterator operator++(int) {
    iterator temp(*this);
    ptr =
        reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) + element_size);
    return temp;
  }

  //*****************************************
  iterator &operator--() {
    ptr =
        reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) - element_size);
    return *this;
  }

  //*****************************************
  iterator operator--(int) {
    iterator temp(*this);
    ptr =
        reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) - element_size);
    return temp;
  }

  //***************************************************
  iterator &operator+=(difference_type offset) {
    ptr = reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) +
                                    (offset * difference_type(element_size)));
    return *this;
  }

  //***************************************************
  iterator &operator-=(difference_type offset) {
    ptr = reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) -
                                    (offset * difference_type(element_size)));
    return *this;
  }

  //***************************************************
  friend bool operator==(const iterator &lhs, const iterator &rhs) {
    return (lhs.ptr == rhs.ptr) && (lhs.element_size == rhs.element_size);
  }

  //***************************************************
  friend bool operator!=(const iterator &lhs, const iterator &rhs) {
    return !(lhs == rhs);
  }

  //***************************************************
  friend bool operator<(const iterator &lhs, const iterator &rhs) {
    return lhs.ptr < rhs.ptr;
  }

  //***************************************************
  friend bool operator<=(const iterator &lhs, const iterator &rhs) {
    return !(lhs > rhs);
  }

  //***************************************************
  friend bool operator>(const iterator &lhs, const iterator &rhs) {
    return (rhs < lhs);
  }

  //***************************************************
  friend bool operator>=(const iterator &lhs, const iterator &rhs) {
    return !(lhs < rhs);
  }

  //***************************************************
  friend iterator operator+(const iterator &lhs, difference_type offset) {
    iterator temp(lhs);
    temp += offset;
    return temp;
  }

  //***************************************************
  friend iterator operator+(difference_type offset, const iterator &rhs) {
    iterator temp(rhs);
    temp += offset;
    return temp;
  }

  //***************************************************
  friend iterator operator-(const iterator &lhs, difference_type offset) {
    iterator temp(lhs);
    temp -= offset;
    return temp;
  }

  //***************************************************
  friend iterator operator-(difference_type offset, const iterator &rhs) {
    iterator temp(rhs);
    temp -= offset;
    return temp;
  }

  //***************************************************
  friend difference_type operator-(const iterator &lhs, const iterator &rhs) {
    return lhs.ptr - rhs.ptr;
  }

private:
  //***************************************************
  iterator(TBase *pbegin_, size_t index_, size_t element_size_)
      : element_size(element_size_) {
    ptr = reinterpret_cast<pointer>(reinterpret_cast<char *>(pbegin_) +
                                    (index_ * element_size));
  }

  TBase *ptr;
  size_t element_size;
};
} // namespace private_poly_span

//***************************************************************************
/// Poly Span - Fixed Extent
//***************************************************************************
template <typename TBase, size_t Extent = gdut::dynamic_extent>
class poly_span {
public:
  typedef TBase element_type;
  typedef typename gdut::remove_cv<TBase>::type value_type;
  typedef size_t size_type;
  typedef TBase &reference;
  typedef const TBase &const_reference;
  typedef TBase *pointer;
  typedef const TBase *const_pointer;
  typedef private_poly_span::iterator<TBase> iterator;

  typedef GDUT_OR_STD::reverse_iterator<iterator> reverse_iterator;

  static GDUT_CONSTANT size_t extent = Extent;

  template <typename UBase, size_t UExtent> friend class poly_span;

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR poly_span() GDUT_NOEXCEPT : pbegin(GDUT_NULLPTR),
                                             element_size(0U) {}

  //*************************************************************************
  /// Construct from iterator + size
  //*************************************************************************
  template <typename TIterator, typename TSize>
  GDUT_CONSTEXPR poly_span(const TIterator begin_,
                           const TSize /*size_*/) GDUT_NOEXCEPT
      : pbegin(gdut::addressof(*begin_)),
        element_size(
            sizeof(typename gdut::iterator_traits<TIterator>::value_type)) {
    typedef typename gdut::iterator_traits<TIterator>::value_type data_type;

    GDUT_STATIC_ASSERT(
        (gdut::is_same<GDUT_OR_STD::random_access_iterator_tag,
                       typename gdut::iterator_traits<
                           TIterator>::iterator_category>::value),
        "Not a random access iterator");
    GDUT_STATIC_ASSERT((gdut::is_base_of<TBase, data_type>::value ||
                        gdut::is_same<TBase, data_type>::value),
                       "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from iterators
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR poly_span(const TIterator begin_, const TIterator /*end_*/)
      : pbegin(gdut::addressof(*begin_)),
        element_size(
            sizeof(typename gdut::iterator_traits<TIterator>::value_type)) {
    typedef typename gdut::iterator_traits<TIterator>::value_type data_type;
    typedef typename gdut::iterator_traits<TIterator>::iterator_category
        iterator_category;

    GDUT_STATIC_ASSERT((gdut::is_same<GDUT_OR_STD::random_access_iterator_tag,
                                      iterator_category>::value),
                       "Not a random access iterator");
    GDUT_STATIC_ASSERT((gdut::is_base_of<TBase, data_type>::value ||
                        gdut::is_same<TBase, data_type>::value),
                       "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from C array
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(U (&begin_)[Size]) GDUT_NOEXCEPT
      : pbegin(begin_),
        element_size(sizeof(U)) {
    GDUT_STATIC_ASSERT(Size <= Extent, "Array data overflow");
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from gdut::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(gdut::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)) {
    GDUT_STATIC_ASSERT(Size <= Extent, "Array data overflow");
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from gdut::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(const gdut::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)) {
    GDUT_STATIC_ASSERT(Size <= Extent, "Array data overflow");
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

#if GDUT_USING_STL && GDUT_USING_CPP11
  //*************************************************************************
  /// Construct from std::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(std::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)) {
    GDUT_STATIC_ASSERT(Size <= Extent, "Array data overflow");
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of U");
  }

  //*************************************************************************
  /// Construct from std::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(const std::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)) {
    GDUT_STATIC_ASSERT(Size <= Extent, "Array data overflow");
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of U");
  }
#endif

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  GDUT_CONSTEXPR poly_span(const poly_span<TBase, Extent> &other) GDUT_NOEXCEPT
      : pbegin(other.pbegin),
        element_size(other.element_size) {}

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  template <typename UBase>
  GDUT_CONSTEXPR poly_span(const poly_span<UBase, Extent> &other) GDUT_NOEXCEPT
      : pbegin(other.pbegin),
        element_size(other.element_size) {}

  //*************************************************************************
  /// Returns a reference to the first element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference front() const GDUT_NOEXCEPT {
    return *pbegin;
  }

  //*************************************************************************
  /// Returns a reference to the last element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference back() const GDUT_NOEXCEPT {
    return *element_at(Extent - 1U);
  }

  //*************************************************************************
  /// Returns a pointer to the first element of the internal storage.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR pointer data() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns an iterator to the beginning of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator begin() const GDUT_NOEXCEPT {
    return iterator(pbegin, 0U, element_size);
  }

  //*************************************************************************
  /// Returns an iterator to the end of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator end() const GDUT_NOEXCEPT {
    return iterator(pbegin, Extent, element_size);
  }

  //*************************************************************************
  // Returns an reverse iterator to the reverse beginning of the poly_span.
  //*************************************************************************
  GDUT_CONSTEXPR reverse_iterator rbegin() const GDUT_NOEXCEPT {
    return reverse_iterator(end());
  }

  //*************************************************************************
  /// Returns a reverse iterator to the end of the poly_span.
  //*************************************************************************
  GDUT_CONSTEXPR reverse_iterator rend() const GDUT_NOEXCEPT {
    return reverse_iterator(begin());
  }

  //*************************************************************************
  /// Returns <b>true</b> if the poly_span size is zero.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR bool empty() const GDUT_NOEXCEPT {
    return (Extent == 0U);
  }

  //*************************************************************************
  /// Returns the size of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size() const GDUT_NOEXCEPT {
    return Extent;
  }

  //*************************************************************************
  /// Returns the size of the type stored in the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_of_element() const GDUT_NOEXCEPT {
    return element_size;
  }

  //*************************************************************************
  /// Returns the size of the poly_span in bytes.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_bytes() const GDUT_NOEXCEPT {
    return Extent * element_size;
  }

  //*************************************************************************
  /// Assign from a poly_span.
  //*************************************************************************
  GDUT_CONSTEXPR14 poly_span &operator=(const poly_span &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    element_size = other.element_size;
    return *this;
  }

  //*************************************************************************
  /// Assign from a poly_span.
  //*************************************************************************
  template <typename UBase>
  GDUT_CONSTEXPR14 poly_span &
  operator=(const poly_span<UBase, Extent> &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    element_size = other.element_size;
    return *this;
  }

  //*************************************************************************
  /// Returns a reference to the indexed value.
  //*************************************************************************
  GDUT_CONSTEXPR reference operator[](size_t i) const { return *element_at(i); }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the first COUNT elements of this
  /// poly_span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::poly_span<element_type, COUNT>
  first() const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, COUNT>(pbegin, 0U, COUNT,
                                                element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the first count elements of this
  /// poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      first(size_t count) const {
    return gdut::poly_span<element_type, gdut::dynamic_extent>(
        pbegin, 0U, count, element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the last COUNT elements of this
  /// poly_span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::poly_span<element_type, COUNT>
  last() const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, COUNT>(pbegin, Extent - COUNT, COUNT,
                                                element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the last count elements of this
  /// poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      last(size_t count) const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, gdut::dynamic_extent>(
        pbegin, Extent - count, count, element_size);
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Obtains a poly_span that is a view from OFFSET over the next COUNT
  /// elements of this poly_span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT = gdut::dynamic_extent>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::poly_span<
      element_type, COUNT != gdut::dynamic_extent ? COUNT : Extent - OFFSET>
  subspan() const GDUT_NOEXCEPT {
    return (COUNT == gdut::dynamic_extent)
               ? gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                                   ? COUNT
                                                   : Extent - OFFSET>(
                     pbegin, OFFSET, Extent, element_size)
               : gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                                   ? COUNT
                                                   : Extent - OFFSET>(
                     pbegin, OFFSET, COUNT, element_size);
  }
#else
  //*************************************************************************
  /// Obtains a poly_span that is a view from OFFSET over the next COUNT
  /// elements of this poly_span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT>
  gdut::poly_span<element_type,
                  COUNT != gdut::dynamic_extent ? COUNT : Extent - OFFSET>
  subspan() const {
    if (COUNT == gdut::dynamic_extent) {
      return gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                               ? COUNT
                                               : Extent - OFFSET>(
          pbegin, OFFSET, Extent, element_size);
    } else {
      return gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                               ? COUNT
                                               : Extent - OFFSET>(
          pbegin, OFFSET, element_size);
    }
  }
#endif

  //*************************************************************************
  /// Obtains a poly_span that is a view from 'offset' over the next 'count'
  /// elements of this poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      subspan(size_t offset,
              size_t count = gdut::dynamic_extent) const GDUT_NOEXCEPT {
    return (count == gdut::dynamic_extent)
               ? gdut::poly_span<element_type, gdut::dynamic_extent>(
                     pbegin, offset, Extent, element_size)
               : gdut::poly_span<element_type, gdut::dynamic_extent>(
                     pbegin, offset, count, element_size);
  }

protected:
  //***************************************************************************
  template <typename TFrom> struct char_ptr_type {
    typedef typename gdut::conditional<gdut::is_const<TFrom>::value,
                                       const char *, char *>::type type;
  };

  typedef typename char_ptr_type<TBase>::type char_ptr_t;

  //***************************************************************************
  pointer element_at(size_t index) const GDUT_NOEXCEPT {
    char_ptr_t base = reinterpret_cast<char_ptr_t>(pbegin);
    return reinterpret_cast<pointer>(base + (index * element_size));
  }

  //*************************************************************************
  /// Construct from iterator + offset + element size
  /// extent_ is ignored.
  //*************************************************************************
  poly_span(TBase *pbegin_, size_t offset_, size_t /*extent_*/,
            size_t element_size_) GDUT_NOEXCEPT
      : pbegin(reinterpret_cast<pointer>(reinterpret_cast<char_ptr_t>(pbegin_) +
                                         (offset_ * element_size_))),
        element_size(element_size_) {}

private:
  pointer pbegin;
  size_t element_size;
};

template <typename TBase, size_t Extent>
GDUT_CONSTANT size_t poly_span<TBase, Extent>::extent;

//***************************************************************************
/// Poly Span - Dynamic Extent
//***************************************************************************
template <typename TBase> class poly_span<TBase, gdut::dynamic_extent> {
public:
  typedef TBase element_type;
  typedef typename gdut::remove_cv<TBase>::type value_type;
  typedef size_t size_type;
  typedef TBase &reference;
  typedef const TBase &const_reference;
  typedef TBase *pointer;
  typedef const TBase *const_pointer;
  typedef gdut::private_poly_span::iterator<TBase> iterator;
  typedef GDUT_OR_STD::reverse_iterator<iterator> reverse_iterator;

  static GDUT_CONSTANT size_t extent = gdut::dynamic_extent;

  template <typename UBase, size_t UExtent> friend class poly_span;

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR poly_span() GDUT_NOEXCEPT : pbegin(GDUT_NULLPTR),
                                             element_size(0U),
                                             span_extent(0U) {}

  //*************************************************************************
  /// Construct from iterator + size
  //*************************************************************************
  template <typename TIterator, typename TSize>
  GDUT_CONSTEXPR poly_span(const TIterator begin_,
                           const TSize size_) GDUT_NOEXCEPT
      : pbegin(gdut::addressof(*begin_)),
        element_size(
            sizeof(typename gdut::iterator_traits<TIterator>::value_type)),
        span_extent(size_) {
    typedef typename gdut::iterator_traits<TIterator>::value_type data_type;
    typedef typename gdut::iterator_traits<TIterator>::iterator_category
        iterator_category;

    GDUT_STATIC_ASSERT((gdut::is_same<GDUT_OR_STD::random_access_iterator_tag,
                                      iterator_category>::value),
                       "Not a random access iterator");
    GDUT_STATIC_ASSERT((gdut::is_base_of<TBase, data_type>::value ||
                        gdut::is_same<TBase, data_type>::value),
                       "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from iterators
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR poly_span(const TIterator begin_, const TIterator end_)
      : pbegin(gdut::addressof(*begin_)),
        element_size(
            sizeof(typename gdut::iterator_traits<TIterator>::value_type)),
        span_extent(size_t(gdut::distance(begin_, end_))) {
    typedef typename gdut::iterator_traits<TIterator>::value_type data_type;
    typedef typename gdut::iterator_traits<TIterator>::iterator_category
        iterator_category;

    GDUT_STATIC_ASSERT((gdut::is_same<GDUT_OR_STD::random_access_iterator_tag,
                                      iterator_category>::value),
                       "Not a random access iterator");
    GDUT_STATIC_ASSERT((gdut::is_base_of<TBase, data_type>::value ||
                        gdut::is_same<TBase, data_type>::value),
                       "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from C array
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(U (&begin_)[Size]) GDUT_NOEXCEPT
      : pbegin(begin_),
        element_size(sizeof(U)),
        span_extent(Size) {
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from gdut::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(gdut::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)),
        span_extent(Size) {
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

  //*************************************************************************
  /// Construct from gdut::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(const gdut::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)),
        span_extent(Size) {
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of the data type");
  }

#if GDUT_USING_STL && GDUT_USING_CPP11
  //*************************************************************************
  /// Construct from std::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(std::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)),
        span_extent(Size) {
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of U");
  }

  //*************************************************************************
  /// Construct from std::array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR poly_span(const std::array<U, Size> &a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        element_size(sizeof(U)),
        span_extent(Size) {
    GDUT_STATIC_ASSERT(
        (gdut::is_base_of<TBase, U>::value || gdut::is_same<TBase, U>::value),
        "TBase not a base of U");
  }
#endif

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  GDUT_CONSTEXPR
  poly_span(const poly_span<TBase, gdut::dynamic_extent> &other) GDUT_NOEXCEPT
      : pbegin(other.pbegin),
        element_size(other.element_size),
        span_extent(other.span_extent) {}

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  template <typename UBase>
  GDUT_CONSTEXPR
  poly_span(const poly_span<UBase, gdut::dynamic_extent> &other) GDUT_NOEXCEPT
      : pbegin(other.pbegin),
        element_size(other.element_size),
        span_extent(other.span_extent) {}

  //*************************************************************************
  /// Returns a reference to the first element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference front() const GDUT_NOEXCEPT {
    return *pbegin;
  }

  //*************************************************************************
  /// Returns a reference to the last element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference back() const GDUT_NOEXCEPT {
    return *element_at(span_extent - 1U);
  }

  //*************************************************************************
  /// Returns a pointer to the first element of the internal storage.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR pointer data() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns an iterator to the beginning of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator begin() const GDUT_NOEXCEPT {
    return iterator(pbegin, 0U, element_size);
  }

  //*************************************************************************
  /// Returns an iterator to the end of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator end() const GDUT_NOEXCEPT {
    return iterator(pbegin, span_extent, element_size);
  }

  //*************************************************************************
  // Returns an reverse iterator to the reverse beginning of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rbegin() const GDUT_NOEXCEPT {
    return reverse_iterator(end());
  }

  //*************************************************************************
  /// Returns a reverse iterator to the end of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rend() const GDUT_NOEXCEPT {
    return reverse_iterator(begin());
  }

  //*************************************************************************
  /// Returns <b>true</b> if the poly_span size is zero.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR bool empty() const GDUT_NOEXCEPT {
    return (span_extent == 0);
  }

  //*************************************************************************
  /// Returns the size of the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size() const GDUT_NOEXCEPT {
    return span_extent;
  }

  //*************************************************************************
  /// Returns the size of the type stored in the poly_span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_of_element() const GDUT_NOEXCEPT {
    return element_size;
  }

  //*************************************************************************
  /// Returns the size of the poly_span in bytes.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_bytes() const GDUT_NOEXCEPT {
    return element_size * span_extent;
  }

  //*************************************************************************
  /// Assign from a poly_span.
  //*************************************************************************
  GDUT_CONSTEXPR14 poly_span &operator=(const poly_span &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    element_size = other.element_size;
    span_extent = other.span_extent;
    return *this;
  }

  //*************************************************************************
  /// Assign from a poly_span.
  //*************************************************************************
  template <typename UBase>
  GDUT_CONSTEXPR14 poly_span &
  operator=(const poly_span<UBase, gdut::dynamic_extent> &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    element_size = other.element_size;
    span_extent = other.span_extent;
    return *this;
  }

  //*************************************************************************
  /// Returns a reference to the indexed value.
  //*************************************************************************
  GDUT_CONSTEXPR reference operator[](size_t i) const { return *element_at(i); }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the first COUNT elements of this
  /// poly_span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::poly_span<element_type, COUNT>
  first() const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, COUNT>(pbegin, 0U, COUNT,
                                                element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the first count elements of this
  /// poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      first(size_t count) const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, gdut::dynamic_extent>(
        pbegin, 0U, count, element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the last COUNT elements of this
  /// poly_span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::poly_span<element_type, COUNT>
  last() const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, COUNT>(pbegin, span_extent - COUNT,
                                                COUNT, element_size);
  }

  //*************************************************************************
  /// Obtains a poly_span that is a view over the last count elements of this
  /// poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      last(size_t count) const GDUT_NOEXCEPT {
    return gdut::poly_span<element_type, gdut::dynamic_extent>(
        pbegin, span_extent - count, count, element_size);
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Obtains a poly_span that is a view from OFFSET over the next COUNT
  /// elements of this poly_span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT = gdut::dynamic_extent>
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                                       ? COUNT
                                                       : gdut::dynamic_extent>
      subspan() const GDUT_NOEXCEPT {
    return (COUNT == gdut::dynamic_extent)
               ? gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                                   ? COUNT
                                                   : gdut::dynamic_extent>(
                     pbegin, OFFSET, span_extent, element_size)
               : gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                                   ? COUNT
                                                   : gdut::dynamic_extent>(
                     pbegin, OFFSET, COUNT, element_size);
  }
#else
  //*************************************************************************
  /// Obtains a poly_span that is a view from OFFSET over the next COUNT
  /// elements of this poly_span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT>
  gdut::poly_span<element_type,
                  COUNT != gdut::dynamic_extent ? COUNT : gdut::dynamic_extent>
  subspan() const {
    if (COUNT == gdut::dynamic_extent) {
      return gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                               ? COUNT
                                               : gdut::dynamic_extent>(
          pbegin, OFFSET, span_extent, element_size);
    } else {
      return gdut::poly_span<element_type, COUNT != gdut::dynamic_extent
                                               ? COUNT
                                               : gdut::dynamic_extent>(
          pbegin, OFFSET, span_extent, element_size);
    }
  }
#endif

  //*************************************************************************
  /// Obtains a poly_span that is a view from 'offset' over the next 'count'
  /// elements of this poly_span.
  //*************************************************************************
  GDUT_NODISCARD
      GDUT_CONSTEXPR gdut::poly_span<element_type, gdut::dynamic_extent>
      subspan(size_t offset,
              size_t count = gdut::dynamic_extent) const GDUT_NOEXCEPT {
    return (count == gdut::dynamic_extent)
               ? gdut::poly_span<element_type, gdut::dynamic_extent>(
                     pbegin, offset, span_extent - offset, element_size)
               : gdut::poly_span<element_type, gdut::dynamic_extent>(
                     pbegin, offset, count, element_size);
  }

protected:
  //*************************************************************************
  /// Construct from iterator + offset + size + element size
  //*************************************************************************
  poly_span(TBase *pbegin_, size_t offset_, size_t extent_,
            size_t element_size_) GDUT_NOEXCEPT
      : pbegin(reinterpret_cast<pointer>(reinterpret_cast<char_ptr_t>(pbegin_) +
                                         (offset_ * element_size_))),
        element_size(element_size_),
        span_extent(extent_) {}

private:
  //***************************************************************************
  template <typename TFrom> struct char_ptr_type {
    typedef typename gdut::conditional<gdut::is_const<TFrom>::value,
                                       const char *, char *>::type type;
  };

  typedef typename char_ptr_type<TBase>::type char_ptr_t;

  //***************************************************************************
  pointer element_at(size_t index) const GDUT_NOEXCEPT {
    char_ptr_t base = reinterpret_cast<char_ptr_t>(pbegin);
    return reinterpret_cast<pointer>(base + (index * element_size));
  }

  pointer pbegin;
  size_t element_size;
  size_t span_extent;
};

template <typename TBase>
GDUT_CONSTANT size_t poly_span<TBase, gdut::dynamic_extent>::extent;

//*************************************************************************
/// Template deduction guides.
//*************************************************************************
#if GDUT_USING_CPP17
template <typename TIterator>
poly_span(const TIterator begin_, const TIterator end_)
    -> poly_span<gdut::remove_pointer_t<TIterator>, gdut::dynamic_extent>;

template <typename TIterator, typename TSize>
poly_span(const TIterator begin_, const TSize size_)
    -> poly_span<gdut::remove_pointer_t<TIterator>, gdut::dynamic_extent>;

template <typename T, size_t Size> poly_span(T (&)[Size]) -> poly_span<T, Size>;

template <typename T, size_t Size>
poly_span(gdut::array<T, Size> &) -> poly_span<T, Size>;

template <typename T, size_t Size>
poly_span(const gdut::array<T, Size> &) -> poly_span<const T, Size>;

#if GDUT_USING_STL
template <typename T, size_t Size>
poly_span(std::array<T, Size> &) -> poly_span<T, Size>;

template <typename T, size_t Size>
poly_span(const std::array<T, Size> &) -> poly_span<const T, Size>;
#endif
#endif

//*************************************************************************
/// Hash function.
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <typename TBase, size_t Extent>
struct hash<gdut::poly_span<TBase, Extent>> {
  size_t operator()(const gdut::poly_span<TBase, Extent> &view) const {
    return gdut::private_hash::generic_hash<size_t>(
        reinterpret_cast<const uint8_t *>(view.data()),
        reinterpret_cast<const uint8_t *>(view.data() + view.size()));
  }
};
#endif
} // namespace gdut

#endif
