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

namespace gdut {
namespace chrono {
class year_month_day_last;

//*************************************************************************
/// year_month_day
//*************************************************************************
class year_month_day {
public:
  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR year_month_day() : y(), m(), d() {}

  //*************************************************************************
  /// Construct from month and day.
  //*************************************************************************
  GDUT_CONSTEXPR14 year_month_day(const gdut::chrono::year &y_,
                                  const gdut::chrono::month &m_,
                                  const gdut::chrono::day &d_) GDUT_NOEXCEPT
      : y(y_),
        m(m_),
        d(d_) {}

  //*************************************************************************
  /// Construct from year_month_day_last.
  //*************************************************************************
  GDUT_CONSTEXPR14
  year_month_day(const gdut::chrono::year_month_day_last &ymdl) GDUT_NOEXCEPT;

  //*************************************************************************
  /// Construct from sys_days.
  //*************************************************************************
  GDUT_CONSTEXPR14
  year_month_day(const gdut::chrono::sys_days &sd) GDUT_NOEXCEPT {
    // Days since 1970-01-01
    int days_since_epoch = static_cast<int>(sd.time_since_epoch().count());

    // Start from 1970-01-01
    gdut::chrono::year current_year(1970);
    gdut::chrono::month current_month(1);

    // Find the year
    while (true) {
      int days_in_year = current_year.is_leap() ? 366 : 365;

      if (days_since_epoch < days_in_year) {
        break;
      }

      days_since_epoch -= days_in_year;
      ++current_year;
    }

    // Find the month
    while (true) {
      unsigned char days_in_month =
          gdut::chrono::private_chrono::days_in_month[current_month];
      if (current_month == gdut::chrono::February && current_year.is_leap()) {
        ++days_in_month;
      }

      if (days_since_epoch < days_in_month) {
        break;
      }

      days_since_epoch -= days_in_month;
      ++current_month;
    }

    // The remaining days are the day of the month (0-based)
    y = current_year;
    m = current_month;
    d = gdut::chrono::day(static_cast<unsigned>(days_since_epoch) + 1);
  }

  //*************************************************************************
  /// Construct from local_days.
  //*************************************************************************
  GDUT_CONSTEXPR14
  year_month_day(const gdut::chrono::local_days &ld) GDUT_NOEXCEPT {
    gdut::chrono::year_month_day ymd = sys_days(ld.time_since_epoch());

    y = ymd.year();
    m = ymd.month();
    d = ymd.day();
  }

  //*************************************************************************
  /// Returns the year.
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::year year() const GDUT_NOEXCEPT { return y; }

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
  /// Returns true if the year/month/day is valid.
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT {
    // Check if the year, month, and day are valid individually
    return y.ok() && m.ok() && d.ok() && d <= max_day_for_month();
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day &
  operator+=(const gdut::chrono::years &dy) GDUT_NOEXCEPT {
    y += dy;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day &
  operator+=(const gdut::chrono::months &dm) GDUT_NOEXCEPT {
    m += dm;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day &
  operator-=(const gdut::chrono::years &dy) GDUT_NOEXCEPT {
    y -= dy;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day &
  operator-=(const gdut::chrono::months &dm) GDUT_NOEXCEPT {
    m -= dm;

    return *this;
  }

  //***********************************************************************
  /// Compare year_month_day with another.
  /// if year < other.year, returns -1
  /// else if year > other.year, returns 1
  /// if month < other.month, returns -1
  /// else if month > other.month, returns 1
  /// else if day < other.day, returns -1
  /// else if day > other.day, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int
  compare(const year_month_day &other) const GDUT_NOEXCEPT {
    if (y < other.y)
      return -1;
    if (y > other.y)
      return 1;
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

  //***********************************************************************
  /// Converts to gdut::chrono::sys_days
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 operator gdut::chrono::sys_days() const GDUT_NOEXCEPT {
    int day_count = 0;

    // Add days for years since 1970
    for (gdut::chrono::year yr(1970); yr < this->year(); ++yr) {
      day_count += yr.is_leap() ? 366 : 365;
    }

    // Add days for months in the current year
    for (gdut::chrono::month mth(1); mth < this->month(); ++mth) {
      day_count += private_chrono::days_in_month[mth];

      if (mth == gdut::chrono::February && this->year().is_leap()) {
        ++day_count; // Add one day for leap year February
      }
    }

    // Add days for the current month
    day_count += static_cast<unsigned>(this->day()) - 1;

    return sys_days(gdut::chrono::days(day_count));
  }

  //***********************************************************************
  /// Converts to gdut::chrono::local_days
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 explicit
  operator gdut::chrono::local_days() const GDUT_NOEXCEPT {
    return local_days(sys_days(*this).time_since_epoch());
  }

private:
  //***********************************************************************
  /// Calculates the last day in the year/month.
  /// Returns 0 if either the year or month are not OK.
  //***********************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::day max_day_for_month() const {
    unsigned char count = 0;

    if (y.ok() && m.ok()) {
      count = private_chrono::days_in_month[m];

      if (y.is_leap() && (m == February)) {
        ++count;
      }
    }

    return gdut::chrono::day(count);
  }

  gdut::chrono::year y;
  gdut::chrono::month m;
  gdut::chrono::day d;
};

//*************************************************************************
/// Adds gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator+(const gdut::chrono::year_month_day &ymd,
          const gdut::chrono::years &dy) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year() + dy, ymd.month(), ymd.day());
}

//*************************************************************************
/// Adds gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator+(const gdut::chrono::years &dy,
          const gdut::chrono::year_month_day &ymd) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year() + dy, ymd.month(), ymd.day());
}

//*************************************************************************
/// Adds gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator+(const gdut::chrono::year_month_day &ymd,
          const gdut::chrono::months &dm) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year(), ymd.month() + dm, ymd.day());
}

//*************************************************************************
/// Adds gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator+(const gdut::chrono::months &dm,
          const gdut::chrono::year_month_day &ymd) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year(), ymd.month() + dm, ymd.day());
}

//*************************************************************************
/// Subtracts gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator-(const gdut::chrono::year_month_day &ymd,
          const gdut::chrono::years &dy) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year() - dy, ymd.month(), ymd.day());
}

//*************************************************************************
/// Subtracts gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator-(const gdut::chrono::year_month_day &ymd,
          const gdut::chrono::months &dm) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ymd.year(), ymd.month() - dm, ymd.day());
}

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::year_month_day &lhs,
           const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  return (lhs.year() == rhs.year()) && (lhs.month() == rhs.month()) &&
         (lhs.day() == rhs.day());
}

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::year_month_day &lhs,
           const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// Less-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<(const gdut::chrono::year_month_day &lhs,
          const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  if (lhs.year() < rhs.year()) {
    return true;
  } else if (lhs.year() == rhs.year()) {
    if (lhs.month() < rhs.month()) {
      return true;
    } else if (lhs.month() == rhs.month()) {
      return (lhs.day() < rhs.day());
    } else {
      return false;
    }
  } else {
    return false;
  }
}

//*************************************************************************
/// Less-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<=(const gdut::chrono::year_month_day &lhs,
           const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  return !(rhs < lhs);
}

//*************************************************************************
/// Greater-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>(const gdut::chrono::year_month_day &lhs,
          const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  return rhs < lhs;
}

//*************************************************************************
/// Greater-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>=(const gdut::chrono::year_month_day &lhs,
           const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  return !(lhs < rhs);
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::year_month_day &lhs,
            const gdut::chrono::year_month_day &rhs) GDUT_NOEXCEPT {
  auto cmp = lhs.year() <=> rhs.year();

  if (cmp != 0) {
    return cmp;
  }

  cmp = lhs.month() <=> rhs.month();

  if (cmp != 0) {
    return cmp;
  }

  return lhs.day() <=> rhs.day();
}
#endif

//*************************************************************************
/// year_month_day_last
//*************************************************************************
class year_month_day_last {
public:
  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14
  year_month_day_last(const gdut::chrono::year &y_,
                      const gdut::chrono::month_day_last &mdl_) GDUT_NOEXCEPT
      : y(y_),
        m(mdl_.month()) {}

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::year year() const GDUT_NOEXCEPT { return y; }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::month month() const GDUT_NOEXCEPT { return m; }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::day day() const GDUT_NOEXCEPT {
    gdut::chrono::day d =
        gdut::chrono::day(gdut::chrono::private_chrono::days_in_month[m]);

    return (d == 28) && y.is_leap() ? gdut::chrono::day(29) : d;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 gdut::chrono::month_day_last
  month_day_last() const GDUT_NOEXCEPT {
    return gdut::chrono::month_day_last(m);
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT { return y.ok() && m.ok(); }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last &
  operator+=(const gdut::chrono::years &dy) GDUT_NOEXCEPT {
    y += dy;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last &
  operator+=(const gdut::chrono::months &dm) GDUT_NOEXCEPT {
    m += dm;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last &
  operator-=(const gdut::chrono::years &dy) GDUT_NOEXCEPT {
    y -= dy;

    return *this;
  }

  //*************************************************************************
  ///
  //*************************************************************************
  GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last &
  operator-=(const gdut::chrono::months &dm) GDUT_NOEXCEPT {
    m -= dm;

    return *this;
  }

  //***********************************************************************
  /// Compare year_month_day with another.
  /// if year < other.year, returns -1
  /// else if year > other.year, returns 1
  /// if month < other.month, returns -1
  /// else if month > other.month, returns 1
  /// else returns 0
  //***********************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 int
  compare(const year_month_day_last &other) const GDUT_NOEXCEPT {
    if (y < other.y)
      return -1;
    if (y > other.y)
      return 1;
    if (m < other.m)
      return -1;
    if (m > other.m)
      return 1;

    return 0;
  }

  //*************************************************************************
  /// Converts to gdut::chrono::sys_days
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 operator gdut::chrono::sys_days() const GDUT_NOEXCEPT {
    gdut::chrono::year_month_day ymd(year(), month(), day());

    return gdut::chrono::sys_days(ymd);
  }

  //*************************************************************************
  /// Converts to gdut::chrono::local_days
  //*************************************************************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14 explicit
  operator gdut::chrono::local_days() const GDUT_NOEXCEPT {
    return local_days(sys_days(*this).time_since_epoch());
  }

private:
  gdut::chrono::year y;
  gdut::chrono::month m;
};

//*************************************************************************
/// Adds gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator+(const gdut::chrono::year_month_day_last &ymdl,
          const gdut::chrono::years &dy) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(ymdl.year() + dy,
                                           ymdl.month_day_last());
}

//*************************************************************************
/// Adds gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator+(const gdut::chrono::years &dy,
          const gdut::chrono::year_month_day_last &ymdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(ymdl.year() + dy,
                                           ymdl.month_day_last());
}

//*************************************************************************
/// Adds gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator+(const gdut::chrono::year_month_day_last &ymdl,
          const gdut::chrono::months &dm) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(
      ymdl.year(), gdut::chrono::month_day_last(ymdl.month() + dm));
}

//*************************************************************************
/// Adds gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator+(const gdut::chrono::months &dm,
          const gdut::chrono::year_month_day_last &ymdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(
      ymdl.year(), gdut::chrono::month_day_last(ymdl.month() + dm));
}

//*************************************************************************
/// Subtracts gdut::chrono::years
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator-(const gdut::chrono::year_month_day_last &ymdl,
          const gdut::chrono::years &dy) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(ymdl.year() - dy,
                                           ymdl.month_day_last());
}

//*************************************************************************
/// Subtracts gdut::chrono::months
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator-(const gdut::chrono::year_month_day_last &ymdl,
          const gdut::chrono::months &dm) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(
      ymdl.year(), gdut::chrono::month_day_last(ymdl.month() - dm));
}

//*************************************************************************
/// Construct from year_month_day_last.
//*************************************************************************
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day::year_month_day(
    const gdut::chrono::year_month_day_last &ymdl) GDUT_NOEXCEPT
    : y(ymdl.year()),
      m(ymdl.month()),
      d(ymdl.day()) {}

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator==(const gdut::chrono::year_month_day_last &lhs,
           const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  return (lhs.year() == rhs.year()) && (lhs.month() == rhs.month());
}

//*************************************************************************
/// Equality operator.
//*************************************************************************
inline GDUT_CONSTEXPR14 bool
operator!=(const gdut::chrono::year_month_day_last &lhs,
           const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// Less-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<(const gdut::chrono::year_month_day_last &lhs,
          const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  if (lhs.year() < rhs.year()) {
    return true;
  } else if (lhs.year() == rhs.year()) {
    return (lhs.month() < rhs.month());
  } else {
    return false;
  }
}

//*************************************************************************
/// Less-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator<=(const gdut::chrono::year_month_day_last &lhs,
           const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  return !(rhs < lhs);
}

//*************************************************************************
/// Greater-than operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>(const gdut::chrono::year_month_day_last &lhs,
          const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  return rhs < lhs;
}

//*************************************************************************
/// Greater-than-equal operator.
//*************************************************************************
GDUT_NODISCARD GDUT_CONSTEXPR14 inline bool
operator>=(const gdut::chrono::year_month_day_last &lhs,
           const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  return !(lhs < rhs);
}

//***********************************************************************
/// Spaceship operator
//***********************************************************************
#if GDUT_USING_CPP20
[[nodiscard]] inline constexpr auto
operator<=>(const gdut::chrono::year_month_day_last &lhs,
            const gdut::chrono::year_month_day_last &rhs) GDUT_NOEXCEPT {
  auto cmp1 = lhs.year() <=> rhs.year();

  if (cmp1 != 0) {
    return cmp1;
  } else {
    auto cmp2 = lhs.month() <=> rhs.month();

    if (cmp2 != 0) {
      return cmp2;
    } else {
      return lhs.month() <=> rhs.month();
    }
  }
}
#endif
} // namespace chrono

//*************************************************************************
/// Hash function for gdut::chrono::year_month_day
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::year_month_day> {
  size_t operator()(const gdut::chrono::year_month_day &ymd) const {
    gdut::chrono::year::rep y =
        static_cast<gdut::chrono::year::rep>(static_cast<unsigned>(ymd.year()));
    gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(
        static_cast<unsigned>(ymd.month()));
    gdut::chrono::day::rep d =
        static_cast<gdut::chrono::day::rep>(static_cast<unsigned>(ymd.day()));

    uint8_t buffer[sizeof(y) + sizeof(m) + sizeof(d)];

    memcpy(buffer, &y, sizeof(y));
    memcpy(buffer + sizeof(y), &m, sizeof(m));
    memcpy(buffer + sizeof(y) + sizeof(m), &d, sizeof(d));

    return gdut::private_hash::generic_hash<size_t>(
        buffer, buffer + sizeof(y) + sizeof(m) + sizeof(d));
  }
};
#endif

//*************************************************************************
/// Hash function for gdut::chrono::year_month_day_last
//*************************************************************************
#if GDUT_USING_8BIT_TYPES
template <> struct hash<gdut::chrono::year_month_day_last> {
  size_t operator()(const gdut::chrono::year_month_day_last &ymdl) const {
    gdut::chrono::year::rep y = static_cast<gdut::chrono::year::rep>(
        static_cast<unsigned>(ymdl.year()));
    gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(
        static_cast<unsigned>(ymdl.month()));
    gdut::chrono::day::rep d =
        static_cast<gdut::chrono::day::rep>(static_cast<unsigned>(ymdl.day()));

    uint8_t buffer[sizeof(y) + sizeof(m) + sizeof(d)];

    memcpy(buffer, &y, sizeof(y));
    memcpy(buffer + sizeof(y), &m, sizeof(m));
    memcpy(buffer + sizeof(y) + sizeof(m), &d, sizeof(d));

    return gdut::private_hash::generic_hash<size_t>(
        buffer, buffer + sizeof(y) + sizeof(m) + sizeof(d));
  }
};
#endif
} // namespace gdut
