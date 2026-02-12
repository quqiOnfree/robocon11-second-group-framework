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

namespace gdut {
namespace chrono {
//***********************************************************************
/// day
//***********************************************************************
class day {
public:
  using rep = uint_least8_t;

  //***********************************************************************
  /// Default constructor
  //***********************************************************************
  GDUT_CONSTEXPR day() GDUT_NOEXCEPT : value(0) {}

  //***********************************************************************
  /// Construct from unsigned
  //***********************************************************************
  GDUT_CONSTEXPR explicit day(unsigned value_) GDUT_NOEXCEPT
      : value(static_cast<unsigned char>(value_)) {}

  //***********************************************************************
  /// Copy constructor
  //***********************************************************************
  GDUT_CONSTEXPR14 day(const gdut::chrono::day &other) GDUT_NOEXCEPT
      : value(other.value) {}

  //***********************************************************************
  /// Assignment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day &
  operator=(const gdut::chrono::day &rhs) GDUT_NOEXCEPT {
    value = rhs.value;

    return *this;
  }

  //***********************************************************************
  /// Assignment operator
  //***********************************************************************
  template <typename TToDuration, typename TValue2, typename TPeriod2>
  GDUT_CONSTEXPR14 gdut::chrono::day &
  operator=(const gdut::chrono::duration<TValue2, TPeriod2> &rhs) {
    value = gdut::chrono::duration_cast<TToDuration, TValue2, TPeriod2>(rhs);

    return *this;
  }

  //***********************************************************************
  /// Pre-increment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day &operator++() GDUT_NOEXCEPT {
    ++value;

    return *this;
  }

  //***********************************************************************
  /// Post-increment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day operator++(int) GDUT_NOEXCEPT {
    const gdut::chrono::day temp = *this;
    ++value;

    return temp;
  }

  //***********************************************************************
  /// Pre-decrement operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day &operator--() GDUT_NOEXCEPT {
    --value;

    return *this;
  }

  //***********************************************************************
  /// Post-decrement operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day operator--(int) GDUT_NOEXCEPT {
    const gdut::chrono::day temp = *this;
    --value;

    return temp;
  }

  //***********************************************************************
  /// Plus-equals operator adding gdut::chrono::days
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day &
  operator+=(const gdut::chrono::days &ds) GDUT_NOEXCEPT {
    value += static_cast<unsigned char>(ds.count());

    return *this;
  }

  //***********************************************************************
  /// Minus-equals operator subtracting gdut::chrono::days
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day &
  operator-=(const gdut::chrono::days &ds) GDUT_NOEXCEPT {
    value -= static_cast<unsigned char>(ds.count());

    return *this;
  }

  //***********************************************************************
  /// Returns <b>true</b> if the day is within the valid 1 to 31 range
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT {
    return (value >= 1U) && (value <= 31U);
  }

  //***********************************************************************
  /// Conversion operator to unsigned int
  //***********************************************************************
  GDUT_CONSTEXPR14 operator unsigned() const GDUT_NOEXCEPT {
    return static_cast<unsigned>(value);
  }

  //***********************************************************************
  /// Compare day with another.
  /// if day < other, returns -1
  /// else if day > other, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int compare(const day &other) const GDUT_NOEXCEPT {
    if (value < other.value)
      return -1;
    if (value > other.value)
      return 1;

    return 0;
  }

  //***********************************************************************
  /// The minimum day value for which ok() will return <b>true</b>
  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::day min() GDUT_NOEXCEPT {
    return gdut::chrono::day(1);
  }

  //***********************************************************************
  /// The maximum day value for which ok() will return <b>true</b>
  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::day max() GDUT_NOEXCEPT {
    return gdut::chrono::day(31);
  }

private:
  rep value;
};

//***********************************************************************
/// Equality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::day &d1,
           const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) == static_cast<unsigned>(d2));
}

//***********************************************************************
/// Inequality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::day &d1,
           const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return !(d1 == d2);
}

//***********************************************************************
/// Less-than operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator<(const gdut::chrono::day &d1,
          const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) < static_cast<unsigned>(d2));
}

//***********************************************************************
/// Less-than-or-equal operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator<=(const gdut::chrono::day &d1,
           const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) <= static_cast<unsigned>(d2));
}

//***********************************************************************
/// Greater-than operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator>(const gdut::chrono::day &d1,
          const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) > static_cast<unsigned>(d2));
}

//***********************************************************************
/// Greater-than-or-equal operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator>=(const gdut::chrono::day &d1,
           const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) >= static_cast<unsigned>(d2));
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::day &d1,
            const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) <=> static_cast<unsigned>(d2));
}
#endif

//***********************************************************************
/// Add gdut::chrono::days to gdut::chrono::day
///\return gdut::chrono::day
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::day
operator+(const gdut::chrono::day &d,
          const gdut::chrono::days &ds) GDUT_NOEXCEPT {
  gdut::chrono::day result(d);

  result += ds;

  return result;
}

//***********************************************************************
/// Add gdut::chrono::day to gdut::chrono::days
///\return gdut::chrono::day
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::day
operator+(const gdut::chrono::days &ds,
          const gdut::chrono::day &d) GDUT_NOEXCEPT {
  gdut::chrono::day result(d);

  result += ds;

  return result;
}

//***********************************************************************
/// Subtract gdut::chrono::days from gdut::chrono::day
///\return gdut::chrono::day
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::day
operator-(const gdut::chrono::day &d,
          const gdut::chrono::days &ds) GDUT_NOEXCEPT {
  gdut::chrono::day result(d);

  result -= ds;

  return result;
}

//***********************************************************************
/// Subtract gdut::chrono::day from gdut::chrono::day
///\return gdut::chrono::days
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::days
operator-(const gdut::chrono::day &d1,
          const gdut::chrono::day &d2) GDUT_NOEXCEPT {
  return gdut::chrono::days(static_cast<int>(static_cast<unsigned>(d1)) -
                            static_cast<int>(static_cast<unsigned>(d2)));
}
} // namespace chrono

//*************************************************************************
/// Hash function for gdut::chrono::day
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::day> {
  size_t operator()(const gdut::chrono::day &d) const {
    gdut::chrono::day::rep value =
        static_cast<gdut::chrono::day::rep>(static_cast<unsigned>(d));
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&value);

    return gdut::private_hash::generic_hash<size_t>(p, p + sizeof(value));
  }
};
#endif
} // namespace gdut

#if GDUT_HAS_CHRONO_LITERALS_DAY
namespace gdut {
inline namespace literals {
inline namespace chrono_literals {
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
inline GDUT_CONSTEXPR14 gdut::chrono::day
operator""_day(unsigned long long d) GDUT_NOEXCEPT
#else
inline GDUT_CONSTEXPR14 gdut::chrono::day
operator""_d(unsigned long long d) GDUT_NOEXCEPT
#endif
{
  return gdut::chrono::day(static_cast<unsigned>(d));
}
} // namespace chrono_literals
} // namespace literals
} // namespace gdut
#endif
