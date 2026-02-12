///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2023 John Wellbelove

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

#ifndef GDUT_IN_CHRONO_H
#error DO NOT DIRECTLY INCLUDE THIS FILE. USE CHRONO.H
#endif

#include "../../limits.hpp"
#include "../../ratio.hpp"
#include "../../static_assert.hpp"
#include "../../type_traits.hpp"

#include <string.h>

namespace gdut {
namespace chrono {
namespace private_chrono {
// Helper to find the greatest common divisor
template <intmax_t Value1, intmax_t Value2> struct gcd {
  static GDUT_CONSTANT intmax_t value = gcd<Value2, Value1 % Value2>::value;
};

template <intmax_t Value1> struct gcd<Value1, 0> {
  static GDUT_CONSTANT intmax_t value = Value1;
};

// Helper to find the least common multiple
template <intmax_t Value1, intmax_t Value2> struct lcm {
  static GDUT_CONSTANT intmax_t value =
      (Value1 / gcd<Value1, Value2>::value) * Value2;
};
} // namespace private_chrono

//***********************************************************************
/// duration_values
//***********************************************************************
template <typename TRep> struct duration_values {
  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR TRep zero() GDUT_NOEXCEPT { return TRep(0); }

  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 TRep min() GDUT_NOEXCEPT {
    return gdut::numeric_limits<TRep>::min();
  }

  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 TRep max() GDUT_NOEXCEPT {
    return gdut::numeric_limits<TRep>::max();
  }
};

template <typename TRep, typename TPeriod> class duration;

template <typename TToDuration, typename TRep, typename TPeriod>
GDUT_CONSTEXPR14 TToDuration
duration_cast(const gdut::chrono::duration<TRep, TPeriod> &d) GDUT_NOEXCEPT;

//***********************************************************************
/// duration
//***********************************************************************
template <typename TRep, typename TPeriod = gdut::ratio<1>> class duration {
public:
  using rep = TRep;
  using period = typename TPeriod::type;

  //***********************************************************************
  GDUT_CONSTEXPR duration() GDUT_NOEXCEPT : value(0) {}

  //***********************************************************************
  GDUT_CONSTEXPR14
  duration(const gdut::chrono::duration<TRep, TPeriod> &other) GDUT_NOEXCEPT
      : value(other.value) {}

  //***********************************************************************
  template <typename TRep2>
  GDUT_CONSTEXPR14 explicit duration(const TRep2 &value_) GDUT_NOEXCEPT
      : value(static_cast<TRep>(value_)) {}

  //***********************************************************************
  template <typename TRep2, typename TPeriod2,
            typename gdut::enable_if<
                gdut::ratio_divide<TPeriod2, TPeriod>::den == 1, int>::type = 0>
  GDUT_CONSTEXPR14
  duration(const gdut::chrono::duration<TRep2, TPeriod2> &other) GDUT_NOEXCEPT
      : value(
            gdut::chrono::duration_cast<gdut::chrono::duration<TRep, TPeriod>>(
                other)
                .count()) {
    GDUT_STATIC_ASSERT(
        !(gdut::is_integral<TRep>::value &&
          gdut::is_floating_point<TRep2>::value),
        "Cannot convert duration from floating point to integral");
  }

  //***********************************************************************
  GDUT_CONSTEXPR14
  gdut::chrono::duration<TRep, TPeriod>
  operator=(const gdut::chrono::duration<TRep, TPeriod> &other) GDUT_NOEXCEPT {
    value = other.count();

    return *this;
  }

  //***********************************************************************
  template <typename TRep2, typename TPeriod2>
  GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod> operator=(
      const gdut::chrono::duration<TRep2, TPeriod2> &other) GDUT_NOEXCEPT {
    value = gdut::chrono::duration_cast<gdut::chrono::duration<TRep, TPeriod>>(
                other)
                .count();

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 TRep count() const GDUT_NOEXCEPT { return value; }

  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::common_type_t<duration>
  operator+() const GDUT_NOEXCEPT {
    return gdut::common_type_t<duration>(*this);
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::common_type_t<duration>
  operator-() const GDUT_NOEXCEPT {
    return gdut::common_type_t<duration>(-value);
  }

  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod>
  zero() GDUT_NOEXCEPT {
    return gdut::chrono::duration<TRep, TPeriod>(
        gdut::chrono::duration_values<TRep>::zero());
  }

  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod>
  min() GDUT_NOEXCEPT {
    return gdut::chrono::duration<TRep, TPeriod>(
        gdut::chrono::duration_values<TRep>::min());
  }

  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod>
  max() GDUT_NOEXCEPT {
    return gdut::chrono::duration<TRep, TPeriod>(
        gdut::chrono::duration_values<TRep>::max());
  }

  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod>
  absolute() const GDUT_NOEXCEPT {
    return gdut::chrono::duration<TRep, TPeriod>(value < 0 ? -value : value);
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &operator++() GDUT_NOEXCEPT {
    ++value;

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration operator++(int) GDUT_NOEXCEPT {
    duration temp(*this);
    ++value;

    return temp;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &operator--() GDUT_NOEXCEPT {
    --value;

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration operator--(int) GDUT_NOEXCEPT {
    duration temp(*this);
    --value;

    return temp;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &
  operator+=(const duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
    value += d.count();

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &
  operator-=(const duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
    value -= d.count();

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &operator*=(const TRep &r) GDUT_NOEXCEPT {
    value *= r;

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &operator/=(const TRep &r) GDUT_NOEXCEPT {
    value /= r;

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &operator%=(const TRep &r) GDUT_NOEXCEPT {
    value %= r;

    return *this;
  }

  //***********************************************************************
  GDUT_CONSTEXPR14 duration &
  operator%=(const duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
    value %= d.count();

    return *this;
  }

  //***********************************************************************
  /// Compare duration with another.
  /// if duration < other, returns -1
  /// else if duration > other, returns 1
  /// else returns 0
  //***********************************************************************
  template <typename TRep2, typename TPeriod2>
  GDUT_CONSTEXPR14 int
  compare(const duration<TRep2, TPeriod2> &other) const GDUT_NOEXCEPT {
    // Determine the common type of the two durations.
    using common_duration = typename gdut::common_type<
        gdut::chrono::duration<TRep, TPeriod>,
        gdut::chrono::duration<TRep2, TPeriod2>>::type;

    common_duration lhs_converted =
        gdut::chrono::duration_cast<common_duration>(*this);
    common_duration rhs_converted =
        gdut::chrono::duration_cast<common_duration>(other);

    if (lhs_converted.count() < rhs_converted.count())
      return -1;
    if (lhs_converted.count() > rhs_converted.count())
      return 1;

    return 0;
  }

private:
  TRep value;
};

//***********************************************************************
/// Duration types
//***********************************************************************
#if (GDUT_USING_64BIT_TYPES)
using nanoseconds = gdut::chrono::duration<int64_t, gdut::nano>;
using microseconds = gdut::chrono::duration<int64_t, gdut::micro>;
using milliseconds = gdut::chrono::duration<int64_t, gdut::milli>;
using seconds = gdut::chrono::duration<int64_t, gdut::ratio<1U>>;
#else
using nanoseconds = gdut::chrono::duration<int32_t, gdut::nano>;
using microseconds = gdut::chrono::duration<int32_t, gdut::micro>;
using milliseconds = gdut::chrono::duration<int32_t, gdut::milli>;
using seconds = gdut::chrono::duration<int32_t, gdut::ratio<1U>>;
#endif
using minutes = gdut::chrono::duration<int32_t, gdut::ratio<60U>>;
using hours = gdut::chrono::duration<int32_t, gdut::ratio<3600U>>;
using days = gdut::chrono::duration<int32_t, gdut::ratio<86400U>>;
using weeks = gdut::chrono::duration<int32_t, gdut::ratio<604800U>>;
using months = gdut::chrono::duration<int32_t, gdut::ratio<2629746U>>;
using years = gdut::chrono::duration<int32_t, gdut::ratio<31556952U>>;

//***********************************************************************
/// duration_cast
//***********************************************************************
template <typename TToDuration, typename TRep, typename TPeriod>
GDUT_CONSTEXPR14 TToDuration
duration_cast(const gdut::chrono::duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
  using from_rep = TRep;
  using from_period = TPeriod;

  using to_rep = typename TToDuration::rep;
  using to_period = typename TToDuration::period;

  using ratio_divide_t = typename gdut::ratio_divide<from_period, to_period>;
  using common_t = typename gdut::common_type<from_rep, to_rep, intmax_t>::type;

  common_t ct_count = static_cast<common_t>(d.count());
  common_t ct_num = static_cast<common_t>(ratio_divide_t::type::num);
  common_t ct_den = static_cast<common_t>(ratio_divide_t::type::den);

  if GDUT_IF_CONSTEXPR ((from_period::num == to_period::num) &&
                        (from_period::den == to_period::den)) {
    return TToDuration(static_cast<to_rep>(d.count()));
  } else if GDUT_IF_CONSTEXPR (ratio_divide_t::num == 1) {
    return TToDuration(static_cast<to_rep>(ct_count / ct_den));
  } else if GDUT_IF_CONSTEXPR (ratio_divide_t::den == 1) {
    return TToDuration(static_cast<to_rep>(ct_count * ct_num));
  } else {
    return TToDuration(static_cast<to_rep>((ct_count * ct_num) / ct_den));
  }
}
} // namespace chrono

//*************************************************************************
/// Hash function for gdut::chrono::duration
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <typename TRep, typename TPeriod>
struct hash<gdut::chrono::duration<TRep, TPeriod>> {
  GDUT_CONSTEXPR14 size_t operator()(
      const gdut::chrono::duration<TRep, TPeriod> &d) const GDUT_NOEXCEPT {
    uint8_t buffer[sizeof(TRep) + sizeof(intmax_t) + sizeof(intmax_t)];

    TRep value = d.count();
    intmax_t num = TPeriod::num;
    intmax_t den = TPeriod::den;

    memcpy(buffer, &value, sizeof(TRep));
    memcpy(buffer + sizeof(TRep), &num, sizeof(intmax_t));
    memcpy(buffer + sizeof(TRep) + sizeof(intmax_t), &den, sizeof(intmax_t));

    return gdut::private_hash::generic_hash<size_t>(
        buffer, buffer + sizeof(TRep) + sizeof(intmax_t) + sizeof(intmax_t));
  }
};
#endif

//***********************************************************************
/// Find the common type of two duration types.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
struct common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                   gdut::chrono::duration<TRep2, TPeriod2>> {
private:
  using value_type = typename gdut::common_type<TRep1, TRep2>::type;
  using period_type = gdut::ratio<
      gdut::chrono::private_chrono::gcd<TPeriod1::num, TPeriod2::num>::value,
      gdut::chrono::private_chrono::lcm<TPeriod1::den, TPeriod2::den>::value>;

public:
  using type = gdut::chrono::duration<value_type, period_type>;
};

//***********************************************************************
/// Check equality.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
           const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  using common_t =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  common_t l = gdut::chrono::duration_cast<common_t>(lhs);
  common_t r = gdut::chrono::duration_cast<common_t>(rhs);

  return l.count() == r.count();
}

//***********************************************************************
/// Check inequality.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
           const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***********************************************************************
/// Less-than.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator<(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
          const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  using common_t =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  common_t l = gdut::chrono::duration_cast<common_t>(lhs);
  common_t r = gdut::chrono::duration_cast<common_t>(rhs);

  return l.count() < r.count();
}

//***********************************************************************
/// Less-than-or-equal.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator<=(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
           const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  return !(rhs < lhs);
}

//***********************************************************************
/// Greater-than.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator>(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
          const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  return rhs < lhs;
}

//***********************************************************************
/// Greater-than-or-equal.
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 bool
operator>=(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
           const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  return !(lhs < rhs);
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
[[nodiscard]] constexpr auto
operator<=>(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
            const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  using common_t =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  common_t l = gdut::chrono::duration_cast<common_t>(lhs);
  common_t r = gdut::chrono::duration_cast<common_t>(rhs);

  return (l.count() <=> r.count());
}
#endif

//***********************************************************************
/// Operator +
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14
    typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                               gdut::chrono::duration<TRep2, TPeriod2>>::type
    operator+(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
              const gdut::chrono::duration<TRep2, TPeriod2> &rhs)
        GDUT_NOEXCEPT {
  // Determine the common type of the two durations.
  using common_duration =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  // Convert both durations to the common type.
  common_duration lhs_converted =
      gdut::chrono::duration_cast<common_duration>(lhs);
  common_duration rhs_converted =
      gdut::chrono::duration_cast<common_duration>(rhs);

  // Return the sum of the two converted durations.
  return common_duration(lhs_converted.count() + rhs_converted.count());
}

//***********************************************************************
/// Operator -
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14
    typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                               gdut::chrono::duration<TRep2, TPeriod2>>::type
    operator-(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
              const gdut::chrono::duration<TRep2, TPeriod2> &rhs)
        GDUT_NOEXCEPT {
  // Determine the common type of the two durations.
  using common_duration =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  // Convert both durations to the common type.
  common_duration lhs_converted =
      gdut::chrono::duration_cast<common_duration>(lhs);
  common_duration rhs_converted =
      gdut::chrono::duration_cast<common_duration>(rhs);

  // Return the difference of the two converted durations.
  return common_duration(lhs_converted.count() - rhs_converted.count());
}

//***********************************************************************
/// Operator *
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2>
GDUT_CONSTEXPR14 typename enable_if<
    !gdut::is_specialization<TRep2, gdut::chrono::duration>::value,
    gdut::chrono::duration<typename gdut::common_type<TRep1, TRep2>::type,
                           TPeriod1>>::type
operator*(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
          const TRep2 &rhs) GDUT_NOEXCEPT {
  using common_rep = typename gdut::common_type<TRep1, TRep2>::type;
  using result_duration = gdut::chrono::duration<common_rep, TPeriod1>;

  // Multiply the count of the duration by the scalar value
  return result_duration(static_cast<common_rep>(lhs.count()) *
                         static_cast<common_rep>(rhs));
}

//***********************************************************************
/// Operator *
//***********************************************************************
template <typename TRep1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 gdut::chrono::duration<
    typename gdut::common_type<TRep1, TRep2>::type, TPeriod2>
operator*(const TRep1 &lhs,
          const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  using common_rep = typename gdut::common_type<TRep1, TRep2>::type;
  using result_duration = gdut::chrono::duration<common_rep, TPeriod2>;

  // Multiply the count of the duration by the scalar value
  return result_duration(static_cast<common_rep>(rhs.count()) *
                         static_cast<common_rep>(lhs));
}

//***********************************************************************
/// Operator /
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2>
GDUT_CONSTEXPR14 typename enable_if<
    !gdut::is_specialization<TRep2, gdut::chrono::duration>::value,
    gdut::chrono::duration<typename gdut::common_type<TRep1, TRep2>::type,
                           TPeriod1>>::type
operator/(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
          const TRep2 &rhs) GDUT_NOEXCEPT {
  using common_rep = typename gdut::common_type<TRep1, TRep2>::type;
  using result_duration = gdut::chrono::duration<common_rep, TPeriod1>;

  // Divide the count of the duration by the scalar value
  return result_duration(static_cast<common_rep>(lhs.count()) /
                         static_cast<common_rep>(rhs));
}

//***********************************************************************
/// Operator /
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14 typename gdut::common_type<TRep1, TRep2>::type
operator/(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
          const gdut::chrono::duration<TRep2, TPeriod2> &rhs) GDUT_NOEXCEPT {
  // Determine the common type of the two durations.
  using common_duration =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  common_duration lhs_converted =
      gdut::chrono::duration_cast<common_duration>(lhs);
  common_duration rhs_converted =
      gdut::chrono::duration_cast<common_duration>(rhs);

  return typename gdut::common_type<TRep1, TRep2>::type(lhs_converted.count() /
                                                        rhs_converted.count());
}

//***********************************************************************
/// Operator %
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2>
GDUT_CONSTEXPR14
    gdut::chrono::duration<typename gdut::common_type<TRep1, TRep2>::type,
                           TPeriod1>
    operator%(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
              const TRep2 &rhs) GDUT_NOEXCEPT {
  using common_rep = typename gdut::common_type<TRep1, TRep2>::type;
  using common_dur = gdut::chrono::duration<common_rep, TPeriod1>;

  // Mod the count of the duration by the scalar value
  return common_dur(static_cast<common_rep>(lhs.count()) % rhs);
}

//***********************************************************************
/// Operator %
//***********************************************************************
template <typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
GDUT_CONSTEXPR14
    typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                               gdut::chrono::duration<TRep2, TPeriod2>>::type
    operator%(const gdut::chrono::duration<TRep1, TPeriod1> &lhs,
              const gdut::chrono::duration<TRep2, TPeriod2> &rhs)
        GDUT_NOEXCEPT {
  // Determine the common type of the two durations.
  using common_duration =
      typename gdut::common_type<gdut::chrono::duration<TRep1, TPeriod1>,
                                 gdut::chrono::duration<TRep2, TPeriod2>>::type;

  common_duration lhs_converted =
      gdut::chrono::duration_cast<common_duration>(lhs);
  common_duration rhs_converted =
      gdut::chrono::duration_cast<common_duration>(rhs);

  return common_duration(lhs_converted.count() % rhs_converted.count());
}

//***********************************************************************
/// Rounds down a duration to the nearest lower precision.
//***********************************************************************
template <typename TToDuration, typename TRep, typename TPeriod>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    gdut::is_specialization<TToDuration, gdut::chrono::duration>::value,
    TToDuration>::type
floor(const gdut::chrono::duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
  TToDuration result = gdut::chrono::duration_cast<TToDuration>(d);

  if (result > d) {
    --result;
  }

  return result;
}

//***********************************************************************
/// Rounds up a duration to the nearest higher precision.
//***********************************************************************
template <typename TToDuration, typename TRep, typename TPeriod>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    gdut::is_specialization<TToDuration, gdut::chrono::duration>::value,
    TToDuration>::type
ceil(const gdut::chrono::duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
  TToDuration result = gdut::chrono::duration_cast<TToDuration>(d);

  if (result < d) {
    ++result;
  }

  return result;
}

//***********************************************************************
/// Rounds a duration to the nearest precision.
/// If the duration is exactly halfway, it rounds away from zero.
//***********************************************************************
template <typename TToDuration, typename TRep, typename TPeriod>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    gdut::is_specialization<TToDuration, gdut::chrono::duration>::value,
    TToDuration>::type
round(const gdut::chrono::duration<TRep, TPeriod> &d) GDUT_NOEXCEPT {
  // Convert the input duration to the target duration type
  TToDuration lower = floor<TToDuration>(d);
  TToDuration upper = ceil<TToDuration>(lower + TToDuration(1));

  auto lower_diff = d - lower;
  auto upper_diff = upper - d;

  if ((lower_diff < upper_diff) ||
      ((lower_diff == upper_diff) && gdut::is_even(lower.count()))) {
    return lower;
  } else {
    return upper;
  }
}

//***********************************************************************
/// Returns the absolute value of a duration.
//***********************************************************************
template <class TRep, class TPeriod,
          typename = gdut::enable_if_t<gdut::numeric_limits<TRep>::is_signed>>
GDUT_CONSTEXPR14 gdut::chrono::duration<TRep, TPeriod>
abs(gdut::chrono::duration<TRep, TPeriod> d) GDUT_NOEXCEPT {
  return d.count() >= 0 ? +d : -d;
}
} // namespace gdut

#if GDUT_HAS_CHRONO_LITERALS_DURATION
namespace gdut {
inline namespace literals {
inline namespace chrono_literals {
//***********************************************************************
/// Literal for hours duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::hours
operator""_hours(unsigned long long h) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::hours
operator""_h(unsigned long long h) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::hours(static_cast<gdut::chrono::hours::rep>(h));
}

//***********************************************************************
/// Literal for floating point hours duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, ratio<3600>>
operator""_hours(long double h) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, ratio<3600>>
operator""_h(long double h) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double, ratio<3600>>(h);
}

//***********************************************************************
/// Literal for minutes duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::minutes
operator""_minutes(unsigned long long m) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::minutes
operator""_min(unsigned long long m) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::minutes(static_cast<gdut::chrono::minutes::rep>(m));
}

//***********************************************************************
/// Literal for floating point minutes duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, ratio<60>>
operator""_minutes(long double m) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, ratio<60>>
operator""_min(long double m) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double, ratio<60>>(m);
}

//***********************************************************************
/// Literal for seconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::seconds
operator""_seconds(unsigned long long s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::seconds
operator""_s(unsigned long long s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::seconds(static_cast<gdut::chrono::seconds::rep>(s));
}

//***********************************************************************
/// Literal for floating point seconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double>
operator""_seconds(long double s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double>
operator""_s(long double s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double>(s);
}

//***********************************************************************
/// Literal for milliseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::milliseconds
operator""_milliseconds(unsigned long long s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::milliseconds
operator""_ms(unsigned long long s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::milliseconds(
      static_cast<gdut::chrono::milliseconds::rep>(s));
}

//***********************************************************************
/// Literal for floating point milliseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, milli>
operator""_milliseconds(long double s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, milli>
operator""_ms(long double s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double, milli>(s);
}

//***********************************************************************
/// Literal for microseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::microseconds
operator""_microseconds(unsigned long long s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::microseconds
operator""_us(unsigned long long s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::microseconds(
      static_cast<gdut::chrono::microseconds::rep>(s));
}

//***********************************************************************
/// Literal for floating point microseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, micro>
operator""_microseconds(long double s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, micro>
operator""_us(long double s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double, micro>(s);
}

//***********************************************************************
/// Literal for nanoseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::nanoseconds
operator""_nanoseconds(unsigned long long s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::nanoseconds
operator""_ns(unsigned long long s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::nanoseconds(
      static_cast<gdut::chrono::nanoseconds::rep>(s));
}

//***********************************************************************
/// Literal for floating point microseconds duration
//***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, nano>
operator""_nanoseconds(long double s) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::duration<double, nano>
operator""_ns(long double s) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::duration<double, nano>(s);
}
} // namespace chrono_literals
} // namespace literals
} // namespace gdut
#endif
