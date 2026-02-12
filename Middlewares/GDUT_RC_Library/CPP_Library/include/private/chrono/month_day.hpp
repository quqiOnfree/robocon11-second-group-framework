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
class month_day {
public:
  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  month_day() = default;

  //*************************************************************************
  /// Construct from month and day.
  //*************************************************************************
  GDUT_CONSTEXPR14 month_day(const gdut::chrono::month &m_,
                             const gdut::chrono::day &d_) GDUT_NOEXCEPT
      : m(m_),
        d(d_) {}

  //*************************************************************************
  /// Returns the month.
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::month month() const GDUT_NOEXCEPT { return m; }

  //*************************************************************************
  /// Returns the day.
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::day day() const GDUT_NOEXCEPT { return d; }

  //*************************************************************************
  /// Returns true if the month/day is valid.
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT {
    if (!m.ok() || !d.ok()) {
      return false;
    }

    unsigned max_day = 0;

    unsigned m_v = static_cast<unsigned>(m);
    max_day = private_chrono::days_in_month[m_v];

    return (max_day > 0) && (static_cast<unsigned>(d) >= 1) &&
           (static_cast<unsigned>(d) <= max_day);
  }

  //***********************************************************************
  /// Compare month_day with another.
  /// if month < other.month, returns -1
  /// else if month > other.month, returns 1
  /// else if day < other.day, returns -1
  /// else if day > other.day, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int compare(const month_day &other) const GDUT_NOEXCEPT {
    if (m < other.m)
      return -1;
    if (m > other.m)
      return 1;
    if (d < other.d)
      return -1;
    if (d > other.d)
      return 1;

    return 0;
  }

private:
  gdut::chrono::month m;
  gdut::chrono::day d;
};

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::month_day &lhs,
           const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  return (lhs.day() == rhs.day()) && (lhs.month() == rhs.month());
}

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::month_day &lhs,
           const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// Less-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<(const gdut::chrono::month_day &lhs,
          const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  if (lhs.month() < rhs.month()) {
    return true;
  } else if (lhs.month() == rhs.month()) {
    return lhs.day() < rhs.day();
  } else {
    return false;
  }
}

//*************************************************************************
/// Less-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<=(const gdut::chrono::month_day &lhs,
           const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  return !(rhs < lhs);
}

//*************************************************************************
/// Greater-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>(const gdut::chrono::month_day &lhs,
          const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  return rhs < lhs;
}

//*************************************************************************
/// Greater-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>=(const gdut::chrono::month_day &lhs,
           const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  return !(lhs < rhs);
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::month_day &lhs,
            const gdut::chrono::month_day &rhs) GDUT_NOEXCEPT {
  auto cmp = lhs.month() <=> rhs.month();

  if (cmp != 0) {
    return cmp;
  } else {
    return lhs.day() <=> rhs.day();
  }
}
#endif

//*************************************************************************
/// month_day_last
//*************************************************************************
class month_day_last {
public:
  //*************************************************************************
  /// Construct from month.
  //*************************************************************************
  GDUT_CONSTEXPR14 explicit month_day_last(const gdut::chrono::month &m_)
      GDUT_NOEXCEPT : m(m_) {}

  //*************************************************************************
  /// Get the month.
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::month month() const GDUT_NOEXCEPT { return m; }

  //*************************************************************************
  /// Is the contained month OK?
  //*************************************************************************
  bool ok() const GDUT_NOEXCEPT { return m.ok(); }

  //***********************************************************************
  /// Compare month_day with another.
  /// if month < other.month, returns -1
  /// else if month > other.month, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int compare(const month_day &other) const GDUT_NOEXCEPT {
    if (m < other.month())
      return -1;
    if (m > other.month())
      return 1;

    return 0;
  }

private:
  gdut::chrono::month m;
};

//***********************************************************************
/// Equality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::month_day_last &mdl1,
           const gdut::chrono::month_day_last &mdl2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(mdl1.month()) ==
          static_cast<unsigned>(mdl2.month()));
}

//***********************************************************************
/// Inequality operator
//***********************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::month_day_last &mdl1,
           const gdut::chrono::month_day_last &mdl2) GDUT_NOEXCEPT {
  return !(mdl1 == mdl2);
}

//*************************************************************************
/// Less-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<(const gdut::chrono::month_day_last &lhs,
          const gdut::chrono::month_day_last &rhs) GDUT_NOEXCEPT {
  return (lhs.month() < rhs.month());
}

//*************************************************************************
/// Less-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<=(const gdut::chrono::month_day_last &lhs,
           const gdut::chrono::month_day_last &rhs) GDUT_NOEXCEPT {
  return !(rhs < lhs);
}

//*************************************************************************
/// Greater-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>(const gdut::chrono::month_day_last &lhs,
          const gdut::chrono::month_day_last &rhs) GDUT_NOEXCEPT {
  return rhs < lhs;
}

//*************************************************************************
/// Greater-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>=(const gdut::chrono::month_day_last &lhs,
           const gdut::chrono::month_day_last &rhs) GDUT_NOEXCEPT {
  return !(lhs < rhs);
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::month_day_last &mdl1,
            const gdut::chrono::month_day_last &mdl2) GDUT_NOEXCEPT {
  return (static_cast<unsigned>(mdl1.month()) <=>
          static_cast<unsigned>(mdl2.month()));
}
#endif
} // namespace chrono

//*************************************************************************
/// Hash function for gdut::chrono::month_day
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::month_day> {
  size_t operator()(const gdut::chrono::month_day &md) const {
    gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(
        static_cast<unsigned>(md.month()));
    gdut::chrono::day::rep d =
        static_cast<gdut::chrono::day::rep>(static_cast<unsigned>(md.day()));

    uint8_t buffer[sizeof(m) + sizeof(d)];

    memcpy(buffer, &m, sizeof(m));
    memcpy(buffer + sizeof(m), &d, sizeof(d));

    return gdut::private_hash::generic_hash<size_t>(buffer, buffer + sizeof(m) +
                                                                sizeof(d));
  }
};
#endif

//*************************************************************************
/// Hash function for gdut::chrono::month_day_last
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::month_day_last> {
  size_t operator()(const gdut::chrono::month_day_last &mdl) const {
    gdut::chrono::month::rep value = static_cast<gdut::chrono::month::rep>(
        static_cast<unsigned>(mdl.month()));
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&value);

    return gdut::private_hash::generic_hash<size_t>(p, p + sizeof(value));
  }
};
#endif
} // namespace gdut
