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

namespace gdut
{
  namespace chrono
  {
    //*************************************************************************
    /// year_month_weekday
    //*************************************************************************
    class year_month_weekday
    {
    public:

      //*************************************************************************
      /// Default constructor.
      //*************************************************************************
      GDUT_CONSTEXPR year_month_weekday()
        : y()
        , m()
        , wdi()
      {
      }

      //*************************************************************************
      /// Construct from month, day, and weekday_indexed.
      //*************************************************************************
      GDUT_CONSTEXPR14 year_month_weekday(const gdut::chrono::year&    y_, 
                                         const gdut::chrono::month&   m_, 
                                         const gdut::chrono::weekday_indexed& wdi_) GDUT_NOEXCEPT
        : y(y_)
        , m(m_)
        , wdi(wdi_)
      {
      }

      //*************************************************************************
      /// Construct from sys_days.
      //*************************************************************************
      GDUT_CONSTEXPR14 year_month_weekday(const gdut::chrono::sys_days& sd) GDUT_NOEXCEPT
      {
        // Extract year, month, and day
        year_month_day ymd = year_month_day{sd};

        gdut::chrono::year  yr  = ymd.year();
        gdut::chrono::month mth = ymd.month();
        gdut::chrono::day   dy  = ymd.day();

        // Get the weekday from sys_days
        gdut::chrono::weekday wd = gdut::chrono::weekday{sd};

        // Count how many times this weekday has occurred in the month so far
        // We walk backward from the given day in steps of 7 days
        unsigned index = 1;
        
        for (int offset = static_cast<int>(dy) - 7; offset > 0; offset -= 7) 
        {
          ++index;
        }

        y = yr;
        m = mth;
        wdi = gdut::chrono::weekday_indexed{ wd, index };
      }

      //*************************************************************************
      /// Construct from local_days.
      //*************************************************************************
      GDUT_CONSTEXPR14 year_month_weekday(const gdut::chrono::local_days& ld) GDUT_NOEXCEPT
      {
        year_month_weekday ymwd(sys_days(ld.time_since_epoch()));

        y   = ymwd.year();
        m   = ymwd.month();
        wdi = ymwd.weekday_indexed();
      }

      //*************************************************************************
      /// Returns the year.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::year year() const GDUT_NOEXCEPT
      {
        return y;
      }

      //*************************************************************************
      /// Returns the month.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::month month() const GDUT_NOEXCEPT
      {
        return m;
      }

      //*************************************************************************
      /// Returns the weekday.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday weekday() const GDUT_NOEXCEPT
      {
        return wdi.weekday();
      }

      //*************************************************************************
      /// Returns the weekday index.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 unsigned index() const GDUT_NOEXCEPT
      {
        return wdi.index();
      }

      //*************************************************************************
      /// Returns the weekday_indexed.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday_indexed weekday_indexed() const GDUT_NOEXCEPT
      {
        return wdi;
      }

      //*************************************************************************
      /// Returns true if the year/month/day is valid.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {       
        return y.ok() && m.ok() && wdi.ok();
      }

      //***********************************************************************
      /// Converts to gdut::chrono::sys_days
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 operator gdut::chrono::sys_days() const GDUT_NOEXCEPT
      {
        if (ok())
        {
          gdut::chrono::year_month_weekday ymwd(*this);
          gdut::chrono::year_month_day ymd{ ymwd.year(), ymwd.month(), gdut::chrono::day(1) };

          gdut::chrono::sys_days sd = ymd;

          unsigned int target_wd = ymwd.weekday().c_encoding();
          unsigned int target_index = ymwd.index();

          gdut::chrono::weekday first_weekday(static_cast<int>(sd.time_since_epoch().count()));

          int first_wd = first_weekday.c_encoding();
          int offset = (target_wd - first_wd + 7) % 7;
          int day_of_month = offset + static_cast<int>(target_index - 1) * 7;

          gdut::chrono::year_month_day result(year(), month(), gdut::chrono::day(day_of_month));

          return gdut::chrono::sys_days(result);
        }
        else
        {
          return gdut::chrono::sys_days();
        }
      }

      //***********************************************************************
      /// Converts to gdut::chrono::local_days
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 operator gdut::chrono::local_days() const GDUT_NOEXCEPT
      {
        return local_days(sys_days(*this).time_since_epoch());
      }

    private:

      gdut::chrono::year    y;
      gdut::chrono::month   m;
      gdut::chrono::weekday_indexed wdi;
    };

    //*************************************************************************
    /// Adds gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator +(const gdut::chrono::year_month_weekday& ymwd,
                                                                      const gdut::chrono::years& dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year() + dy, ymwd.month(), ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Adds gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator +(const gdut::chrono::years& dy,
                                                                      const gdut::chrono::year_month_weekday& ymwd) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year() + dy, ymwd.month(), ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Adds gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator +(const gdut::chrono::year_month_weekday& ymwd,
                                                                      const gdut::chrono::months& dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year(), ymwd.month() + dm, ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Adds gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator +(const gdut::chrono::months& dm,
                                                                      const gdut::chrono::year_month_weekday& ymwd) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year(), ymwd.month() + dm, ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator -(const gdut::chrono::year_month_weekday& ymwd,
                                                                      const gdut::chrono::years& dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year() - dy, ymwd.month(), ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday operator -(const gdut::chrono::year_month_weekday& ymwd,
                                                                      const gdut::chrono::months& dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday(ymwd.year(), ymwd.month() - dm, ymwd.weekday_indexed());
    }

    //*************************************************************************
    /// Equality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::year_month_weekday& lhs, 
                                            const gdut::chrono::year_month_weekday& rhs) GDUT_NOEXCEPT
    {
      return (lhs.year() == rhs.year())   && 
             (lhs.month() == rhs.month()) && 
             (lhs.weekday() == rhs.weekday());
    }

    //*************************************************************************
    /// Inequality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::year_month_weekday& lhs, 
                                            const gdut::chrono::year_month_weekday& rhs) GDUT_NOEXCEPT
    {
      return !(lhs == rhs);
    }

    //*************************************************************************
    /// year_month_weekday_last
    //*************************************************************************
    class year_month_weekday_last
    {
    public:

      //*************************************************************************
      /// Construct from year, month, weekday_last
      //*************************************************************************
      GDUT_CONSTEXPR14 year_month_weekday_last(const gdut::chrono::year&         y_, 
                                              const gdut::chrono::month&        m_, 
                                              const gdut::chrono::weekday_last& wdl_) GDUT_NOEXCEPT
        : y(y_)
        , m(m_)
        , wdl(wdl_)
      {
      }

      //*************************************************************************
      /// Returns the year.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::year year() const GDUT_NOEXCEPT
      {
        return y;
      }

      //*************************************************************************
      /// Returns the month.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::month month() const GDUT_NOEXCEPT
      {
        return m;
      }

      //*************************************************************************
      /// Returns the weekday.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday weekday() const GDUT_NOEXCEPT
      {
        return wdl.weekday();
      }

      //*************************************************************************
      /// Returns the weekday_last.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday_last weekday_last() const GDUT_NOEXCEPT
      {
        return wdl;
      }

      //*************************************************************************
      /// Adds gdut::chrono::years.
      //*************************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last& operator +=(const gdut::chrono::years& dy) GDUT_NOEXCEPT
      {
        y += dy;

        return *this;
      }

      //*************************************************************************
      /// Adds gdut::chrono::months.
      //*************************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last& operator +=(const gdut::chrono::months& dm) GDUT_NOEXCEPT
      {
        m += dm;

        return *this;
      }

      //*************************************************************************
      /// Subtracts gdut::chrono::years.
      //*************************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last& operator -=(const gdut::chrono::years& dy) GDUT_NOEXCEPT
      {
        y -= dy;

        return *this;
      }

      //*************************************************************************
      /// Subtracts gdut::chrono::months.
      //*************************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last& operator -=(const gdut::chrono::months& dm) GDUT_NOEXCEPT
      {
        m -= dm;

        return *this;
      }

      //*************************************************************************
      /// Converts to gdut::chrono::sys_days
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 operator gdut::chrono::sys_days() const GDUT_NOEXCEPT
      {
        // Get the last day of the month
        gdut::chrono::year_month_day_last ymdl(year(), gdut::chrono::month_day_last(month()));
        gdut::chrono::day last_day = ymdl.day();

        // Walk backward from the last day to find the last occurrence of the target weekday
        unsigned d = static_cast<unsigned>(last_day);
        
        for (; d >= 1; --d)
        {
          gdut::chrono::year_month_day ymd(year(), month(), gdut::chrono::day(d));
          gdut::chrono::sys_days ymd_sys_days = static_cast<gdut::chrono::sys_days>(ymd);
          gdut::chrono::weekday wd(static_cast<int>(ymd_sys_days.time_since_epoch().count()));
          
          if (wd == weekday())
          {
            return ymd_sys_days;
          }
        }

        // If not found (should not happen for valid input), return epoch
        return gdut::chrono::sys_days();
      }

      //*************************************************************************
      /// Converts to gdut::chrono::local_days
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 explicit operator gdut::chrono::local_days() const GDUT_NOEXCEPT
      {
        return local_days(sys_days(*this).time_since_epoch());
      }

    private:

      gdut::chrono::year  y;
      gdut::chrono::month m;
      gdut::chrono::weekday_last wdl;
    };

    //*************************************************************************
    /// Adds gdut::chrono::years and const gdut::chrono::year_month_weekday_last.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator +(const gdut::chrono::year_month_weekday_last& ymwdl,
                                                                           const gdut::chrono::years& dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year() + dy, ymwdl.month(), ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Adds gdut::chrono::years and const gdut::chrono::year_month_weekday_last.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator +(const gdut::chrono::years& dy,
                                                                           const gdut::chrono::year_month_weekday_last& ymwdl) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year() + dy, ymwdl.month(), ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Adds const gdut::chrono::year_month_weekday_last and gdut::chrono::months.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator +(const gdut::chrono::year_month_weekday_last& ymwdl,
                                                                           const gdut::chrono::months& dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year(), ymwdl.month() + dm, ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Adds gdut::chrono::months and const gdut::chrono::year_month_weekday_last.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator +(const gdut::chrono::months& dm,
                                                                           const gdut::chrono::year_month_weekday_last& ymwdl) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year(), ymwdl.month() + dm, ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::years from const gdut::chrono::year_month_weekday_last.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator -(const gdut::chrono::year_month_weekday_last& ymwdl,
                                                                           const gdut::chrono::years& dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year() - dy, ymwdl.month(), ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::months from const gdut::chrono::year_month_weekday_last
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last operator -(const gdut::chrono::year_month_weekday_last& ymwdl,
                                                                           const gdut::chrono::months& dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month_weekday_last(ymwdl.year(), ymwdl.month() - dm, ymwdl.weekday_last());
    }

    //*************************************************************************
    /// Equality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::year_month_weekday_last& lhs, 
                                            const gdut::chrono::year_month_weekday_last& rhs) GDUT_NOEXCEPT
    {
      return (lhs.year() == rhs.year())   &&
             (lhs.month() == rhs.month()) &&
             (lhs.weekday() == rhs.weekday());
    }

    //*************************************************************************
    /// Inequality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::year_month_weekday_last& lhs, 
                                            const gdut::chrono::year_month_weekday_last& rhs) GDUT_NOEXCEPT
    {
      return !(lhs == rhs);
    }
  }

  //*************************************************************************
  /// Hash function for gdut::chrono::year_month_weekday
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::year_month_weekday>
  {
    size_t operator()(const gdut::chrono::year_month_weekday& ymwd) const
    {
      gdut::chrono::year::rep  y = static_cast<gdut::chrono::year::rep>(static_cast<unsigned>(ymwd.year()));
      gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(static_cast<unsigned>(ymwd.month()));
      unsigned int  wd = ymwd.weekday().c_encoding();

      uint8_t buffer[sizeof(y) + sizeof(m) + sizeof(wd)];

      memcpy(buffer,                         &y, sizeof(y));
      memcpy(buffer + sizeof(y),             &m, sizeof(m));
      memcpy(buffer + sizeof(y) + sizeof(m), &wd, sizeof(wd));

      return gdut::private_hash::generic_hash<size_t>(buffer, buffer + sizeof(y) + sizeof(m) + sizeof(wd));
    }
  };
#endif

  //*************************************************************************
  /// Hash function for gdut::chrono::year_month_weekday_last
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::year_month_weekday_last>
  {
    size_t operator()(const gdut::chrono::year_month_weekday_last& ymwdl) const
    {
      gdut::chrono::year::rep  y = static_cast<gdut::chrono::year::rep>(static_cast<unsigned>(ymwdl.year()));
      gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(static_cast<unsigned>(ymwdl.month()));
      unsigned int  wd = ymwdl.weekday().c_encoding();

      uint8_t buffer[sizeof(y) + sizeof(m) + sizeof(wd)];

      memcpy(buffer,                         &y, sizeof(y));
      memcpy(buffer + sizeof(y),             &m, sizeof(m));
      memcpy(buffer + sizeof(y) + sizeof(m), &wd, sizeof(wd));

      return gdut::private_hash::generic_hash<size_t>(buffer, buffer + sizeof(y) + sizeof(m) + sizeof(wd));
    }
  };
#endif
}

