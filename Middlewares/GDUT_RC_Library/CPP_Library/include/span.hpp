///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2020 John Wellbelove

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

#ifndef GDUT_SPAN_INCLUDED
#define GDUT_SPAN_INCLUDED

#include "platform.hpp"

#include "algorithm.hpp"
#include "alignment.hpp"
#include "array.hpp"
#include "byte.hpp"
#include "circular_iterator.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "hash.hpp"
#include "integral_limits.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "nullptr.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"

#include "private/dynamic_extent.hpp"

///\defgroup span span
///\ingroup containers

namespace gdut {
template <typename T> struct is_std_array : gdut::false_type {};
#if GDUT_USING_STL && GDUT_USING_CPP11
template <typename T, size_t N>
struct is_std_array<std::array<T, N>> : gdut::true_type {};
#endif
template <typename T> struct is_std_array<const T> : is_std_array<T> {};
template <typename T> struct is_std_array<volatile T> : is_std_array<T> {};
template <typename T>
struct is_std_array<const volatile T> : is_std_array<T> {};

template <typename T> struct is_etl_array : gdut::false_type {};
template <typename T, size_t N>
struct is_etl_array<gdut::array<T, N>> : gdut::true_type {};
template <typename T> struct is_etl_array<const T> : is_etl_array<T> {};
template <typename T> struct is_etl_array<volatile T> : is_etl_array<T> {};
template <typename T>
struct is_etl_array<const volatile T> : is_etl_array<T> {};

//***************************************************************************
// Tag to indicate a class is a span.
//***************************************************************************
class span_tag {};

//***************************************************************************
///\ingroup span
/// Exception base for span
//***************************************************************************
class span_exception : public exception {
public:
  span_exception(string_type reason_, string_type file_name_,
                 numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
///\ingroup span
/// Bad alignment exception.
//***************************************************************************
class span_alignment_exception : public span_exception {
public:
  span_alignment_exception(string_type file_name_, numeric_type line_number_)
      : span_exception(GDUT_ERROR_TEXT("span:alignment", GDUT_SPAN_FILE_ID "A"),
                       file_name_, line_number_) {}
};

//***************************************************************************
///\ingroup span
/// span size exception.
//***************************************************************************
class span_size_mismatch : public span_exception {
public:
  span_size_mismatch(string_type file_name_, numeric_type line_number_)
      : span_exception(GDUT_ERROR_TEXT("span:size", GDUT_SPAN_FILE_ID "B"),
                       file_name_, line_number_) {}
};

//***************************************************************************
///\ingroup span
/// The out of range exceptions.
//***************************************************************************
class span_out_of_range : public span_exception {
public:
  span_out_of_range(string_type file_name_, numeric_type line_number_)
      : span_exception(GDUT_ERROR_TEXT("span:range", GDUT_SPAN_FILE_ID "C"),
                       file_name_, line_number_) {}
};

//***************************************************************************
/// Span - Fixed Extent
//***************************************************************************
template <typename T, size_t Extent = gdut::dynamic_extent>
class span : public span_tag {
public:
  typedef T element_type;
  typedef typename gdut::remove_cv<T>::type value_type;
  typedef size_t size_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *pointer;
  typedef const T *const_pointer;

  typedef T *iterator;
  typedef const T *const_iterator;
  typedef GDUT_OR_STD::reverse_iterator<iterator> reverse_iterator;
  typedef GDUT_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;

  typedef gdut::circular_iterator<pointer> circular_iterator;
  typedef gdut::circular_iterator<GDUT_OR_STD::reverse_iterator<pointer>>
      reverse_circular_iterator;

  static GDUT_CONSTANT size_t extent = Extent;

  //*************************************************************************
  /// Construct from iterators + size
  //*************************************************************************
  template <typename TIterator, typename TSize>
  GDUT_CONSTEXPR explicit span(const TIterator begin_,
                               const TSize /*size_*/) GDUT_NOEXCEPT
      : pbegin(gdut::to_address(begin_)) {}

  //*************************************************************************
  /// Construct from iterators
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR explicit span(const TIterator begin_,
                               const TIterator /*end_*/) GDUT_NOEXCEPT
      : pbegin(gdut::to_address(begin_)) {}

  //*************************************************************************
  /// Construct from C array
  //*************************************************************************
#if GDUT_USING_CPP11
  template <size_t Array_Size, typename = typename gdut::enable_if<
                                   (Array_Size == Extent), void>::type>
  GDUT_CONSTEXPR span(element_type (&begin_)[Array_Size]) GDUT_NOEXCEPT
      : pbegin(begin_) {}
#else
  //*************************************************************************
  /// Construct from C array
  //*************************************************************************
  template <size_t Array_Size>
  GDUT_CONSTEXPR
  span(element_type (&begin_)[Array_Size],
       typename gdut::enable_if<(Array_Size == Extent), void>::type * = 0)
      GDUT_NOEXCEPT : pbegin(begin_) {}
#endif

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <
      typename TContainer,
      typename = typename gdut::enable_if<
          !gdut::is_base_of<span_tag,
                            gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_std_array<
                  gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_etl_array<
                  gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_pointer<gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_array<gdut::remove_reference_t<TContainer>>::value &&
              gdut::is_same<gdut::remove_cv_t<T>,
                            gdut::remove_cv_t<typename gdut::remove_reference_t<
                                TContainer>::value_type>>::value,
          void>::type>
  GDUT_CONSTEXPR span(TContainer &&a) GDUT_NOEXCEPT : pbegin(a.data()) {}
#else
  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <typename TContainer>
  span(TContainer &a,
       typename gdut::enable_if<
           !gdut::is_base_of<span_tag, typename gdut::remove_reference<
                                           TContainer>::type>::value &&
               !gdut::is_std_array<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_etl_array<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_pointer<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_array<TContainer>::value &&
               gdut::is_same<
                   typename gdut::remove_cv<T>::type,
                   typename gdut::remove_cv<typename gdut::remove_reference<
                       TContainer>::type::value_type>::type>::value,
           void>::type * = 0) GDUT_NOEXCEPT : pbegin(a.data()) {}

  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <typename TContainer>
  span(const TContainer &a,
       typename gdut::enable_if<
           !gdut::is_base_of<span_tag, typename gdut::remove_reference<
                                           TContainer>::type>::value &&
               !gdut::is_std_array<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_etl_array<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_pointer<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_array<TContainer>::value &&
               gdut::is_same<
                   typename gdut::remove_cv<T>::type,
                   typename gdut::remove_cv<typename gdut::remove_reference<
                       TContainer>::type::value_type>::type>::value,
           void>::type * = 0) GDUT_NOEXCEPT : pbegin(a.data()) {}
#endif

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  GDUT_CONSTEXPR span(const span &other) GDUT_NOEXCEPT : pbegin(other.pbegin) {}

  //*************************************************************************
  /// Copy constructor
  /// From fixed extent span.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR
  span(const gdut::span<U, Size> &other,
       typename gdut::enable_if<Size == Extent, void>::type * = 0) GDUT_NOEXCEPT
      : pbegin(other.data()) {}

  //*************************************************************************
  /// Copy constructor
  /// From dynamic extent span.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR14
  span(const gdut::span<U, Size> &other,
       typename gdut::enable_if<Size == gdut::dynamic_extent, void>::type * = 0)
      : pbegin(other.data()) {
    GDUT_ASSERT(other.size() == Extent, GDUT_ERROR(span_size_mismatch));
  }

#if GDUT_USING_STL && GDUT_USING_CPP11
  //*************************************************************************
  /// Constructor from std array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR
  span(std::array<U, Size> &other,
       typename gdut::enable_if<Size == Extent, void>::type * = 0) GDUT_NOEXCEPT
      : pbegin(other.data()) {}

  //*************************************************************************
  /// Constructor from const std array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR
  span(const std::array<U, Size> &other,
       typename gdut::enable_if<Size == Extent && gdut::is_const<T>::value,
                                void>::type * = 0) GDUT_NOEXCEPT
      : pbegin(other.data()) {}
#endif

  //*************************************************************************
  /// Constructor from etl array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR
  span(gdut::array<U, Size> &other,
       typename gdut::enable_if<Size == Extent, void>::type * = 0) GDUT_NOEXCEPT
      : pbegin(other.data()) {}

  //*************************************************************************
  /// Constructor from const etl array.
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR
  span(const gdut::array<U, Size> &other,
       typename gdut::enable_if<Size == Extent && gdut::is_const<T>::value,
                                void>::type * = 0) GDUT_NOEXCEPT
      : pbegin(other.data()) {}

  //*************************************************************************
  /// Returns a reference to the first element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference front() const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Extent > 0, "Span is empty");

    return *pbegin;
  }

  //*************************************************************************
  /// Returns a reference to the last element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference back() const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Extent > 0, "Span is empty");

    return *((pbegin + Extent) - 1);
  }

  //*************************************************************************
  /// Returns a pointer to the first element of the internal storage.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR pointer data() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns a const iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_iterator cbegin() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns an iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator begin() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns a circular iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR circular_iterator
  begin_circular() const GDUT_NOEXCEPT {
    return circular_iterator(begin(), end());
  }

  //*************************************************************************
  /// Returns a const iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_iterator cend() const GDUT_NOEXCEPT {
    return (pbegin + Extent);
  }

  //*************************************************************************
  /// Returns an iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator end() const GDUT_NOEXCEPT {
    return (pbegin + Extent);
  }

  //*************************************************************************
  // Returns a const reverse iterator to the reverse beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_reverse_iterator
  crbegin() const GDUT_NOEXCEPT {
    return const_reverse_iterator((pbegin + Extent));
  }

  //*************************************************************************
  // Returns an reverse iterator to the reverse beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rbegin() const GDUT_NOEXCEPT {
    return reverse_iterator((pbegin + Extent));
  }

  //*************************************************************************
  /// Returns a reverse circular iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_circular_iterator
  rbegin_circular() const GDUT_NOEXCEPT {
    return reverse_circular_iterator(rbegin(), rend());
  }

  //*************************************************************************
  /// Returns a const reverse iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_reverse_iterator
  crend() const GDUT_NOEXCEPT {
    return const_reverse_iterator(pbegin);
  }

  //*************************************************************************
  /// Returns a reverse iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rend() const GDUT_NOEXCEPT {
    return reverse_iterator(pbegin);
  }

  //*************************************************************************
  /// Returns <b>true</b> if the span size is zero.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR bool empty() const GDUT_NOEXCEPT {
    return Extent == 0;
  }

  //*************************************************************************
  /// Returns the size of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size() const GDUT_NOEXCEPT {
    return Extent;
  }

  //*************************************************************************
  /// Returns the size of the span in bytes.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_bytes() const GDUT_NOEXCEPT {
    return sizeof(element_type) * Extent;
  }

  //*************************************************************************
  /// Returns the maximum possible size of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t max_size() const GDUT_NOEXCEPT {
    return size();
  }

  //*************************************************************************
  /// Assign from a span.
  //*************************************************************************
  GDUT_CONSTEXPR14 span &operator=(const span &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    return *this;
  }

  //*************************************************************************
  /// Returns a reference to the value at index 'i'.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR14 reference at(size_t i) {
    GDUT_ASSERT(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
  }

  //*************************************************************************
  /// Returns a const reference to the value at index 'i'.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR14 const_reference at(size_t i) const {
    GDUT_ASSERT(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
  }

  //*************************************************************************
  /// Returns a reference to the indexed value.
  //*************************************************************************
  GDUT_CONSTEXPR reference operator[](const size_t i) const {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_INDEX_OPERATOR
    return i < size() ? pbegin[i] : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_INDEX_OPERATOR(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the first COUNT elements of this span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, COUNT>
  first() const GDUT_NOEXCEPT {
    // If Extent is static, check that original span contains at least COUNT
    // elements
    GDUT_STATIC_ASSERT(COUNT <= Extent,
                       "Original span does not contain COUNT elements");

    return gdut::span<element_type, COUNT>(pbegin, pbegin + COUNT);
  }

  //*************************************************************************
  /// Obtains a span that is a view over the first count elements of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, gdut::dynamic_extent>
  first(size_t count) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return count <= size() ? gdut::span<element_type, gdut::dynamic_extent>(
                                 pbegin, pbegin + count)
                           : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(count <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, gdut::dynamic_extent>(pbegin,
                                                          pbegin + count);
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the last COUNT elements of this span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, COUNT>
  last() const GDUT_NOEXCEPT {
    // If Extent is static, check that original span contains at least COUNT
    // elements
    GDUT_STATIC_ASSERT(COUNT <= Extent,
                       "Original span does not contain COUNT elements");

    return gdut::span<element_type, COUNT>(pbegin + Extent - COUNT,
                                           (pbegin + Extent));
  }

  //*************************************************************************
  /// Obtains a span that is a view over the last count elements of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, gdut::dynamic_extent>
  last(size_t count) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return count <= size() ? gdut::span<element_type, gdut::dynamic_extent>(
                                 (pbegin + Extent) - count, (pbegin + Extent))
                           : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(count <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, gdut::dynamic_extent>(
        (pbegin + Extent) - count, (pbegin + Extent));
#endif
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Obtains a span that is a view from OFFSET over the next COUNT elements of
  /// this span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT = gdut::dynamic_extent>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<
      element_type, COUNT != gdut::dynamic_extent ? COUNT : Extent - OFFSET>
  subspan() const GDUT_NOEXCEPT {
    // If Extent is static, check that OFFSET is within the original span
    GDUT_STATIC_ASSERT(OFFSET <= Extent,
                       "OFFSET is not within the original span");

    // If count is also static, check that OFFSET + COUNT is within the original
    // span
    GDUT_STATIC_ASSERT(
        (COUNT != gdut::dynamic_extent) ? COUNT <= (Extent - OFFSET) : true,
        "OFFSET + COUNT is not within the original span");

    return (COUNT == gdut::dynamic_extent)
               ? gdut::span<element_type, COUNT != gdut::dynamic_extent
                                              ? COUNT
                                              : Extent - OFFSET>(
                     pbegin + OFFSET, (pbegin + Extent))
               : gdut::span<element_type, COUNT != gdut::dynamic_extent
                                              ? COUNT
                                              : Extent - OFFSET>(
                     pbegin + OFFSET, pbegin + OFFSET + COUNT);
  }
#else
  //*************************************************************************
  /// Obtains a span that is a view from OFFSET over the next COUNT elements of
  /// this span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT>
  gdut::span<element_type,
             COUNT != gdut::dynamic_extent ? COUNT : Extent - OFFSET>
  subspan() const {
    // If Extent is static, check that OFFSET is within the original span
    GDUT_STATIC_ASSERT(OFFSET <= Extent,
                       "OFFSET is not within the original span");

    // If count is also static, check that OFFSET + COUNT is within the original
    // span
    GDUT_STATIC_ASSERT(
        (COUNT != gdut::dynamic_extent) ? COUNT <= (Extent - OFFSET) : true,
        "OFFSET + COUNT is not within the original span");

    if (COUNT == gdut::dynamic_extent) {
      return gdut::span<element_type,
                        (COUNT != gdut::dynamic_extent ? COUNT
                                                       : Extent - OFFSET)>(
          pbegin + OFFSET, (pbegin + Extent));
    } else {
      return gdut::span<element_type, COUNT != gdut::dynamic_extent
                                          ? COUNT
                                          : Extent - OFFSET>(
          pbegin + OFFSET, pbegin + OFFSET + COUNT);
    }
  }
#endif

  //*************************************************************************
  /// Obtains a span that is a view from 'offset' over the next 'count' elements
  /// of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, gdut::dynamic_extent>
  subspan(size_t offset, size_t count = gdut::dynamic_extent) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return (offset <= size()) &&
                   (count != gdut::dynamic_extent ? count <= (size() - offset)
                                                  : true)
               ? ((count == gdut::dynamic_extent)
                      ? gdut::span<element_type, gdut::dynamic_extent>(
                            pbegin + offset, (pbegin + Extent))
                      : gdut::span<element_type, gdut::dynamic_extent>(
                            pbegin + offset, pbegin + offset + count))
               : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(offset <= size(), GDUT_ERROR(span_out_of_range));
    GDUT_ASSERT_CHECK_EXTRA(
        count != gdut::dynamic_extent ? count <= (size() - offset) : true,
        GDUT_ERROR(span_out_of_range));

    return (count == gdut::dynamic_extent)
               ? gdut::span<element_type, gdut::dynamic_extent>(
                     pbegin + offset, (pbegin + Extent))
               : gdut::span<element_type, gdut::dynamic_extent>(
                     pbegin + offset, pbegin + offset + count);
#endif
  }

  //*************************************************************************
  /// Moves the pointer to the first element of the span further by a specified
  /// number of elements.
  ///\tparam elements Number of elements to move forward
  //*************************************************************************
  void advance(size_t elements) GDUT_NOEXCEPT {
    elements = gdut::min(elements, size());
    pbegin += elements;
  }

  //*************************************************************************
  /// Reinterpret the span as a span with different element type.
  //*************************************************************************
  template <typename TNew>
  GDUT_NODISCARD GDUT_CONSTEXPR14
      gdut::span<TNew, Extent * sizeof(element_type) / sizeof(TNew)>
      reinterpret_as() const {
    GDUT_ASSERT(gdut::is_aligned<gdut::alignment_of<TNew>::value>(pbegin),
                GDUT_ERROR(span_alignment_exception));

    return gdut::span<TNew, Extent * sizeof(element_type) / sizeof(TNew)>(
        reinterpret_cast<TNew *>(pbegin),
        Extent * sizeof(element_type) / sizeof(TNew));
  }

private:
  pointer pbegin;
};

//*************************************************************************
/// Pseudo constructor for constructing from C array without explicitly
/// specifying type and size
//*************************************************************************
template <typename T, size_t Extent>
GDUT_CONSTEXPR span<T, Extent> make_span(T (&data)[Extent]) {
  return span<T, Extent>(data);
}

//***************************************************************************
/// Span - Dynamic Extent
//***************************************************************************
template <typename T> class span<T, gdut::dynamic_extent> : public span_tag {
public:
  typedef T element_type;
  typedef typename gdut::remove_cv<T>::type value_type;
  typedef size_t size_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *pointer;
  typedef const T *const_pointer;

  typedef T *iterator;
  typedef const T *const_iterator;
  typedef GDUT_OR_STD::reverse_iterator<iterator> reverse_iterator;
  typedef GDUT_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;

  typedef gdut::circular_iterator<pointer> circular_iterator;
  typedef gdut::circular_iterator<GDUT_OR_STD::reverse_iterator<pointer>>
      reverse_circular_iterator;

  static GDUT_CONSTANT size_t extent = gdut::dynamic_extent;

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR span() GDUT_NOEXCEPT : pbegin(GDUT_NULLPTR),
                                        pend(GDUT_NULLPTR) {}

  //*************************************************************************
  /// Construct from pointer + size
  //*************************************************************************
  template <typename TIterator, typename TSize>
  GDUT_CONSTEXPR span(const TIterator begin_, const TSize size_) GDUT_NOEXCEPT
      : pbegin(gdut::to_address(begin_)),
        pend(gdut::to_address(begin_) + size_) {}

  //*************************************************************************
  /// Construct from iterators
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR span(const TIterator begin_,
                      const TIterator end_) GDUT_NOEXCEPT
      : pbegin(gdut::to_address(begin_)),
        pend(gdut::to_address(begin_) + gdut::distance(begin_, end_)) {}

  //*************************************************************************
  /// Construct from C array
  //*************************************************************************
  template <size_t Array_Size>
  GDUT_CONSTEXPR span(element_type (&begin_)[Array_Size]) GDUT_NOEXCEPT
      : pbegin(begin_),
        pend(begin_ + Array_Size) {}

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <
      typename TContainer,
      typename = typename gdut::enable_if<
          !gdut::is_base_of<span_tag,
                            gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_pointer<gdut::remove_reference_t<TContainer>>::value &&
              !gdut::is_array<gdut::remove_reference_t<TContainer>>::value &&
              gdut::is_same<gdut::remove_cv_t<T>,
                            gdut::remove_cv_t<typename gdut::remove_reference_t<
                                TContainer>::value_type>>::value,
          void>::type>
  GDUT_CONSTEXPR span(TContainer &&a) GDUT_NOEXCEPT
      : pbegin(a.data()),
        pend(a.data() + a.size()) {}
#else
  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <typename TContainer>
  GDUT_CONSTEXPR
  span(TContainer &a,
       typename gdut::enable_if<
           !gdut::is_base_of<span_tag, typename gdut::remove_reference<
                                           TContainer>::type>::value &&
               !gdut::is_pointer<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_array<TContainer>::value &&
               gdut::is_same<
                   typename gdut::remove_cv<T>::type,
                   typename gdut::remove_cv<typename gdut::remove_reference<
                       TContainer>::type::value_type>::type>::value,
           void>::type * = 0) GDUT_NOEXCEPT : pbegin(a.data()),
                                              pend(a.data() + a.size()) {}

  //*************************************************************************
  /// Construct from a container or other type that supports
  /// data() and size() member functions.
  //*************************************************************************
  template <typename TContainer>
  GDUT_CONSTEXPR
  span(const TContainer &a,
       typename gdut::enable_if<
           !gdut::is_base_of<span_tag, typename gdut::remove_reference<
                                           TContainer>::type>::value &&
               !gdut::is_pointer<
                   typename gdut::remove_reference<TContainer>::type>::value &&
               !gdut::is_array<TContainer>::value &&
               gdut::is_same<
                   typename gdut::remove_cv<T>::type,
                   typename gdut::remove_cv<typename gdut::remove_reference<
                       TContainer>::type::value_type>::type>::value,
           void>::type * = 0) GDUT_NOEXCEPT : pbegin(a.data()),
                                              pend(a.data() + a.size()) {}
#endif

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  GDUT_CONSTEXPR span(const span &other) GDUT_NOEXCEPT : pbegin(other.pbegin),
                                                         pend(other.pend) {}

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  template <typename U, size_t Size>
  GDUT_CONSTEXPR span(const gdut::span<U, Size> &other) GDUT_NOEXCEPT
      : pbegin(other.data()),
        pend(other.data() + other.size()) {}

  //*************************************************************************
  /// Returns a reference to the first element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference front() const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return size() > 0 ? *pbegin : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(size() > 0, GDUT_ERROR(span_out_of_range));

    return *pbegin;
#endif
  }

  //*************************************************************************
  /// Returns a reference to the last element.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reference back() const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return size() > 0 ? *(pend - 1) : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(size() > 0, GDUT_ERROR(span_out_of_range));

    return *(pend - 1);
#endif
  }

  //*************************************************************************
  /// Returns a pointer to the first element of the internal storage.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR pointer data() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns a const iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_iterator cbegin() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns an iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator begin() const GDUT_NOEXCEPT {
    return pbegin;
  }

  //*************************************************************************
  /// Returns a circular iterator to the beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR circular_iterator
  begin_circular() const GDUT_NOEXCEPT {
    return circular_iterator(begin(), end());
  }

  //*************************************************************************
  /// Returns a const iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_iterator cend() const GDUT_NOEXCEPT {
    return pend;
  }

  //*************************************************************************
  /// Returns an iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR iterator end() const GDUT_NOEXCEPT {
    return pend;
  }

  //*************************************************************************
  // Returns an reverse iterator to the reverse beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rbegin() const GDUT_NOEXCEPT {
    return reverse_iterator(pend);
  }

  //*************************************************************************
  // Returns a const reverse iterator to the reverse beginning of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_reverse_iterator
  crbegin() const GDUT_NOEXCEPT {
    return const_reverse_iterator(pend);
  }

  //*************************************************************************
  /// Returns a reverse circular iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_circular_iterator
  rbegin_circular() const GDUT_NOEXCEPT {
    return reverse_circular_iterator(rbegin(), rend());
  }

  //*************************************************************************
  /// Returns a const reverse iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR const_reverse_iterator
  crend() const GDUT_NOEXCEPT {
    return const_reverse_iterator(pbegin);
  }

  //*************************************************************************
  /// Returns a reverse iterator to the end of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR reverse_iterator rend() const GDUT_NOEXCEPT {
    return reverse_iterator(pbegin);
  }

  //*************************************************************************
  /// Returns <b>true</b> if the span size is zero.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR bool empty() const GDUT_NOEXCEPT {
    return (pbegin == pend);
  }

  //*************************************************************************
  /// Returns the size of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size() const GDUT_NOEXCEPT {
    return (pend - pbegin);
  }

  //*************************************************************************
  /// Returns the size of the span in bytes.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t size_bytes() const GDUT_NOEXCEPT {
    return sizeof(element_type) * (pend - pbegin);
  }

  //*************************************************************************
  /// Returns the maximum possible size of the span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR size_t max_size() const GDUT_NOEXCEPT {
    return size();
  }

  //*************************************************************************
  /// Assign from a span.
  //*************************************************************************
  GDUT_CONSTEXPR14 span &operator=(const span &other) GDUT_NOEXCEPT {
    pbegin = other.pbegin;
    pend = other.pend;
    return *this;
  }

  //*************************************************************************
  /// Returns a reference to the value at index 'i'.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR14 reference at(size_t i) {
    GDUT_ASSERT(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
  }

  //*************************************************************************
  /// Returns a const reference to the value at index 'i'.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR14 const_reference at(size_t i) const {
    GDUT_ASSERT(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
  }

  //*************************************************************************
  /// Returns a reference to the indexed value.
  //*************************************************************************
  GDUT_CONSTEXPR reference operator[](const size_t i) const {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_INDEX_OPERATOR
    return i < size() ? pbegin[i] : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_INDEX_OPERATOR(i < size(), GDUT_ERROR(span_out_of_range));

    return pbegin[i];
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the first COUNT elements of this span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, COUNT> first() const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return COUNT <= size()
               ? gdut::span<element_type, COUNT>(pbegin, pbegin + COUNT)
               : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(COUNT <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, COUNT>(pbegin, pbegin + COUNT);
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the first count elements of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, gdut::dynamic_extent>
  first(size_t count) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return count <= size() ? gdut::span<element_type, gdut::dynamic_extent>(
                                 pbegin, pbegin + count)
                           : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(count <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, gdut::dynamic_extent>(pbegin,
                                                          pbegin + count);
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the last COUNT elements of this span.
  //*************************************************************************
  template <size_t COUNT>
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, COUNT> last() const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return COUNT <= size() ? gdut::span<element_type, COUNT>(pend - COUNT, pend)
                           : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(COUNT <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, COUNT>(pend - COUNT, pend);
#endif
  }

  //*************************************************************************
  /// Obtains a span that is a view over the last count elements of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR gdut::span<element_type, gdut::dynamic_extent>
  last(size_t count) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return count <= size() ? gdut::span<element_type, gdut::dynamic_extent>(
                                 pend - count, pend)
                           : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(count <= size(), GDUT_ERROR(span_out_of_range));

    return gdut::span<element_type, gdut::dynamic_extent>(pend - count, pend);
#endif
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Obtains a span that is a view from OFFSET over the next COUNT elements of
  /// this span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT = gdut::dynamic_extent>
  GDUT_NODISCARD GDUT_CONSTEXPR
      gdut::span<element_type,
                 COUNT != gdut::dynamic_extent ? COUNT : gdut::dynamic_extent>
      subspan() const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
#if GDUT_USING_CPP11 && GDUT_NOT_USING_CPP14 && GDUT_USING_EXCEPTIONS &&       \
    GDUT_CHECKING_EXTRA
    return (OFFSET <= size()) &&
                   (COUNT != gdut::dynamic_extent ? COUNT <= (size() - OFFSET)
                                                  : true)
               ? ((COUNT == gdut::dynamic_extent)
                      ? gdut::span<element_type, COUNT != gdut::dynamic_extent
                                                     ? COUNT
                                                     : gdut::dynamic_extent>(
                            pbegin + OFFSET, pend)
                      : gdut::span<element_type, COUNT != gdut::dynamic_extent
                                                     ? COUNT
                                                     : gdut::dynamic_extent>(
                            pbegin + OFFSET, pbegin + OFFSET + COUNT))
               : throw(GDUT_ERROR(span_out_of_range));
#else
    GDUT_ASSERT_CHECK_EXTRA(OFFSET <= size(), GDUT_ERROR(span_out_of_range));
    GDUT_ASSERT_CHECK_EXTRA(
        COUNT != gdut::dynamic_extent ? COUNT <= (size() - OFFSET) : true,
        GDUT_ERROR(span_out_of_range));

    return (COUNT == gdut::dynamic_extent)
               ? gdut::span<element_type, COUNT != gdut::dynamic_extent
                                              ? COUNT
                                              : gdut::dynamic_extent>(
                     pbegin + OFFSET, pend)
               : gdut::span<element_type, COUNT != gdut::dynamic_extent
                                              ? COUNT
                                              : gdut::dynamic_extent>(
                     pbegin + OFFSET, pbegin + OFFSET + COUNT);
#endif
  }
#else
  //*************************************************************************
  /// Obtains a span that is a view from OFFSET over the next COUNT elements of
  /// this span.
  //*************************************************************************
  template <size_t OFFSET, size_t COUNT>
  gdut::span<element_type,
             COUNT != gdut::dynamic_extent ? COUNT : gdut::dynamic_extent>
  subspan() const {
    GDUT_ASSERT_CHECK_EXTRA(OFFSET <= size(), GDUT_ERROR(span_out_of_range));
    GDUT_ASSERT_CHECK_EXTRA(
        COUNT != gdut::dynamic_extent ? COUNT <= (size() - OFFSET) : true,
        GDUT_ERROR(span_out_of_range));

    if (COUNT == gdut::dynamic_extent) {
      return gdut::span<element_type, COUNT != gdut::dynamic_extent
                                          ? COUNT
                                          : gdut::dynamic_extent>(
          pbegin + OFFSET, pend);
    } else {
      return gdut::span<element_type, COUNT != gdut::dynamic_extent
                                          ? COUNT
                                          : gdut::dynamic_extent>(
          pbegin + OFFSET, pbegin + OFFSET + COUNT);
    }
  }
#endif

  //*************************************************************************
  /// Obtains a span that is a view from 'offset' over the next 'count' elements
  /// of this span.
  //*************************************************************************
  GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::span<element_type, gdut::dynamic_extent>
  subspan(size_t offset, size_t count = gdut::dynamic_extent) const
      GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS || GDUT_NOT_CHECKING_EXTRA) {
    GDUT_ASSERT_CHECK_EXTRA(offset <= size(), GDUT_ERROR(span_out_of_range));
    GDUT_ASSERT_CHECK_EXTRA(
        count != gdut::dynamic_extent ? count <= (size() - offset) : true,
        GDUT_ERROR(span_out_of_range));

    return (count == gdut::dynamic_extent)
               ? gdut::span<element_type, gdut::dynamic_extent>(pbegin + offset,
                                                                pend)
               : gdut::span<element_type, gdut::dynamic_extent>(
                     pbegin + offset, pbegin + offset + count);
  }

  //*************************************************************************
  /// Moves the pointer to the first element of the span further by a specified
  /// number of elements.
  ///\tparam elements Number of elements to move forward
  //*************************************************************************
  void advance(size_t elements) GDUT_NOEXCEPT {
    elements = gdut::min(elements, size());
    pbegin += elements;
  }

  //*************************************************************************
  /// Reinterpret the span as a span with different element type.
  //*************************************************************************
  template <typename TNew>
  GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::span<TNew, gdut::dynamic_extent>
  reinterpret_as() const {
    GDUT_ASSERT(gdut::is_aligned<gdut::alignment_of<TNew>::value>(pbegin),
                GDUT_ERROR(span_alignment_exception));

    return gdut::span<TNew, gdut::dynamic_extent>(
        reinterpret_cast<TNew *>(pbegin),
        (pend - pbegin) * sizeof(element_type) / sizeof(TNew));
  }

  //*************************************************************************
  /// Split off and return an initial span of specified type of this span.
  /// The original span is advanced by the size of the returned span.
  /// \tparam TRet Returned span type
  /// \param n Number of elements in returned span
  //*************************************************************************
  template <typename TRet>
  GDUT_NODISCARD gdut::span<TRet> take(size_t const n) {
    GDUT_STATIC_ASSERT(sizeof(TRet) % sizeof(element_type) == 0,
                       "sizeof(TRet) must be divisible by sizeof(T)");

    GDUT_ASSERT(gdut::is_aligned<gdut::alignment_of<TRet>::value>(pbegin),
                GDUT_ERROR(span_alignment_exception));
    GDUT_ASSERT(sizeof(TRet) * n <= sizeof(element_type) * size(),
                GDUT_ERROR(span_size_mismatch));

    gdut::span<TRet> result = reinterpret_as<TRet>().first(n);
    advance(sizeof(TRet) / sizeof(element_type) * n);

    return result;
  }

  //*************************************************************************
  /// Split off and return an initial value of specified type of this span.
  /// The original span is advanced by the size of TRet
  /// \tparam TRet Returned span type
  //*************************************************************************
  template <typename TRet> GDUT_NODISCARD TRet &take() {
    GDUT_STATIC_ASSERT(sizeof(TRet) % sizeof(element_type) == 0,
                       "sizeof(TRet) must be divisible by sizeof(T)");

    GDUT_ASSERT(gdut::is_aligned<gdut::alignment_of<TRet>::value>(pbegin),
                GDUT_ERROR(span_alignment_exception));
    GDUT_ASSERT(sizeof(TRet) <= sizeof(element_type) * size(),
                GDUT_ERROR(span_size_mismatch));

    TRet &result = *reinterpret_cast<TRet *>(data());
    advance(sizeof(TRet) / sizeof(element_type));

    return result;
  }

private:
  pointer pbegin;
  pointer pend;
};

//*************************************************************************
/// Pseudo constructor for constructing from container without explicitly
/// specifying type and size
//*************************************************************************
template <typename T>
GDUT_CONSTEXPR span<typename T::value_type, gdut::dynamic_extent>
make_span(T &data) {
  return span<typename T::value_type, gdut::dynamic_extent>(data);
}

//*************************************************************************
/// Pseudo constructor for constructing from const container without
/// explicitly specifying type and size
//*************************************************************************
template <typename T>
GDUT_CONSTEXPR span<typename T::value_type const, gdut::dynamic_extent>
make_span(const T &data) {
  return span<typename T::value_type const, gdut::dynamic_extent>(data);
}

template <typename T, size_t Extent>
GDUT_CONSTANT size_t span<T, Extent>::extent;

template <typename T>
GDUT_CONSTANT size_t span<T, gdut::dynamic_extent>::extent;

//*************************************************************************
/// Compare two spans for equality.
//*************************************************************************
template <typename T1, size_t N1, typename T2, size_t N2>
GDUT_NODISCARD GDUT_CONSTEXPR typename gdut::enable_if<
    gdut::is_same<typename gdut::remove_cv<T1>::type,
                  typename gdut::remove_cv<T2>::type>::value,
    bool>::type
operator==(const gdut::span<T1, N1> &lhs,
           const gdut::span<T2, N2> &rhs) GDUT_NOEXCEPT {
  return (lhs.begin() == rhs.begin()) && (lhs.size() == rhs.size());
}

//*************************************************************************
/// Compare two spans for inequality.
//*************************************************************************
template <typename T1, size_t N1, typename T2, size_t N2>
GDUT_NODISCARD GDUT_CONSTEXPR bool
operator!=(const gdut::span<T1, N1> &lhs,
           const gdut::span<T2, N2> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// Equality function.
/// Performs a comparison of the range values.
/// Returns <b>true</b> if one of the following are <b>true</b>
/// 1. Both spans are empty.
/// 2. They both point to the same range of data.
/// 3. The values in the two ranges are equal.
//*************************************************************************
template <typename T1, size_t N1, typename T2, size_t N2>
typename gdut::enable_if<
    gdut::is_same<typename gdut::remove_cv<T1>::type,
                  typename gdut::remove_cv<T2>::type>::value,
    bool>::type
equal(const gdut::span<T1, N1> &lhs, const gdut::span<T2, N2> &rhs) {
  return (lhs.empty() && rhs.empty()) ||
         ((lhs.begin() == rhs.begin()) && (lhs.size() == rhs.size())) ||
         gdut::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

//*************************************************************************
/// Copy complete element data from one span to another. If the destination
/// span is bigger than the source span, only the initial part of
/// destination span is overwritten.
///\param src Source
///\param dst Destination
///\return true, if copy was successful (including empty source span, or
///        spans pointing to the same address)
///\return false, if the destination span is shorter than the source span.
//*************************************************************************
template <typename T1, size_t N1, typename T2, size_t N2>
typename gdut::enable_if<
    gdut::is_same<typename gdut::remove_cv<T1>::type,
                  typename gdut::remove_cv<T2>::type>::value &&
        !gdut::is_const<T2>::value,
    bool>::type
copy(const gdut::span<T1, N1> &src, const gdut::span<T2, N2> &dst) {
  if (src.empty() || (src.begin() == dst.begin())) {
    return true;
  }
  if (src.size() > dst.size()) {
    return false;
  }
  (void)gdut::copy(src.begin(), src.end(), dst.begin());
  return true;
}

//*************************************************************************
/// Template deduction guides.
//*************************************************************************
#if GDUT_USING_CPP17
template <typename TIterator>
span(const TIterator begin_, const TIterator end_)
    -> span<gdut::remove_pointer_t<TIterator>, gdut::dynamic_extent>;

template <typename TIterator, typename TSize>
span(const TIterator begin_, const TSize size_)
    -> span<gdut::remove_pointer_t<TIterator>, gdut::dynamic_extent>;

template <typename T, size_t Size> span(T (&)[Size]) -> span<T, Size>;

template <typename T, size_t Size>
span(gdut::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const gdut::array<T, Size> &) -> span<const T, Size>;

#if GDUT_USING_STL && GDUT_USING_CPP11
template <typename T, size_t Size> span(std::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const std::array<T, Size> &) -> span<const T, Size>;
#endif
#endif

//*************************************************************************
/// Hash function.
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <typename T, size_t Extent> struct hash<gdut::span<T, Extent>> {
  size_t operator()(const gdut::span<T> &view) const {
    return gdut::private_hash::generic_hash<size_t>(
        reinterpret_cast<const uint8_t *>(view.data()),
        reinterpret_cast<const uint8_t *>(view.data() + view.size()));
  }
};
#endif

//*************************************************************************
/// Obtains a view to the byte representation of the elements of the span s.
//*************************************************************************
template <class T, size_t Size>
span<const byte, (Size == gdut::dynamic_extent) ? (gdut::dynamic_extent)
                                                : (Size * sizeof(T))>
as_bytes(span<T, Size> s) GDUT_NOEXCEPT {
  return span<const byte, (Size == gdut::dynamic_extent)
                              ? (gdut::dynamic_extent)
                              : (Size * sizeof(T))>(
      reinterpret_cast<byte *>(s.data()), s.size_bytes());
}

//*************************************************************************
/// Obtains a view to the byte representation of the elements of the span s.
//*************************************************************************
template <class T, size_t Size>
span<byte, (Size == gdut::dynamic_extent) ? (gdut::dynamic_extent)
                                          : (Size * sizeof(T))>
as_writable_bytes(span<T, Size> s) GDUT_NOEXCEPT {
  GDUT_STATIC_ASSERT(!gdut::is_const<T>::value,
                     "span<T> must be of non-const type");
  return span<byte, (Size == gdut::dynamic_extent) ? (gdut::dynamic_extent)
                                                   : (Size * sizeof(T))>(
      reinterpret_cast<byte *>(s.data()), s.size_bytes());
}
} // namespace gdut

#endif
