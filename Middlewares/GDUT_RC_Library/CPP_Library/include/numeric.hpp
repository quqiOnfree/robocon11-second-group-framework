///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2014 John Wellbelove

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

#ifndef GDUT_NUMERIC_INCLUDED
#define GDUT_NUMERIC_INCLUDED

#include "platform.hpp"
#include "type_traits.hpp"
#include "limits.hpp"
#include "iterator.hpp"

#if GDUT_USING_STL
  #include <iterator>
#endif

///\defgroup numeric numeric
///\ingroup utilities

namespace gdut
{
  //***************************************************************************
  /// iota
  /// Reverse engineered version of std::iota for non C++ 0x11 compilers.
  /// Fills a range of elements with sequentially increasing values starting with <b>value</b>.
  ///\param first An iterator to the first position to fill.
  ///\param last  An iterator to the last + 1 position.
  ///\ingroup numeric
  //***************************************************************************
  template <typename TIterator, typename T>
  GDUT_CONSTEXPR14 void iota(TIterator first, TIterator last, T value)
  {
    while (first != last)
    {
      *first++ = value++;
    }
  }

  //***************************************************************************
  /// midpoint
  /// For floating point.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 typename gdut::enable_if<!gdut::is_pointer<T>::value &&
                                          !gdut::is_integral<T>::value &&
                                          gdut::is_floating_point<T>::value, T>::type
    midpoint(T a, T b) GDUT_NOEXCEPT
  {
    T lo = gdut::numeric_limits<T>::min() * T(2);
    T hi = gdut::numeric_limits<T>::max() * T(2);

    return ((abs(a) <= hi) && (abs(b) <= hi)) ?
              (a + b) / T(2) :
              (abs(a) < lo) ?
                a + (b / T(2)) :
                (abs(b) < lo) ?
                  ((a / T(2)) + b) :
                  (a / T(2)) + (b / T(2));
  }

  //***************************************************************************
  /// midpoint
  /// For unsigned integrals.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 typename gdut::enable_if<!gdut::is_pointer<T>::value &&
                                          gdut::is_integral<T>::value &&
                                          !gdut::is_floating_point<T>::value &&
                                          gdut::is_unsigned<T>::value, T>::type
    midpoint(T a, T b) GDUT_NOEXCEPT
  {
    if (a > b)
    {
      return a - ((a - b) >> 1);
    }
    else
    {
      return a + ((b - a) >> 1);
    }
  }

  //***************************************************************************
  /// midpoint
  /// For signed integrals.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 typename gdut::enable_if<!gdut::is_pointer<T>::value&&
                                          gdut::is_integral<T>::value &&
                                          !gdut::is_floating_point<T>::value&&
                                          gdut::is_signed<T>::value, T>::type
    midpoint(T a, T b) GDUT_NOEXCEPT
  {
    typedef typename gdut::make_unsigned<T>::type utype;

    if (a > b)
    {
      return a - T(utype(utype(a) - utype(b)) >> 1);
    }
    else
    {
      return a + T((utype(b) - utype(a)) >> 1);
    }
  }

  //***************************************************************************
  /// midpoint
  /// For pointers.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 typename gdut::enable_if<gdut::is_pointer<T>::value&&
                                          !gdut::is_integral<T>::value &&
                                          !gdut::is_floating_point<T>::value, T>::type
    midpoint(T a, T b) GDUT_NOEXCEPT
  {
    if (a > b)
    {
      return b + (gdut::distance(b, a) / 2U);
    }
    else
    {
      return a + (gdut::distance(a, b) / 2U);
    }
  }

  //***************************************************************************
  /// midpoint
  /// For ETL random access iterators.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 T midpoint(T a, T b, typename gdut::enable_if<!gdut::is_pointer<T>::value &&
    !gdut::is_integral<T>::value &&
    !gdut::is_floating_point<T>::value &&
    gdut::is_same<typename gdut::iterator_traits<T>::iterator_category, GDUT_OR_STD::random_access_iterator_tag>::value , int>::type = 0)
  {
    if (a > b)
    {
      return b + (gdut::distance(b, a) / 2U);
    }
    else
    {
      return a + (gdut::distance(a, b) / 2U);
    }
  }

  //***************************************************************************
  /// midpoint
  /// For ETL forward and bidirectional iterators.
  /// Parameter 'a' must be before 'b', otherwise the result is undefined.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 T midpoint(T a, T b, typename gdut::enable_if<(!gdut::is_pointer<T>::value &&
                                                                !gdut::is_integral<T>::value &&
                                                                !gdut::is_floating_point<T>::value &&
                                                                (gdut::is_same<typename gdut::iterator_traits<T>::iterator_category, GDUT_OR_STD::forward_iterator_tag>::value ||
                                                                 gdut::is_same<typename gdut::iterator_traits<T>::iterator_category, GDUT_OR_STD::bidirectional_iterator_tag>::value)),  int>::type = 0)
  {
    gdut::advance(a, gdut::distance(a, b) / 2U);
    return a;
  }

  //***************************************************************************
  /// Linear interpolation
  /// For floating point.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR typename gdut::enable_if<gdut::is_floating_point<T>::value, T>::type
    lerp(T a, T b, T t) GDUT_NOEXCEPT
  {
    return a + (t * (b - a));
  }

  //***************************************************************************
  /// Linear interpolation
  /// For when any parameter is not floating point.
  //***************************************************************************
  template <typename TArithmetic1, typename TArithmetic2, typename TArithmetic3>
  GDUT_CONSTEXPR typename gdut::enable_if<!gdut::is_floating_point<TArithmetic1>::value ||
                                        !gdut::is_floating_point<TArithmetic2>::value ||
                                        !gdut::is_floating_point<TArithmetic3>::value, typename gdut::conditional<gdut::is_same<TArithmetic1, long double>::value ||
                                                                                                                gdut::is_same<TArithmetic2, long double>::value ||
                                                                                                                gdut::is_same<TArithmetic3, long double>::value, long double, double>::type>::type
    lerp(TArithmetic1 a, TArithmetic2 b, TArithmetic3 t) GDUT_NOEXCEPT
  {
    typedef typename gdut::conditional<gdut::is_integral<TArithmetic1>::value, double, TArithmetic1>::type typecast_a;
    typedef typename gdut::conditional<gdut::is_integral<TArithmetic2>::value, double, TArithmetic2>::type typecast_b;
    typedef typename gdut::conditional<gdut::is_integral<TArithmetic3>::value, double, TArithmetic3>::type typecast_t;

    return typecast_a(a) + (typecast_t(t) * (typecast_b(b) - typecast_a(a)));
  }
}

#endif
