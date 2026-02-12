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
class month;

GDUT_CONSTEXPR14 gdut::chrono::month
operator+(const gdut::chrono::month &m,
          const gdut::chrono::months &ms) GDUT_NOEXCEPT;
GDUT_CONSTEXPR14 gdut::chrono::month
operator+(const gdut::chrono::months &ms,
          const gdut::chrono::month &m) GDUT_NOEXCEPT;
GDUT_CONSTEXPR14 gdut::chrono::month
operator-(const gdut::chrono::month &m,
          const gdut::chrono::months &ms) GDUT_NOEXCEPT;

namespace private_chrono {
static GDUT_CONSTANT unsigned char days_in_month[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
}

//***********************************************************************
/// month
//***********************************************************************
class month {
public:
  using rep = uint_least8_t;

  //***********************************************************************
  /// Default constructor
  //***********************************************************************
  GDUT_CONSTEXPR month() GDUT_NOEXCEPT : value(0) {}

  //***********************************************************************
  /// Construct from unsigned
  //***********************************************************************
  GDUT_CONSTEXPR explicit month(unsigned value_) GDUT_NOEXCEPT : value(value_) {
  }

  //***********************************************************************
  /// Copy constructor
  //***********************************************************************
  GDUT_CONSTEXPR14 month(const gdut::chrono::month &other) GDUT_NOEXCEPT
      : value(other.value) {}

  //***********************************************************************
  /// Assignment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month &
  operator=(const gdut::chrono::month &rhs) GDUT_NOEXCEPT {
    value = rhs.value;

    return *this;
  }

  //***********************************************************************
  /// Pre-increment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month &operator++() GDUT_NOEXCEPT {
    *this += gdut::chrono::months(1);

    return *this;
  }

  //***********************************************************************
  /// Post-increment operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month operator++(int) GDUT_NOEXCEPT {
    const gdut::chrono::month temp = *this;

    *this += gdut::chrono::months(1);

    return temp;
  }

  //***********************************************************************
  /// Pre-decrement operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month &operator--() GDUT_NOEXCEPT {
    *this -= gdut::chrono::months(1);

    return *this;
  }

  //***********************************************************************
  /// Post-decrement operator
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month operator--(int) GDUT_NOEXCEPT {
    gdut::chrono::month temp = *this;

    *this -= gdut::chrono::months(1);

    return temp;
  }

  //***********************************************************************
  /// Plus-equals operator adding gdut::chrono::months
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month &
  operator+=(const gdut::chrono::months &ms) GDUT_NOEXCEPT {
    *this = *this + ms;

    return *this;
  }

  //***********************************************************************
  /// Minus-equals operator subtracting gdut::chrono::months
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month &
  operator-=(const gdut::chrono::months &ms) GDUT_NOEXCEPT {
    *this = *this - ms;

    return *this;
  }

  //***********************************************************************
  /// Returns <b>true</b> if the month is within the valid 1 to 31 range
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT {
    return (value >= 1U) && (value <= 12U);
  }

  //***********************************************************************
  /// Compare month with another.
  /// if month < other, returns -1
  /// else if month > other, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int compare(const month &other) const GDUT_NOEXCEPT {
    if (value < other.value)
      return -1;
    if (value > other.value)
      return 1;

    return 0;
  }

  //***********************************************************************
  /// The minimum month value for which ok() will return <b>true</b>
  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::month min() GDUT_NOEXCEPT {
    return gdut::chrono::month(1);
  }

  //***********************************************************************
  /// The maximum month value for which ok() will return <b>true</b>
  //***********************************************************************
  GDUT_NODISCARD
  static GDUT_CONSTEXPR14 gdut::chrono::month max() GDUT_NOEXCEPT {
    return gdut::chrono::month(12);
  }

  //***********************************************************************
  /// Conversion operator to unsigned int
  //***********************************************************************
  GDUT_CONSTEXPR14 operator unsigned() const GDUT_NOEXCEPT {
    return static_cast<unsigned>(value);
  }

private:
  rep value;
};

//***********************************************************************
/// Equality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::month &d1,
           const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) == static_cast<unsigned>(d2));
}

//***********************************************************************
/// Inequality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::month &d1,
           const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return !(d1 == d2);
}

//***********************************************************************
/// Less-than operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator<(const gdut::chrono::month &d1,
          const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) < static_cast<unsigned>(d2));
}

//***********************************************************************
/// Less-than-or-equal operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator<=(const gdut::chrono::month &d1,
           const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) <= static_cast<unsigned>(d2));
}

//***********************************************************************
/// Greater-than operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator>(const gdut::chrono::month &d1,
          const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) > static_cast<unsigned>(d2));
}

//***********************************************************************
/// Greater-than-or-equal operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator>=(const gdut::chrono::month &d1,
           const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) >= static_cast<unsigned>(d2));
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::month &d1,
            const gdut::chrono::month &d2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(d1) <=> static_cast<unsigned>(d2));
}
#endif

//***********************************************************************
/// Add gdut::chrono::months to gdut::chrono::month
///\return gdut::chrono::month
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::month
operator+(const gdut::chrono::month &m,
          const gdut::chrono::months &ms) GDUT_NOEXCEPT {
  unsigned int value = static_cast<unsigned int>(m);

  value = value % 12U;

  if (value == 0U) {
    value = 12U;
  }

  int delta = ms.count() % 12;

  // Adjust to allow a limited +-11 month delta
  value += 11U;
  value += delta;
  value %= 12U;
  ++value;

  return gdut::chrono::month(value);
}

//***********************************************************************
/// Add gdut::chrono::month to gdut::chrono::months
///\return gdut::chrono::month
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::month
operator+(const gdut::chrono::months &ms,
          const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return m + ms;
}

//***********************************************************************
/// Subtract gdut::chrono::months from gdut::chrono::month
///\return gdut::chrono::month
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::month
operator-(const gdut::chrono::month &m,
          const gdut::chrono::months &ms) GDUT_NOEXCEPT {
  return m + gdut::chrono::months(-ms.count());
}

//***********************************************************************
/// Subtract gdut::chrono::month from gdut::chrono::month
///\return gdut::chrono::months
//***********************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::months
operator-(const gdut::chrono::month &m1,
          const gdut::chrono::month &m2) GDUT_NOEXCEPT {
  if (m1.ok() && m2.ok()) {
    // Calculate the signed difference.
    int difference = static_cast<int>(static_cast<unsigned>(m1)) -
                     static_cast<int>(static_cast<unsigned>(m2));

    // Adjust for wrap-around.
    if (difference < 0) {
      difference += 12;
    }

    gdut::chrono::months ms(difference);

    // Check for validity.
    if (m1 == (m2 + ms)) {
      return ms;
    }
  }

  return gdut::chrono::months();
}

#if GDUT_USING_CPP17
inline constexpr gdut::chrono::month January{1};
inline constexpr gdut::chrono::month February{2};
inline constexpr gdut::chrono::month March{3};
inline constexpr gdut::chrono::month April{4};
inline constexpr gdut::chrono::month May{5};
inline constexpr gdut::chrono::month June{6};
inline constexpr gdut::chrono::month July{7};
inline constexpr gdut::chrono::month August{8};
inline constexpr gdut::chrono::month September{9};
inline constexpr gdut::chrono::month October{10};
inline constexpr gdut::chrono::month November{11};
inline constexpr gdut::chrono::month December{12};
#else
static GDUT_CONSTANT gdut::chrono::month January{1};
static GDUT_CONSTANT gdut::chrono::month February{2};
static GDUT_CONSTANT gdut::chrono::month March{3};
static GDUT_CONSTANT gdut::chrono::month April{4};
static GDUT_CONSTANT gdut::chrono::month May{5};
static GDUT_CONSTANT gdut::chrono::month June{6};
static GDUT_CONSTANT gdut::chrono::month July{7};
static GDUT_CONSTANT gdut::chrono::month August{8};
static GDUT_CONSTANT gdut::chrono::month September{9};
static GDUT_CONSTANT gdut::chrono::month October{10};
static GDUT_CONSTANT gdut::chrono::month November{11};
static GDUT_CONSTANT gdut::chrono::month December{12};
#endif
} // namespace chrono

//*************************************************************************
/// Hash function for gdut::chrono::month
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::month> {
  size_t operator()(const gdut::chrono::month &m) const {
    gdut::chrono::month::rep value =
        static_cast<gdut::chrono::month::rep>(static_cast<unsigned>(m));
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&value);

    return gdut::private_hash::generic_hash<size_t>(p, p + sizeof(value));
  }
};
#endif

} // namespace gdut
