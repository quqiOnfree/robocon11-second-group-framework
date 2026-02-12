///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2022 John Wellbelove

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

#ifndef GDUT_UNALIGNED_TYPE_INCLUDED
#define GDUT_UNALIGNED_TYPE_INCLUDED

///\defgroup unaligned_types unaligned_types
/// Unaligned types utilities
///\ingroup utilities

#include "algorithm.hpp"
#include "array.hpp"
#include "binary.hpp"
#include "bit.hpp"
#include "endianness.hpp"
#include "exception.hpp"
#include "file_error_numbers.hpp"
#include "iterator.hpp"
#include "platform.hpp"
#include "type_traits.hpp"

#if GDUT_USING_CPP20 && GDUT_USING_STL
#include <bit>
#endif

#include <string.h>

namespace gdut {
struct unaligned_type_exception : public gdut::exception {
public:
  unaligned_type_exception(string_type reason_, string_type file_name_,
                           numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
/// Buffer size exception.
//***************************************************************************
class unaligned_type_buffer_size : public unaligned_type_exception {
public:
  unaligned_type_buffer_size(string_type file_name_, numeric_type line_number_)
      : unaligned_type_exception(GDUT_ERROR_TEXT("unaligned_type:buffer size",
                                                 GDUT_UNALIGNED_TYPE_FILE_ID
                                                 "A"),
                                 file_name_, line_number_) {}
};

namespace private_unaligned_type {
//*************************************************************************
/// unaligned_type_common
/// Contains all functionality that doesn't require the type.
/// GDUT_PACKED ensures that GCC does not complain when used in a packed object.
//*************************************************************************
template <size_t Size_, typename TDerivedType>
GDUT_PACKED_CLASS(unaligned_type_common) {
public:
  typedef TDerivedType derived_type;
  typedef unsigned char storage_type;
  typedef storage_type *pointer;
  typedef const storage_type *const_pointer;
  typedef storage_type *iterator;
  typedef const storage_type *const_iterator;
  typedef gdut::reverse_iterator<iterator> reverse_iterator;
  typedef gdut::reverse_iterator<const_iterator> const_reverse_iterator;

  //*************************************************************************
  /// Default constructor
  //*************************************************************************
  unaligned_type_common() {}

  //*************************************************************************
  /// Size of the storage.
  //*************************************************************************
  size_t size() const { return Size_; }

  //*************************************************************************
  /// Pointer to the beginning of the storage.
  //*************************************************************************
  pointer data() { return get_storage(); }

  //*************************************************************************
  /// Const pointer to the beginning of the storage.
  //*************************************************************************
  const_pointer data() const { return get_storage(); }

  //*************************************************************************
  /// Iterator to the beginning of the storage.
  //*************************************************************************
  iterator begin() { return iterator(get_storage()); }

  //*************************************************************************
  /// Const iterator to the beginning of the storage.
  //*************************************************************************
  const_iterator begin() const { return const_iterator(get_storage()); }

  //*************************************************************************
  /// Const iterator to the beginning of the storage.
  //*************************************************************************
  const_iterator cbegin() const { return const_iterator(get_storage()); }

  //*************************************************************************
  /// Reverse iterator to the beginning of the storage.
  //*************************************************************************
  reverse_iterator rbegin() { return reverse_iterator(get_storage() + Size_); }

  //*************************************************************************
  /// Const reverse iterator to the beginning of the storage.
  //*************************************************************************
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(get_storage() + Size_);
  }

  //*************************************************************************
  /// Const reverse iterator to the beginning of the storage.
  //*************************************************************************
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(get_storage() + Size_);
  }

  //*************************************************************************
  /// Iterator to the end of the storage.
  //*************************************************************************
  iterator end() { return iterator(get_storage() + Size_); }

  //*************************************************************************
  /// Const iterator to the end of the storage.
  //*************************************************************************
  const_iterator end() const { return const_iterator(get_storage() + Size_); }

  //*************************************************************************
  /// Const iterator to the end of the storage.
  //*************************************************************************
  const_iterator cend() const { return const_iterator(get_storage() + Size_); }

  //*************************************************************************
  /// Reverse iterator to the end of the storage.
  //*************************************************************************
  reverse_iterator rend() { return reverse_iterator(get_storage()); }

  //*************************************************************************
  /// Const reverse iterator to the end of the storage.
  //*************************************************************************
  const_reverse_iterator rend() const {
    return const_reverse_iterator(get_storage());
  }

  //*************************************************************************
  /// Const reverse iterator to the end of the storage.
  //*************************************************************************
  const_reverse_iterator crend() const {
    return const_reverse_iterator(get_storage());
  }

  //*************************************************************************
  /// Index operator.
  //*************************************************************************
  storage_type &operator[](int i) { return get_storage()[i]; }

  //*************************************************************************
  /// Const index operator.
  //*************************************************************************
  const storage_type &operator[](int i) const { return get_storage()[i]; }

private:
  //*************************************************************************
  /// Get a pointer to the storage.
  //*************************************************************************
  pointer get_storage() { return static_cast<derived_type *>(this)->storage; }

  //*************************************************************************
  /// Get a const pointer to the storage.
  //*************************************************************************
  const_pointer get_storage() const {
    return static_cast<const derived_type *>(this)->storage;
  }
};
GDUT_END_PACKED

//*************************************************************************
/// unaligned_type_storage
/// Contains the fixed storage for a type.
/// GDUT_PACKED ensures that GCC does not complain when used in a packed object.
//*************************************************************************
template <size_t Size_>
GDUT_PACKED_CLASS(unaligned_type_storage)
    : public unaligned_type_common<Size_, unaligned_type_storage<Size_>> {
public:
  friend class unaligned_type_common<Size_, unaligned_type_storage<Size_>>;

protected:
  //*******************************
  unaligned_type_storage() : storage() {}

  unsigned char storage[Size_];
};
GDUT_END_PACKED

//*************************************************************************
/// unaligned_type_storage_ext
/// Contains a pointer to the fixed storage for a type.
/// GDUT_PACKED ensures that GCC does not complain when used in a packed object.
//*************************************************************************
template <size_t Size_>
GDUT_PACKED_CLASS(unaligned_type_storage_ext)
    : public unaligned_type_common<Size_, unaligned_type_storage_ext<Size_>> {
public:
  friend class unaligned_type_common<Size_, unaligned_type_storage_ext<Size_>>;

protected:
  //*******************************
  unaligned_type_storage_ext(unsigned char *storage_) : storage(storage_) {}

  //*******************************
  unaligned_type_storage_ext(const unaligned_type_storage_ext<Size_> &other)
      : storage(other.storage) {}

  //*******************************
  unaligned_type_storage_ext &operator=(
      const unaligned_type_storage_ext<Size_> &other) {
    storage = other.storage;

    return *this;
  }

  unsigned char *storage;
};
GDUT_END_PACKED

//*************************************************************************
/// Unaligned copy
//*************************************************************************
template <size_t Size_, int Endian_, bool Is_Integral> class unaligned_copy;

//*************************************************************************
/// Unaligned copy
/// For integrals.
//*************************************************************************
template <size_t Size_, int Endian_>
GDUT_PACKED_CLASS(unaligned_copy)<Size_, Endian_, true> {
public:
  typedef typename private_unaligned_type::unaligned_type_storage<
      Size_>::storage_type storage_type;
  typedef
      typename private_unaligned_type::unaligned_type_storage<Size_>::pointer
          pointer;
  typedef typename private_unaligned_type::unaligned_type_storage<
      Size_>::const_pointer const_pointer;

  //*******************************
  template <typename T>
  static void copy_value_to_store(const T &value, pointer store) {
    memcpy(store, &value, Size_);

#if GDUT_HAS_CONSTEXPR_ENDIANESS
    if GDUT_IF_CONSTEXPR (Endian_ == gdut::endianness::value())
#else
    if (Endian_ != gdut::endianness::value())
#endif
    {
      gdut::reverse(store, store + Size_);
    }
  }

  //*******************************
  template <typename T>
  static void copy_store_to_value(const_pointer store, T & value) {
    memcpy(&value, store, Size_);

#if GDUT_HAS_CONSTEXPR_ENDIANESS
    if GDUT_IF_CONSTEXPR (Endian == gdut::endianness::value())
#else
    if (Endian_ != gdut::endianness::value())
#endif
    {
      value = gdut::reverse_bytes(value);
    }
  }

  //*******************************
  static void copy_store_to_store(const_pointer src, int endian_src,
                                  pointer dst) {
    memcpy(dst, src, Size_);

    if (Endian_ != endian_src) {
      gdut::reverse(dst, dst + Size_);
    }
  }
};
GDUT_END_PACKED

//*************************************************************************
/// Unaligned copy
/// For floating point.
//*************************************************************************
template <size_t Size_, int Endian_>
GDUT_PACKED_CLASS(unaligned_copy)<Size_, Endian_, false> {
public:
  typedef typename private_unaligned_type::unaligned_type_storage<
      Size_>::storage_type storage_type;
  typedef
      typename private_unaligned_type::unaligned_type_storage<Size_>::pointer
          pointer;
  typedef typename private_unaligned_type::unaligned_type_storage<
      Size_>::const_pointer const_pointer;

  //*******************************
  template <typename T>
  static void copy_value_to_store(const T &value, pointer store) {
    memcpy(store, &value, Size_);

#if GDUT_HAS_CONSTEXPR_ENDIANESS
    if GDUT_IF_CONSTEXPR (Endian_ == gdut::endianness::value())
#else
    if (Endian_ != gdut::endianness::value())
#endif
    {
      gdut::reverse(store, store + Size_);
    }
  }

  //*******************************
  template <typename T>
  static void copy_store_to_value(const_pointer store, T & value) {
    memcpy(&value, store, Size_);

#if GDUT_HAS_CONSTEXPR_ENDIANESS
    if GDUT_IF_CONSTEXPR (Endian == gdut::endianness::value())
#else
    if (Endian_ != gdut::endianness::value())
#endif
    {
      gdut::reverse(reinterpret_cast<pointer>(&value),
                    reinterpret_cast<pointer>(&value) + Size_);
    }
  }

  //*******************************
  static void copy_store_to_store(const_pointer src, int endian_src,
                                  pointer dst) {
    memcpy(dst, src, Size_);

    if (Endian_ != endian_src) {
      gdut::reverse(dst, dst + Size_);
    }
  }
};
GDUT_END_PACKED
} // namespace private_unaligned_type

//*************************************************************************
/// unaligned_type
///\brief Allows an arithmetic type to be stored at an unaligned address.
///\tparam T      The arithmetic type.
///\tparam Endian The endianness of the arithmetic type.
//*************************************************************************
template <typename T, int Endian_>
GDUT_PACKED_CLASS(unaligned_type)
    : public private_unaligned_type::unaligned_type_storage<sizeof(T)> {
public:
  GDUT_STATIC_ASSERT(gdut::is_integral<T>::value ||
                         gdut::is_floating_point<T>::value,
                     "Unaligned type must be integral or floating point");

  typedef T value_type;

  typedef private_unaligned_type::unaligned_copy<
      sizeof(T), Endian_, gdut::is_floating_point<T>::value ? false : true>
      unaligned_copy;

  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::storage_type storage_type;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::pointer pointer;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::const_pointer const_pointer;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::iterator iterator;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::const_iterator const_iterator;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::reverse_iterator reverse_iterator;
  typedef typename private_unaligned_type::unaligned_type_storage<sizeof(
      T)>::const_reverse_iterator const_reverse_iterator;

  static GDUT_CONSTANT int Endian = Endian_;
  static GDUT_CONSTANT size_t Size = sizeof(T);

  //*************************************************************************
  /// Default constructor
  //*************************************************************************
  unaligned_type() {}

  //*************************************************************************
  /// Construct from a value.
  //*************************************************************************
  unaligned_type(T value) {
    unaligned_copy::copy_value_to_store(value, this->storage);
  }

  //*************************************************************************
  /// Construct from an address.
  //*************************************************************************
  unaligned_type(const void *address) {
    gdut::copy_n(reinterpret_cast<const char *>(address), sizeof(T),
                 this->storage);
  }

  //*************************************************************************
  /// Construct from an address and buffer size.
  //*************************************************************************
  unaligned_type(const void *address, size_t buffer_size) {
    GDUT_ASSERT(sizeof(T) <= buffer_size,
                GDUT_ERROR(gdut::unaligned_type_buffer_size));

    gdut::copy_n(reinterpret_cast<const char *>(address), sizeof(T),
                 this->storage);
  }

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  unaligned_type(const unaligned_type<T, Endian> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian, this->storage);
  }

  //*************************************************************************
  /// Copy constructor
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type(const unaligned_type<T, Endian_Other> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian_Other,
                                        this->storage);
  }

  //*************************************************************************
  /// Assignment operator
  //*************************************************************************
  unaligned_type &operator=(T value) {
    unaligned_copy::copy_value_to_store(value, this->storage);

    return *this;
  }

  //*************************************************************************
  /// Assignment operator.
  //*************************************************************************
  unaligned_type &operator=(const unaligned_type<T, Endian_> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian_, this->storage);

    return *this;
  }

  //*************************************************************************
  /// Assignment operator from other endianness.
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type &operator=(const unaligned_type<T, Endian_Other> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian_Other,
                                        this->storage);

    return *this;
  }

  //*************************************************************************
  /// Conversion operator
  //*************************************************************************
  operator T() const {
    T value = T();

    unaligned_copy::copy_store_to_value(this->storage, value);

    return value;
  }

  //*************************************************************************
  /// Get the value.
  //*************************************************************************
  T value() const {
    T value = T();

    unaligned_copy::copy_store_to_value(this->storage, value);

    return value;
  }
};
GDUT_END_PACKED

template <typename T, int Endian_>
GDUT_CONSTANT int unaligned_type<T, Endian_>::Endian;

template <typename T, int Endian_>
GDUT_CONSTANT size_t unaligned_type<T, Endian_>::Size;

//*************************************************************************
/// unaligned_type_ext
///\brief Allows an arithmetic type to be stored at an unaligned address.
///       Uses an external buffer.
///\tparam T      The arithmetic type.
///\tparam Endian The endianness of the arithmetic type.
//*************************************************************************
template <typename T, int Endian_>
GDUT_PACKED_CLASS(unaligned_type_ext)
    : public private_unaligned_type::unaligned_type_storage_ext<sizeof(T)> {
public:
  GDUT_STATIC_ASSERT(gdut::is_integral<T>::value ||
                         gdut::is_floating_point<T>::value,
                     "Unaligned type must be integral or floating point");

  template <typename U, int Endian_Other> friend class unaligned_type_ext;

  typedef T value_type;

  typedef private_unaligned_type::unaligned_copy<
      sizeof(T), Endian_, gdut::is_floating_point<T>::value ? false : true>
      unaligned_copy;

  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::storage_type storage_type;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::pointer pointer;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::const_pointer const_pointer;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::iterator iterator;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::const_iterator const_iterator;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::reverse_iterator reverse_iterator;
  typedef typename private_unaligned_type::unaligned_type_storage_ext<sizeof(
      T)>::const_reverse_iterator const_reverse_iterator;

  static GDUT_CONSTANT int Endian = Endian_;
  static GDUT_CONSTANT size_t Size = sizeof(T);

  //*************************************************************************
  /// Construct from a storage pointer
  //*************************************************************************
  unaligned_type_ext(pointer storage_)
      : private_unaligned_type::unaligned_type_storage_ext<Size>(storage_) {}

  //*************************************************************************
  /// Construct from a value and storage pointer
  //*************************************************************************
  unaligned_type_ext(T value, pointer storage_)
      : private_unaligned_type::unaligned_type_storage_ext<Size>(storage_) {
    unaligned_copy::copy_value_to_store(value, this->storage);
  }

  //*************************************************************************
  /// Copy constructor with storage pointer
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type_ext(const unaligned_type_ext<T, Endian_Other> &other,
                     pointer storage_)
      : private_unaligned_type::unaligned_type_storage_ext<Size>(storage_) {
    unaligned_copy::copy_store_to_store(other.data(), Endian_Other,
                                        this->storage);
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Move constructor
  //*************************************************************************
  unaligned_type_ext(unaligned_type_ext<T, Endian> && other)
      : private_unaligned_type::unaligned_type_storage_ext<Size>(
            other.storage) {
    other.storage = GDUT_NULLPTR;
  }

  //*************************************************************************
  /// Move constructor
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type_ext(unaligned_type_ext<T, Endian_Other> && other)
      : private_unaligned_type::unaligned_type_storage_ext<Size>(
            other.storage) {
    // If we're constructing from a different endianess then we need to reverse
    // the data order.
    if (Endian != Endian_Other) {
      gdut::reverse(this->begin(), this->end());
    }

    other.storage = GDUT_NULLPTR;
  }
#endif

  //*************************************************************************
  /// Assignment operator
  //*************************************************************************
  unaligned_type_ext &operator=(T value) {
    unaligned_copy::copy_value_to_store(value, this->storage);

    return *this;
  }

  //*************************************************************************
  /// Copy assignment operator from other endianness.
  //*************************************************************************
  unaligned_type_ext &operator=(const unaligned_type_ext<T, Endian> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian, this->storage);

    return *this;
  }

  //*************************************************************************
  /// Copy assignment operator from other endianness.
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type_ext &operator=(
      const unaligned_type_ext<T, Endian_Other> &other) {
    unaligned_copy::copy_store_to_store(other.data(), Endian_Other,
                                        this->storage);

    return *this;
  }

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Move assignment operator from other endianness.
  //*************************************************************************
  unaligned_type_ext &operator=(unaligned_type_ext<T, Endian> &&other) {
    this->storage = other.storage;
    other.storage = GDUT_NULLPTR;

    return *this;
  }

  //*************************************************************************
  /// Move assignment operator from other endianness.
  //*************************************************************************
  template <int Endian_Other>
  unaligned_type_ext &operator=(unaligned_type_ext<T, Endian_Other> &&other) {
    this->storage = other.storage;

    // If we're assigning from a different endianess then we need to reverse the
    // data order.
    if (Endian != Endian_Other) {
      gdut::reverse(this->begin(), this->end());
    }

    other.storage = GDUT_NULLPTR;

    return *this;
  }
#endif

  //*************************************************************************
  /// Conversion operator
  //*************************************************************************
  operator T() const {
    T value = T();

    unaligned_copy::copy_store_to_value(this->storage, value);

    return value;
  }

  //*************************************************************************
  /// Get the value.
  //*************************************************************************
  T value() const {
    T value = T();

    unaligned_copy::copy_store_to_value(this->storage, value);

    return value;
  }

  //*************************************************************************
  /// Sets the storage for the type.
  //*************************************************************************
  void set_storage(pointer storage_) { this->storage = storage_; }

private:
  unaligned_type_ext() GDUT_DELETE;
};
GDUT_END_PACKED

template <typename T, int Endian_>
GDUT_CONSTANT int unaligned_type_ext<T, Endian_>::Endian;

template <typename T, int Endian_>
GDUT_CONSTANT size_t unaligned_type_ext<T, Endian_>::Size;

#if GDUT_HAS_CONSTEXPR_ENDIANNESS
// Host order
typedef unaligned_type<char, gdut::endianness::value()> host_char_t;
typedef unaligned_type<signed char, gdut::endianness::value()> host_schar_t;
typedef unaligned_type<unsigned char, gdut::endianness::value()> host_uchar_t;
typedef unaligned_type<short, gdut::endianness::value()> host_short_t;
typedef unaligned_type<unsigned short, gdut::endianness::value()> host_ushort_t;
typedef unaligned_type<int, gdut::endianness::value()> host_int_t;
typedef unaligned_type<unsigned int, gdut::endianness::value()> host_uint_t;
typedef unaligned_type<long, gdut::endianness::value()> host_long_t;
typedef unaligned_type<unsigned long, gdut::endianness::value()> host_ulong_t;
typedef unaligned_type<long long, gdut::endianness::value()> host_long_long_t;
typedef unaligned_type<unsigned long long, gdut::endianness::value()>
    host_ulong_long_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type<int8_t, gdut::endianness::value()> host_int8_t;
typedef unaligned_type<uint8_t, gdut::endianness::value()> host_uint8_t;
#endif
typedef unaligned_type<int16_t, gdut::endianness::value()> host_int16_t;
typedef unaligned_type<uint16_t, gdut::endianness::value()> host_uint16_t;
typedef unaligned_type<int32_t, gdut::endianness::value()> host_int32_t;
typedef unaligned_type<uint32_t, gdut::endianness::value()> host_uint32_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type<int64_t, gdut::endianness::value()> host_int64_t;
typedef unaligned_type<uint64_t, gdut::endianness::value()> host_uint64_t;
#endif
typedef unaligned_type<float, gdut::endianness::value()> host_float_t;
typedef unaligned_type<double, gdut::endianness::value()> host_double_t;
typedef unaligned_type<long double, gdut::endianness::value()>
    host_long_double_t;
#endif

// Little Endian
typedef unaligned_type<char, gdut::endian::little> le_char_t;
typedef unaligned_type<signed char, gdut::endian::little> le_schar_t;
typedef unaligned_type<unsigned char, gdut::endian::little> le_uchar_t;
typedef unaligned_type<short, gdut::endian::little> le_short_t;
typedef unaligned_type<unsigned short, gdut::endian::little> le_ushort_t;
typedef unaligned_type<int, gdut::endian::little> le_int_t;
typedef unaligned_type<unsigned int, gdut::endian::little> le_uint_t;
typedef unaligned_type<long, gdut::endian::little> le_long_t;
typedef unaligned_type<unsigned long, gdut::endian::little> le_ulong_t;
typedef unaligned_type<long long, gdut::endian::little> le_long_long_t;
typedef unaligned_type<unsigned long long, gdut::endian::little>
    le_ulong_long_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type<int8_t, gdut::endian::little> le_int8_t;
typedef unaligned_type<uint8_t, gdut::endian::little> le_uint8_t;
#endif
typedef unaligned_type<int16_t, gdut::endian::little> le_int16_t;
typedef unaligned_type<uint16_t, gdut::endian::little> le_uint16_t;
typedef unaligned_type<int32_t, gdut::endian::little> le_int32_t;
typedef unaligned_type<uint32_t, gdut::endian::little> le_uint32_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type<int64_t, gdut::endian::little> le_int64_t;
typedef unaligned_type<uint64_t, gdut::endian::little> le_uint64_t;
#endif
typedef unaligned_type<float, gdut::endian::little> le_float_t;
typedef unaligned_type<double, gdut::endian::little> le_double_t;
typedef unaligned_type<long double, gdut::endian::little> le_long_double_t;

// Big Endian
typedef unaligned_type<char, gdut::endian::big> be_char_t;
typedef unaligned_type<signed char, gdut::endian::big> be_schar_t;
typedef unaligned_type<unsigned char, gdut::endian::big> be_uchar_t;
typedef unaligned_type<short, gdut::endian::big> be_short_t;
typedef unaligned_type<unsigned short, gdut::endian::big> be_ushort_t;
typedef unaligned_type<int, gdut::endian::big> be_int_t;
typedef unaligned_type<unsigned int, gdut::endian::big> be_uint_t;
typedef unaligned_type<long, gdut::endian::big> be_long_t;
typedef unaligned_type<unsigned long, gdut::endian::big> be_ulong_t;
typedef unaligned_type<long long, gdut::endian::big> be_long_long_t;
typedef unaligned_type<unsigned long long, gdut::endian::big> be_ulong_long_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type<int8_t, gdut::endian::big> be_int8_t;
typedef unaligned_type<uint8_t, gdut::endian::big> be_uint8_t;
#endif
typedef unaligned_type<int16_t, gdut::endian::big> be_int16_t;
typedef unaligned_type<uint16_t, gdut::endian::big> be_uint16_t;
typedef unaligned_type<int32_t, gdut::endian::big> be_int32_t;
typedef unaligned_type<uint32_t, gdut::endian::big> be_uint32_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type<int64_t, gdut::endian::big> be_int64_t;
typedef unaligned_type<uint64_t, gdut::endian::big> be_uint64_t;
#endif
typedef unaligned_type<float, gdut::endian::big> be_float_t;
typedef unaligned_type<double, gdut::endian::big> be_double_t;
typedef unaligned_type<long double, gdut::endian::big> be_long_double_t;

// Network Order
typedef be_char_t net_char_t;
typedef be_schar_t net_schar_t;
typedef be_uchar_t net_uchar_t;
typedef be_short_t net_short_t;
typedef be_ushort_t net_ushort_t;
typedef be_int_t net_int_t;
typedef be_uint_t net_uint_t;
typedef be_long_t net_long_t;
typedef be_ulong_t net_ulong_t;
typedef be_long_long_t net_long_long_t;
typedef be_ulong_long_t net_ulong_long_t;
#if GDUT_USING_8BIT_TYPES
typedef be_int8_t net_int8_t;
typedef be_uint8_t net_uint8_t;
#endif
typedef be_int16_t net_int16_t;
typedef be_uint16_t net_uint16_t;
typedef be_int32_t net_int32_t;
typedef be_uint32_t net_uint32_t;
#if GDUT_USING_64BIT_TYPES
typedef be_int64_t net_int64_t;
typedef be_uint64_t net_uint64_t;
#endif
typedef be_float_t net_float_t;
typedef be_double_t net_double_t;
typedef be_long_double_t net_long_double_t;

#if GDUT_USING_CPP11
template <typename T, int Endian>
using unaligned_type_t = typename gdut::unaligned_type<T, Endian>::type;
#endif

#if GDUT_USING_CPP17
template <typename T, int Endian>
constexpr size_t unaligned_type_v = gdut::unaligned_type<T, Endian>::Size;
#endif

#if GDUT_HAS_CONSTEXPR_ENDIANNESS
// Host order
typedef unaligned_type_ext<char, gdut::endianness::value()> host_char_ext_t;
typedef unaligned_type_ext<signed char, gdut::endianness::value()>
    host_schar_ext_t;
typedef unaligned_type_ext<unsigned char, gdut::endianness::value()>
    host_uchar_ext_t;
typedef unaligned_type_ext<short, gdut::endianness::value()> host_short_ext_t;
typedef unaligned_type_ext<unsigned short, gdut::endianness::value()>
    host_ushort_ext_t;
typedef unaligned_type_ext<int, gdut::endianness::value()> host_int_ext_t;
typedef unaligned_type_ext<unsigned int, gdut::endianness::value()>
    host_uint_ext_t;
typedef unaligned_type_ext<long, gdut::endianness::value()> host_long_ext_t;
typedef unaligned_type_ext<unsigned long, gdut::endianness::value()>
    host_ulong_ext_t;
typedef unaligned_type_ext<long long, gdut::endianness::value()>
    host_long_long_ext_t;
typedef unaligned_type_ext<unsigned long long, gdut::endianness::value()>
    host_ulong_long_ext_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type_ext<int8_t, gdut::endianness::value()> host_int8_ext_t;
typedef unaligned_type_ext<uint8_t, gdut::endianness::value()> host_uint8_ext_t;
#endif
typedef unaligned_type_ext<int16_t, gdut::endianness::value()> host_int16_ext_t;
typedef unaligned_type_ext<uint16_t, gdut::endianness::value()>
    host_uint16_ext_t;
typedef unaligned_type_ext<int32_t, gdut::endianness::value()> host_int32_ext_t;
typedef unaligned_type_ext<uint32_t, gdut::endianness::value()>
    host_uint32_ext_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type_ext<int64_t, gdut::endianness::value()> host_int64_ext_t;
typedef unaligned_type_ext<uint64_t, gdut::endianness::value()>
    host_uint64_ext_t;
#endif
typedef unaligned_type_ext<float, gdut::endianness::value()> host_float_ext_t;
typedef unaligned_type_ext<double, gdut::endianness::value()> host_double_ext_t;
typedef unaligned_type_ext<long double, gdut::endianness::value()>
    host_long_double_ext_t;
#endif

// Little Endian
typedef unaligned_type_ext<char, gdut::endian::little> le_char_ext_t;
typedef unaligned_type_ext<signed char, gdut::endian::little> le_schar_ext_t;
typedef unaligned_type_ext<unsigned char, gdut::endian::little> le_uchar_ext_t;
typedef unaligned_type_ext<short, gdut::endian::little> le_short_ext_t;
typedef unaligned_type_ext<unsigned short, gdut::endian::little>
    le_ushort_ext_t;
typedef unaligned_type_ext<int, gdut::endian::little> le_int_ext_t;
typedef unaligned_type_ext<unsigned int, gdut::endian::little> le_uint_ext_t;
typedef unaligned_type_ext<long, gdut::endian::little> le_long_ext_t;
typedef unaligned_type_ext<unsigned long, gdut::endian::little> le_ulong_ext_t;
typedef unaligned_type_ext<long long, gdut::endian::little> le_long_long_ext_t;
typedef unaligned_type_ext<unsigned long long, gdut::endian::little>
    le_ulong_long_ext_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type_ext<int8_t, gdut::endian::little> le_int8_ext_t;
typedef unaligned_type_ext<uint8_t, gdut::endian::little> le_uint8_ext_t;
#endif
typedef unaligned_type_ext<int16_t, gdut::endian::little> le_int16_ext_t;
typedef unaligned_type_ext<uint16_t, gdut::endian::little> le_uint16_ext_t;
typedef unaligned_type_ext<int32_t, gdut::endian::little> le_int32_ext_t;
typedef unaligned_type_ext<uint32_t, gdut::endian::little> le_uint32_ext_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type_ext<int64_t, gdut::endian::little> le_int64_ext_t;
typedef unaligned_type_ext<uint64_t, gdut::endian::little> le_uint64_ext_t;
#endif
typedef unaligned_type_ext<float, gdut::endian::little> le_float_ext_t;
typedef unaligned_type_ext<double, gdut::endian::little> le_double_ext_t;
typedef unaligned_type_ext<long double, gdut::endian::little>
    le_long_double_ext_t;

// Big Endian
typedef unaligned_type_ext<char, gdut::endian::big> be_char_ext_t;
typedef unaligned_type_ext<signed char, gdut::endian::big> be_schar_ext_t;
typedef unaligned_type_ext<unsigned char, gdut::endian::big> be_uchar_ext_t;
typedef unaligned_type_ext<short, gdut::endian::big> be_short_ext_t;
typedef unaligned_type_ext<unsigned short, gdut::endian::big> be_ushort_ext_t;
typedef unaligned_type_ext<int, gdut::endian::big> be_int_ext_t;
typedef unaligned_type_ext<unsigned int, gdut::endian::big> be_uint_ext_t;
typedef unaligned_type_ext<long, gdut::endian::big> be_long_ext_t;
typedef unaligned_type_ext<unsigned long, gdut::endian::big> be_ulong_ext_t;
typedef unaligned_type_ext<long long, gdut::endian::big> be_long_long_ext_t;
typedef unaligned_type_ext<unsigned long long, gdut::endian::big>
    be_ulong_long_ext_t;
#if GDUT_USING_8BIT_TYPES
typedef unaligned_type_ext<int8_t, gdut::endian::big> be_int8_ext_t;
typedef unaligned_type_ext<uint8_t, gdut::endian::big> be_uint8_ext_t;
#endif
typedef unaligned_type_ext<int16_t, gdut::endian::big> be_int16_ext_t;
typedef unaligned_type_ext<uint16_t, gdut::endian::big> be_uint16_ext_t;
typedef unaligned_type_ext<int32_t, gdut::endian::big> be_int32_ext_t;
typedef unaligned_type_ext<uint32_t, gdut::endian::big> be_uint32_ext_t;
#if GDUT_USING_64BIT_TYPES
typedef unaligned_type_ext<int64_t, gdut::endian::big> be_int64_ext_t;
typedef unaligned_type_ext<uint64_t, gdut::endian::big> be_uint64_ext_t;
#endif
typedef unaligned_type_ext<float, gdut::endian::big> be_float_ext_t;
typedef unaligned_type_ext<double, gdut::endian::big> be_double_ext_t;
typedef unaligned_type_ext<long double, gdut::endian::big> be_long_double_ext_t;

// Network Order
typedef be_char_ext_t net_char_ext_t;
typedef be_schar_ext_t net_schar_ext_t;
typedef be_uchar_ext_t net_uchar_ext_t;
typedef be_short_ext_t net_short_ext_t;
typedef be_ushort_ext_t net_ushort_ext_t;
typedef be_int_ext_t net_int_ext_t;
typedef be_uint_ext_t net_uint_ext_t;
typedef be_long_ext_t net_long_ext_t;
typedef be_ulong_ext_t net_ulong_ext_t;
typedef be_long_long_ext_t net_long_long_ext_t;
typedef be_ulong_long_ext_t net_ulong_long_ext_t;
#if GDUT_USING_8BIT_TYPES
typedef be_int8_ext_t net_int8_ext_t;
typedef be_uint8_ext_t net_uint8_ext_t;
#endif
typedef be_int16_ext_t net_int16_ext_t;
typedef be_uint16_ext_t net_uint16_ext_t;
typedef be_int32_ext_t net_int32_ext_t;
typedef be_uint32_ext_t net_uint32_ext_t;
#if GDUT_USING_64BIT_TYPES
typedef be_int64_ext_t net_int64_ext_t;
typedef be_uint64_ext_t net_uint64_ext_t;
#endif
typedef be_float_ext_t net_float_ext_t;
typedef be_double_ext_t net_double_ext_t;
typedef be_long_double_ext_t net_long_double_ext_t;

#if GDUT_USING_CPP11
template <typename T, int Endian>
using unaligned_type_ext_t = typename gdut::unaligned_type_ext<T, Endian>::type;
#endif

#if GDUT_USING_CPP17
template <typename T, int Endian>
constexpr size_t unaligned_type_ext_t_v =
    gdut::unaligned_type_ext<T, Endian>::Size;
#endif
} // namespace gdut

#endif
