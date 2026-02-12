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

namespace gdut
{
  namespace chrono
  {
    class year_month
    {
    public:

      //*************************************************************************
      /// Default constructor.
      //*************************************************************************
      GDUT_CONSTEXPR year_month()
        : y()
        , m()
      {
      }

      //*************************************************************************
      /// Construct from month and day.
      //*************************************************************************
      GDUT_CONSTEXPR14 year_month(const gdut::chrono::year&  y_, 
                                 const gdut::chrono::month& m_) GDUT_NOEXCEPT
        : y(y_)
        , m(m_)
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
      /// Returns true if the month/day is valid.
      //*************************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {       
        return y.ok() && m.ok();
      }

      //***********************************************************************
      /// Compare year_month with another.
      /// if month < other.month, returns -1
      /// else if month > other.month, returns 1
      /// else if day < other.day, returns -1
      /// else if day > other.day, returns 1
      /// else returns 0
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 int compare(const year_month& other) const GDUT_NOEXCEPT 
      {
        if (y < other.y) return -1;
        if (y > other.y) return 1;
        if (m < other.m) return -1;
        if (m > other.m) return 1;
      
        return 0;
      }

    private:

      gdut::chrono::year  y;
      gdut::chrono::month m;
    };

    //*************************************************************************
    /// Adds gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator +(const gdut::chrono::year_month& ym,
                                                              const gdut::chrono::years&      dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year() + dy, ym.month());
    }

    //*************************************************************************
    /// Adds gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator +(const gdut::chrono::years&      dy,
                                                              const gdut::chrono::year_month& ym) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year() + dy, ym.month());
    }

    //*************************************************************************
    /// Adds gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator +(const gdut::chrono::year_month& ym,
                                                              const gdut::chrono::months&     dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year(), ym.month() + dm);
    }

    //*************************************************************************
    /// Adds gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator +(const gdut::chrono::months& dm,
                                                              const gdut::chrono::year_month& ym) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year(), ym.month() + dm);
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::years
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator -(const gdut::chrono::year_month& ym,
                                                              const gdut::chrono::years&      dy) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year() - dy, ym.month());
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::months
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year_month operator -(const gdut::chrono::year_month& ym,
                                                              const gdut::chrono::months&     dm) GDUT_NOEXCEPT
    {
      return gdut::chrono::year_month(ym.year(), ym.month() - dm);
    }

    //*************************************************************************
    /// Subtracts gdut::chrono::year_month
    //*************************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::months operator -(const gdut::chrono::year_month& ym1,
                                                          const gdut::chrono::year_month& ym2) GDUT_NOEXCEPT
    {
      return gdut::chrono::months(static_cast<int>(((int(ym1.year()) - int(ym2.year())) * 12) + (unsigned(ym1.month()) - unsigned(ym2.month()))));
    }

    //*************************************************************************
    /// Equality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::year_month& lhs, 
                                            const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      return (lhs.year() == rhs.year()) && (lhs.month() == rhs.month());
    }

    //*************************************************************************
    /// Equality operator.
    //*************************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::year_month& lhs, 
                                            const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      return !(lhs == rhs);
    }

    //*************************************************************************
    /// Less-than operator.
    //*************************************************************************
    GDUT_NODISCARD GDUT_CONSTEXPR14
    inline bool operator <(const gdut::chrono::year_month& lhs,
                           const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      if (lhs.year() < rhs.year())
      {
        return true;
      }
      else if (lhs.year() == rhs.year())
      {
        return lhs.month() < rhs.month();
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    /// Less-than-equal operator.
    //*************************************************************************
    GDUT_NODISCARD GDUT_CONSTEXPR14
    inline bool operator <=(const gdut::chrono::year_month& lhs,
                            const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      return !(rhs < lhs);
    }

    //*************************************************************************
    /// Greater-than operator.
    //*************************************************************************
    GDUT_NODISCARD GDUT_CONSTEXPR14
    inline bool operator >(const gdut::chrono::year_month& lhs,
                           const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      return rhs < lhs;
    }

    //*************************************************************************
    /// Greater-than-equal operator.
    //*************************************************************************
    GDUT_NODISCARD GDUT_CONSTEXPR14
    inline bool operator >=(const gdut::chrono::year_month& lhs,
                            const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      return !(lhs < rhs);
    }

    //***********************************************************************
    /// Spaceship operator
    //***********************************************************************
#if GDUT_USING_CPP20
    [[nodiscard]] inline constexpr auto operator <=>(const gdut::chrono::year_month& lhs, 
                                                     const gdut::chrono::year_month& rhs) GDUT_NOEXCEPT
    {
      auto cmp = lhs.year()<=> rhs.year();

      if (cmp != 0)
      {
        return cmp;
      }
      else
      {
        return lhs.month() <=> rhs.month();
      }
    }
#endif
  }

  //*************************************************************************
  /// Hash function for gdut::chrono::year_month
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::year_month>
  {
    size_t operator()(const gdut::chrono::year_month& ym) const
    {    
      gdut::chrono::year::rep  y = static_cast<gdut::chrono::year::rep>(static_cast<unsigned>(ym.year()));
      gdut::chrono::month::rep m = static_cast<gdut::chrono::month::rep>(static_cast<unsigned>(ym.month()));

      uint8_t buffer[sizeof(y) + sizeof(m)];

      memcpy(buffer,             &y, sizeof(y));
      memcpy(buffer + sizeof(y), &m, sizeof(m));

      return gdut::private_hash::generic_hash<size_t>(buffer, buffer + sizeof(y) + sizeof(m));
    }
  };
#endif
}

