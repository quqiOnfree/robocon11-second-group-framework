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

#ifndef GDUT_RATIO_INCLUDED
#define GDUT_RATIO_INCLUDED

#include "gcd.hpp"
#include "platform.hpp"
#include "static_assert.hpp"

#include "type_traits.hpp"

#include <stddef.h>
#include <stdint.h>

///\defgroup ratio ratio
///\ingroup maths

namespace gdut {
//***********************************************************************
/// ratio
//***********************************************************************
template <intmax_t Num, intmax_t Den = 1UL> struct ratio {
  GDUT_STATIC_ASSERT(Num != 0, "Numerator cannot be zero");
  GDUT_STATIC_ASSERT(Den != 0, "Denominator cannot be zero");

  static GDUT_CONSTANT intmax_t num = Num / gdut::gcd_const<Num, Den>::value;
  static GDUT_CONSTANT intmax_t den = Den / gdut::gcd_const<Num, Den>::value;

  typedef gdut::ratio<num, den> type;
};

template <intmax_t Num, intmax_t Den>
GDUT_CONSTANT intmax_t ratio<Num, Den>::num;

template <intmax_t Num, intmax_t Den>
GDUT_CONSTANT intmax_t ratio<Num, Den>::den;

#if GDUT_USING_CPP11
//***********************************************************************
/// ratio_add
//***********************************************************************
template <typename TRatio1, typename TRatio2>
using ratio_add =
    gdut::ratio<(TRatio1::num * TRatio2::den) + (TRatio2::num * TRatio1::den),
                TRatio1::den * TRatio2::den>;

//***********************************************************************
/// ratio_subtract
//***********************************************************************
template <typename TRatio1, typename TRatio2>
using ratio_subtract =
    gdut::ratio<(TRatio1::num * TRatio2::den) - (TRatio2::num * TRatio1::den),
                TRatio1::den * TRatio2::den>;

//***********************************************************************
/// ratio_multiply
//***********************************************************************
template <typename TRatio1, typename TRatio2>
using ratio_multiply =
    gdut::ratio<TRatio1::num * TRatio2::num, TRatio1::den * TRatio2::den>;

//***********************************************************************
/// ratio_divide
//***********************************************************************
template <typename TRatio1, typename TRatio2>
using ratio_divide =
    gdut::ratio<TRatio1::num * TRatio2::den, TRatio1::den * TRatio2::num>;
#endif

//***********************************************************************
/// ratio_equal
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_equal : gdut::bool_constant<(TRatio1::num == TRatio2::num) &&
                                         (TRatio1::den == TRatio2::den)> {};

//***********************************************************************
/// ratio_not_equal
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_not_equal
    : gdut::bool_constant<!gdut::ratio_equal<TRatio1, TRatio2>::value> {};

//***********************************************************************
/// ratio_less
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_less : gdut::bool_constant<(TRatio1::num * TRatio2::den) <
                                        (TRatio2::num * TRatio1::den)> {};

//***********************************************************************
/// ratio_less_equal
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_less_equal
    : gdut::bool_constant<!gdut::ratio_less<TRatio2, TRatio1>::value> {};

//***********************************************************************
/// ratio_greater
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_greater
    : gdut::bool_constant<gdut::ratio_less<TRatio2, TRatio1>::value> {};

//***********************************************************************
/// ratio_greater_equal
//***********************************************************************
template <typename TRatio1, typename TRatio2>
struct ratio_greater_equal
    : gdut::bool_constant<!gdut::ratio_less<TRatio1, TRatio2>::value> {};

#if GDUT_USING_CPP17
template <typename R1, typename R2>
inline constexpr bool ratio_equal_v = ratio_equal<R1, R2>::value;

template <typename R1, typename R2>
inline constexpr bool ratio_not_equal_v = ratio_not_equal<R1, R2>::value;

template <typename R1, typename R2>
inline constexpr bool ratio_less_v = ratio_less<R1, R2>::value;

template <typename R1, typename R2>
inline constexpr bool ratio_less_equal_v = ratio_less_equal<R1, R2>::value;

template <typename R1, typename R2>
inline constexpr bool ratio_greater_v = ratio_greater<R1, R2>::value;

template <typename R1, typename R2>
inline constexpr bool ratio_greater_equal_v =
    ratio_greater_equal<R1, R2>::value;
#endif

//***********************************************************************
/// Predefined ration types.
//***********************************************************************
#if INT_MAX > INT32_MAX
typedef ratio<1, 1000000000000000000> atto;
typedef ratio<1, 1000000000000000> femto;
typedef ratio<1, 1000000000000> pico;
#endif

#if (INT_MAX >= INT32_MAX)
typedef ratio<1, 1000000000> nano;
typedef ratio<1, 1000000> micro;
#endif

#if (INT_MAX >= INT16_MAX)
typedef ratio<1, 1000> milli;
typedef ratio<1, 100> centi;
typedef ratio<1, 10> deci;
typedef ratio<10, 1> deca;
typedef ratio<100, 1> hecto;
typedef ratio<1000, 1> kilo;
#endif

#if (INT_MAX >= INT32_MAX)
typedef ratio<1000000, 1> mega;
typedef ratio<1000000000, 1> giga;
#endif

#if INT_MAX > INT32_MAX
typedef ratio<1000000000000, 1> tera;
typedef ratio<1000000000000000, 1> peta;
typedef ratio<1000000000000000000, 1> exa;
#endif

/// An approximation of Pi.
typedef ratio<355, 113> ratio_pi;

/// An approximation of root 2.
typedef ratio<239, 169> ratio_root2;

/// An approximation of 1 over root 2.
typedef ratio<169, 239> ratio_1_over_root2;

/// An approximation of e.
typedef ratio<326, 120> ratio_e;

#if GDUT_USING_CPP11
namespace private_ratio {
// Primary template for GCD calculation
template <typename T, T Value1, T Value2, bool = (Value2 == 0)>
struct ratio_gcd;

// Specialisation for the case when Value2 is not zero
template <typename T, T Value1, T Value2>
struct ratio_gcd<T, Value1, Value2, false> {
  static constexpr T value = ratio_gcd<T, Value2, Value1 % Value2>::value;
};

// Specialisation for the case when Value2 is zero
template <typename T, T Value1, T Value2>
struct ratio_gcd<T, Value1, Value2, true> {
  static constexpr T value = (Value1 < 0) ? -Value1 : Value1;
};

// Primary template for LCM calculation
template <typename T, T Value1, T Value2> struct ratio_lcm {
private:
  static constexpr T product =
      ((Value1 * Value2) < 0) ? -(Value1 * Value2) : Value1 * Value2;

public:
  static constexpr T value = product / ratio_gcd<T, Value1, Value2>::value;
};

template <typename R1> struct ratio_reduce {
private:
  static GDUT_CONSTEXPR11 intmax_t gcd =
      gdut::private_ratio::ratio_gcd<intmax_t, R1::num, R1::den>::value;

public:
  using type = ratio<R1::num / gcd, R1::den / gcd>;
};

template <typename R1, typename R2> struct ratio_add {
private:
  static GDUT_CONSTEXPR11 intmax_t lcm =
      gdut::private_ratio::ratio_lcm<intmax_t, R1::den, R2::den>::value;

public:
  using type = typename ratio_reduce<
      ratio<R1::num * lcm / R1::den + R2::num * lcm / R2::den, lcm>>::type;
};

template <typename R1, typename R2> struct ratio_subtract {
public:
  using type = typename ratio_add<R1, ratio<-R2::num, R2::den>>::type;
};

template <typename R1, typename R2> struct ratio_multiply {
private:
  static GDUT_CONSTEXPR11 intmax_t gcd1 =
      gdut::private_ratio::ratio_gcd<intmax_t, R1::num, R2::den>::value;
  static GDUT_CONSTEXPR11 intmax_t gcd2 =
      gdut::private_ratio::ratio_gcd<intmax_t, R2::num, R1::den>::value;

public:
  using type = ratio<(R1::num / gcd1) * (R2::num / gcd2),
                     (R1::den / gcd2) * (R2::den / gcd1)>;
};

template <typename R1, typename R2> struct ratio_divide {
public:
  using type = typename ratio_multiply<R1, ratio<R2::den, R2::num>>::type;
};
} // namespace private_ratio

#endif
} // namespace gdut

#endif
