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

#ifndef GDUT_FLAGS_INCLUDED
#define GDUT_FLAGS_INCLUDED

#include "algorithm.hpp"
#include "initializer_list.hpp"
#include "integral_limits.hpp"
#include "platform.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace gdut {
//*************************************************************************
/// flags
/// Flags based around an integral value.
//*************************************************************************
template <typename T, T MASK = gdut::integral_limits<T>::max> class flags {
public:
  GDUT_STATIC_ASSERT(gdut::is_integral<T>::value &&gdut::is_unsigned<T>::value,
                     "Unsigned integral values only");

  typedef T value_type;

  static GDUT_CONSTANT value_type ALL_SET =
      gdut::integral_limits<value_type>::max & MASK;
  static GDUT_CONSTANT value_type ALL_CLEAR = 0;

  static GDUT_CONSTANT size_t NBITS = gdut::integral_limits<value_type>::bits;

  //*************************************************************************
  /// Constructor
  //*************************************************************************
  GDUT_CONSTEXPR flags() GDUT_NOEXCEPT : data(value_type(0)) {}

  GDUT_CONSTEXPR flags(value_type pattern) GDUT_NOEXCEPT : data(pattern &MASK) {
  }

  GDUT_CONSTEXPR flags(const flags<T, MASK> &pattern) GDUT_NOEXCEPT
      : data(pattern.value()) {}

  //*************************************************************************
  /// Tests bits.
  //*************************************************************************
  template <value_type pattern> GDUT_CONSTEXPR bool test() const GDUT_NOEXCEPT {
    return (data & pattern) != value_type(0);
  }

  //*******************************************
  GDUT_CONSTEXPR bool test(value_type pattern) const GDUT_NOEXCEPT {
    return (data & pattern) != value_type(0);
  }

  //*************************************************************************
  /// Set the bits.
  //*************************************************************************
  template <value_type pattern, bool value>
  GDUT_CONSTEXPR14 flags<T, MASK> &set() GDUT_NOEXCEPT {
    value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

    return *this;
  }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR14 flags<T, MASK> &set(bool value) GDUT_NOEXCEPT {
    value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

    return *this;
  }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR14 flags<T, MASK> &set() GDUT_NOEXCEPT {
    data |= (pattern & MASK);

    return *this;
  }

  //*******************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &set(value_type pattern) GDUT_NOEXCEPT {
    data |= (pattern & MASK);

    return *this;
  }

  //*******************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &set(value_type pattern,
                                       bool value) GDUT_NOEXCEPT {
    value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

    return *this;
  }

  //*************************************************************************
  /// Clear all of the flags.
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &clear() GDUT_NOEXCEPT {
    data = ALL_CLEAR;

    return *this;
  }

  //*************************************************************************
  /// Reset the bit at the pattern.
  //*************************************************************************
  template <value_type pattern>
  GDUT_CONSTEXPR14 flags<T, MASK> &reset() GDUT_NOEXCEPT {
    data &= ~pattern;

    return *this;
  }

  //*******************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &reset(value_type pattern) GDUT_NOEXCEPT {
    data &= ~pattern;

    return *this;
  }

  //*************************************************************************
  /// Flip bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &flip() GDUT_NOEXCEPT {
    data = (~data & MASK);

    return *this;
  }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR14 flags<T, MASK> &flip() GDUT_NOEXCEPT {
    data ^= pattern & MASK;

    return *this;
  }

  //*******************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &flip(value_type pattern) GDUT_NOEXCEPT {
    data ^= pattern & MASK;

    return *this;
  }

  //*************************************************************************
  // Are all the bits sets?
  //*************************************************************************
  GDUT_CONSTEXPR bool all() const GDUT_NOEXCEPT { return data == MASK; }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR bool all_of() const GDUT_NOEXCEPT {
    return (data & (pattern & MASK)) == (pattern & MASK);
  }

  //*******************************************
  GDUT_CONSTEXPR bool all_of(value_type pattern) const GDUT_NOEXCEPT {
    return (data & (pattern & MASK)) == (pattern & MASK);
  }

  //*************************************************************************
  /// Are none of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR bool none() const GDUT_NOEXCEPT {
    return (data & MASK) == ALL_CLEAR;
  }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR bool none_of() const GDUT_NOEXCEPT {
    return !any_of(pattern);
  }

  //*******************************************
  GDUT_CONSTEXPR bool none_of(value_type pattern) const GDUT_NOEXCEPT {
    return !any_of(pattern);
  }

  //*************************************************************************
  /// Are any of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR bool any() const GDUT_NOEXCEPT {
    return (data & MASK) != value_type(0);
  }

  //*******************************************
  template <value_type pattern>
  GDUT_CONSTEXPR bool any_of() const GDUT_NOEXCEPT {
    return (data & (pattern & MASK)) != value_type(0);
  }

  //*******************************************
  GDUT_CONSTEXPR bool any_of(value_type pattern) const {
    return (data & (pattern & MASK)) != value_type(0);
  }

  //*************************************************************************
  /// Return the value of the flags.
  //*************************************************************************
  GDUT_CONSTEXPR value_type value() const GDUT_NOEXCEPT { return data; }

  //*************************************************************************
  /// Set the value of the flags.
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &value(value_type pattern) GDUT_NOEXCEPT {
    data = pattern & MASK;

    return *this;
  }

  //*************************************************************************
  /// Return the value of the flags.
  //*************************************************************************
  GDUT_CONSTEXPR operator value_type() const GDUT_NOEXCEPT { return data; }

  //*************************************************************************
  /// operator &=
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &
  operator&=(value_type pattern) GDUT_NOEXCEPT {
    data &= pattern;

    return *this;
  }

  //*************************************************************************
  /// operator |=
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &
  operator|=(value_type pattern) GDUT_NOEXCEPT {
    data |= (pattern & MASK);

    return *this;
  }

  //*************************************************************************
  /// operator ^=
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &
  operator^=(value_type pattern) GDUT_NOEXCEPT {
    data ^= (pattern & MASK);

    return *this;
  }

  //*************************************************************************
  /// operator =
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &
  operator=(flags<T, MASK> other) GDUT_NOEXCEPT {
    data = other.data;

    return *this;
  }

  //*************************************************************************
  /// operator =
  //*************************************************************************
  GDUT_CONSTEXPR14 flags<T, MASK> &operator=(value_type pattern) GDUT_NOEXCEPT {
    data = (pattern & MASK);

    return *this;
  }

  //*************************************************************************
  /// swap
  //*************************************************************************
  void swap(flags<T, MASK> &other) GDUT_NOEXCEPT {
    using GDUT_OR_STD::swap;
    swap(data, other.data);
  }

private:
  value_type data;
};

template <typename T, T MASK>
GDUT_CONSTANT typename flags<T, MASK>::value_type flags<T, MASK>::ALL_SET;

template <typename T, T MASK>
GDUT_CONSTANT typename flags<T, MASK>::value_type flags<T, MASK>::ALL_CLEAR;

template <typename T, T MASK> GDUT_CONSTANT size_t flags<T, MASK>::NBITS;

//***************************************************************************
/// operator ==
//***************************************************************************
template <typename T, T MASK>
GDUT_CONSTEXPR bool operator==(flags<T, MASK> lhs,
                               flags<T, MASK> rhs) GDUT_NOEXCEPT {
  return lhs.value() == rhs.value();
}

//***************************************************************************
/// operator !=
//***************************************************************************
template <typename T, T MASK>
GDUT_CONSTEXPR bool operator!=(flags<T, MASK> lhs,
                               flags<T, MASK> rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// swap
//*************************************************************************
template <typename T, T MASK>
void swap(gdut::flags<T, MASK> &lhs, gdut::flags<T, MASK> &rhs) GDUT_NOEXCEPT {
  lhs.swap(rhs);
}
} // namespace gdut

#endif
