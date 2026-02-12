///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2025 John Wellbelove

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

#include "../../absolute.hpp"
#include "../../power.hpp"

namespace gdut {
//***********************************************************************
/// absolute
/// Enabled for gdut::chrono::duration
//***********************************************************************
template <typename TDuration>
GDUT_NODISCARD GDUT_CONSTEXPR14 typename gdut::enable_if<
    gdut::is_specialization<TDuration, gdut::chrono::duration>::value,
    TDuration>::type
absolute(TDuration dur) GDUT_NOEXCEPT {
  return TDuration((dur.count() < 0) ? -dur.count() : dur.count());
}

namespace chrono {
//***********************************************************************
/// hh_mm_ss
//***********************************************************************
template <typename TDuration> class hh_mm_ss {
private:
  // Helper template to calculate the number of digits in a denominator at
  // compile time
  template <uintmax_t Den, int Width = 0> struct fractional_width_helper {
    static constexpr int value =
        fractional_width_helper<Den / 10, Width + 1>::value;
  };

  // Base case: when Den == 1, stop recursion
  template <int Width> struct fractional_width_helper<1, Width> {
    static constexpr int value = Width;
  };

  // Special case: when Den == 0 (invalid denominator), return 0
  template <int Width> struct fractional_width_helper<0, Width> {
    static constexpr int value = 0;
  };

  // Calculate fractional width for TDuration
  template <typename TDur> struct calculate_fractional_width {
    static constexpr int value =
        (TDur::period::den == 1)
            ? 0
            : fractional_width_helper<TDur::period::den>::value;
  };

public:
  GDUT_STATIC_ASSERT(
      (gdut::is_specialization<TDuration, gdut::chrono::duration>::value),
      "TDuration is not a gdut::chrono::duration type");

  static constexpr int fractional_width =
      calculate_fractional_width<TDuration>::value;

  //***********************************************************************
  /// The return type for to_duration.
  //***********************************************************************
  using precision = gdut::chrono::duration<
      common_type_t<typename TDuration::rep, gdut::chrono::seconds::rep>,
      ratio<1, gdut::power<10, fractional_width>::value>>;

  //***********************************************************************
  /// Default constructor.
  //***********************************************************************
  GDUT_CONSTEXPR
  hh_mm_ss() GDUT_NOEXCEPT : d(TDuration::zero()) {}

  //***********************************************************************
  /// Construct from duration.
  //***********************************************************************
  GDUT_CONSTEXPR14
  explicit hh_mm_ss(TDuration d_) GDUT_NOEXCEPT : d(d_) {}

  //***********************************************************************
  /// Checks for negative duration.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  bool is_negative() const GDUT_NOEXCEPT { return d < TDuration::zero(); }

  //***********************************************************************
  /// Returns the hours.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  gdut::chrono::hours hours() const GDUT_NOEXCEPT {
    auto dur = gdut::absolute(d);

    return gdut::chrono::duration_cast<gdut::chrono::hours>(dur);
  }

  //***********************************************************************
  /// Returns the minutes.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::minutes minutes() const GDUT_NOEXCEPT {
    auto dur = gdut::absolute(d) - hours();

    return gdut::chrono::duration_cast<gdut::chrono::minutes>(dur);
  }

  //***********************************************************************
  /// Returns the seconds.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  gdut::chrono::seconds seconds() const GDUT_NOEXCEPT {
    auto dur = gdut::absolute(d) - hours() - minutes();

    return gdut::chrono::duration_cast<gdut::chrono::seconds>(dur);
  }

  //***********************************************************************
  /// Returns the subseconds.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 precision subseconds() const GDUT_NOEXCEPT {
    return gdut::absolute(d) -
           gdut::chrono::duration_cast<gdut::chrono::seconds>(
               gdut::absolute(d));
  }

  //***********************************************************************
  /// Returns the duration.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 explicit operator precision() const GDUT_NOEXCEPT {
    return to_duration();
  }

  //***********************************************************************
  /// Returns the duration.
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  precision to_duration() const GDUT_NOEXCEPT { return d; }

private:
  TDuration d;
};

template <typename TDuration>
constexpr int gdut::chrono::hh_mm_ss<TDuration>::fractional_width;
} // namespace chrono
} // namespace gdut
