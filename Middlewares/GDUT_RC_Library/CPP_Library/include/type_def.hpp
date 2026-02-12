///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2016 John Wellbelove

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

#ifndef GDUT_TYPE_DEF_INCLUDED
#define GDUT_TYPE_DEF_INCLUDED

#include "platform.hpp"
#include "type_traits.hpp"

namespace gdut {
#define GDUT_TYPEDEF(T, name)                                                  \
  class name##_tag;                                                            \
  typedef gdut::type_def<name##_tag, T> name
#define GDUT_USING(name, T)                                                    \
  class name##_tag;                                                            \
  typedef gdut::type_def<name##_tag, T> name

//*************************************************************************
/// A template type to define strong typedefs.
/// Usage:
///\code
/// // Short form.
/// GDUT_TYPEDEF(int, mytype);
/// or
/// GDUT_USING(mytype, int);
///
/// // Long form.
/// class mytype_t_tag;
/// typedef gdut::type_def<mytype_t_tag, int> mytype;
///\endcode
//*************************************************************************
template <typename TIdType, typename TValue> class type_def {
public:
  typedef TValue type;
  typedef TValue value_type;
  typedef TIdType id_type;

  //*********************************************************************
  GDUT_CONSTEXPR type_def() GDUT_NOEXCEPT : value(TValue()) {}

  //*********************************************************************
#if GDUT_USING_CPP11
  template <typename T, typename = typename gdut::enable_if<
                            gdut::is_convertible<T, TValue>::value, void>::type>
#else
  template <typename T>
#endif
  GDUT_CONSTEXPR type_def(T value_) GDUT_NOEXCEPT : value(value_) {
  }

  //*********************************************************************
#if GDUT_USING_CPP11
  GDUT_CONSTEXPR type_def(const type_def &other) = default;
#endif

  //*********************************************************************
  GDUT_CONSTEXPR operator TValue() const GDUT_NOEXCEPT { return value; }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator++() GDUT_NOEXCEPT {
    ++value;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def operator++(int) GDUT_NOEXCEPT {
    type_def temp(*this);
    type_def::operator++();
    return temp;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator--() GDUT_NOEXCEPT {
    --value;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def operator--(int) GDUT_NOEXCEPT {
    type_def temp(*this);
    type_def::operator--();
    return temp;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator+=(T rhs) GDUT_NOEXCEPT {
    value += rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14
  type_def &operator+=(const type_def &rhs) GDUT_NOEXCEPT {
    value += rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator-=(T rhs) GDUT_NOEXCEPT {
    value -= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator-=(const type_def &rhs) GDUT_NOEXCEPT {
    value -= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator*=(T rhs) GDUT_NOEXCEPT {
    value *= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator*=(const type_def &rhs) GDUT_NOEXCEPT {
    value *= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator/=(T rhs) GDUT_NOEXCEPT {
    value /= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator/=(const type_def &rhs) GDUT_NOEXCEPT {
    value /= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator%=(T rhs) GDUT_NOEXCEPT {
    value %= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator%=(const type_def &rhs) GDUT_NOEXCEPT {
    value %= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator&=(T rhs) GDUT_NOEXCEPT {
    value &= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator&=(const type_def &rhs) GDUT_NOEXCEPT {
    value &= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator|=(T rhs) GDUT_NOEXCEPT {
    value |= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator|=(const type_def &rhs) GDUT_NOEXCEPT {
    value |= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def &>::type
#else
      type_def &
#endif
      operator^=(T rhs) GDUT_NOEXCEPT {
    value ^= rhs;
    return *this;
  }

  //*********************************************************************
  GDUT_CONSTEXPR14 type_def &operator^=(const type_def &rhs) GDUT_NOEXCEPT {
    value ^= rhs.value;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_integral<T>::value, type_def &>::type
      operator<<=(T rhs) GDUT_NOEXCEPT {
    value <<= rhs;
    return *this;
  }

  //*********************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_integral<T>::value, type_def &>::type
      operator>>=(T rhs) GDUT_NOEXCEPT {
    value >>= rhs;
    return *this;
  }

  //*********************************************************************
#if GDUT_USING_CPP11
  GDUT_CONSTEXPR14 type_def &operator=(const type_def &rhs) = default;
#endif

  //*********************************************************************
  TValue &get() GDUT_NOEXCEPT { return value; }

  //*********************************************************************
  GDUT_CONSTEXPR const TValue &get() const GDUT_NOEXCEPT { return value; }

  //*********************************************************************
  // + operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator+(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value + rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR type_def operator+(T lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs + rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator+(const type_def &lhs,
                                           const type_def &rhs) {
    return type_def(lhs.value + rhs.value);
  }

  //*********************************************************************
  // - operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator-(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value - rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator-(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs - rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator-(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value - rhs.value);
  }

  //*********************************************************************
  // * operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator*(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value * rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator*(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs * rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator*(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value * rhs.value);
  }

  //*********************************************************************
  // / operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator/(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value / rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator/(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs / rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator/(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value / rhs.value);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator%(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value % rhs);
  }

  //*********************************************************************
  // % operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator%(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs % rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator%(const type_def &lhs,
                                           const type_def &rhs) {
    return type_def(lhs.value % rhs.value);
  }

  //*********************************************************************
  // & operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator&(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value & rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator&(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs & rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator&(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value & rhs.value);
  }

  //*********************************************************************
  // | operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator|(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value | rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator|(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs | rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator|(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value | rhs.value);
  }

  //*********************************************************************
  // ^ operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator^(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value ^ rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               type_def>::type
#else
      type_def
#endif
      operator^(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs ^ rhs.value);
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR type_def operator^(const type_def &lhs,
                                           const type_def &rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value ^ rhs.value);
  }

  //*********************************************************************
  // << operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
      typename gdut::enable_if<gdut::is_integral<T>::value, type_def>::type
      operator<<(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value << rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
      typename gdut::enable_if<(gdut::is_integral<T>::value &&
                                gdut::is_integral<TValue>::value),
                               T>::type
      operator<<(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs << rhs.value;
  }

  //*********************************************************************
  // >> operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
      typename gdut::enable_if<gdut::is_integral<T>::value, type_def>::type
      operator>>(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return type_def(lhs.value >> rhs);
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
      typename gdut::enable_if<(gdut::is_integral<T>::value &&
                                gdut::is_integral<TValue>::value),
                               T>::type
      operator>>(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs >> rhs.value;
  }

  //*********************************************************************
  // < operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator<(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value < rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator<(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs < rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator<(const type_def &lhs,
                                       const type_def &rhs) GDUT_NOEXCEPT {
    return lhs.value < rhs.value;
  }

  //*********************************************************************
  // <= operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator<=(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value <= rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator<=(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs <= rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator<=(const type_def &lhs,
                                        const type_def &rhs) GDUT_NOEXCEPT {
    return lhs.value <= rhs.value;
  }

  //*********************************************************************
  // > operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator>(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value > rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator>(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs > rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator>(const type_def &lhs,
                                       const type_def &rhs) GDUT_NOEXCEPT {
    return lhs.value > rhs.value;
  }

  //*********************************************************************
  // >= operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator>=(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value >= rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator>=(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs >= rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator>=(const type_def &lhs,
                                        const type_def &rhs) {
    return lhs.value >= rhs.value;
  }

  //*********************************************************************
  // == operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator==(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value == rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator==(T lhs, const type_def &rhs) {
    return lhs == rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator==(const type_def &lhs,
                                        const type_def &rhs) GDUT_NOEXCEPT {
    return lhs.value == rhs.value;
  }

  //*********************************************************************
  // != operator
  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator!=(const type_def &lhs, T rhs) GDUT_NOEXCEPT {
    return lhs.value != rhs;
  }

  //*********************************************************************
  template <typename T>
  friend GDUT_CONSTEXPR
#if GDUT_USING_CPP11
      typename gdut::enable_if<gdut::is_convertible<T, TValue>::value,
                               bool>::type
#else
      bool
#endif
      operator!=(T lhs, const type_def &rhs) GDUT_NOEXCEPT {
    return lhs != rhs.value;
  }

  //*********************************************************************
  friend GDUT_CONSTEXPR bool operator!=(const type_def &lhs,
                                        const type_def &rhs) GDUT_NOEXCEPT {
    return lhs.value != rhs.value;
  }

private:
  TValue value;
};
} // namespace gdut

#endif
