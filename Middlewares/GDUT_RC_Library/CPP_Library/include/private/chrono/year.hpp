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
    //***********************************************************************
    /// year
    //***********************************************************************
    class year
    {
    public:

      using rep = int16_t;

      //***********************************************************************
      /// Default constructor
      //***********************************************************************
      GDUT_CONSTEXPR year() GDUT_NOEXCEPT
        : value(0)
      {
      }

      //***********************************************************************
      /// Construct from unsigned
      //***********************************************************************
      GDUT_CONSTEXPR explicit year(unsigned value_) GDUT_NOEXCEPT
        : value(value_)
      {
      }

      //***********************************************************************
      /// Copy constructor
      //***********************************************************************
      GDUT_CONSTEXPR14 year(const gdut::chrono::year& other) GDUT_NOEXCEPT
        : value(other.value)
      {
      }

      //***********************************************************************
      /// Assignment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year& operator =(const gdut::chrono::year& rhs) GDUT_NOEXCEPT
      {
        value = rhs.value;

        return *this;
      }

      //***********************************************************************
      /// Pre-increment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year& operator ++() GDUT_NOEXCEPT
      {
        ++value;

        return *this;
      }

      //***********************************************************************
      /// Post-increment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year operator ++(int) GDUT_NOEXCEPT
      {
        const gdut::chrono::year temp = *this;
        ++value;

        return temp;
      }

      //***********************************************************************
      /// Pre-decrement operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year& operator --() GDUT_NOEXCEPT
      {
        --value;

        return *this;
      }

      //***********************************************************************
      /// Post-decrement operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year operator --(int) GDUT_NOEXCEPT
      {
        const gdut::chrono::year temp = *this;
        --value;

        return temp;
      }

      //***********************************************************************
      /// Plus-equals operator adding gdut::chrono::years
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year& operator +=(const gdut::chrono::years& ys) GDUT_NOEXCEPT
      {
        value += static_cast<unsigned char>(ys.count());

        return *this;
      }

      //***********************************************************************
      /// Minus-equals operator subtracting gdut::chrono::years
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::year& operator -=(const gdut::chrono::years& ys) GDUT_NOEXCEPT
      {
        value -= static_cast<unsigned char>(ys.count());

        return *this;
      }

      //***********************************************************************
      /// Returns <b>true</b> if the year is within the valid -32767 to 32767 range
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {
        return (value != gdut::integral_limits<int16_t>::min);
      }

      //***********************************************************************
      /// The minimum year value for which ok() will return <b>true</b>
      //***********************************************************************
      GDUT_NODISCARD
      static GDUT_CONSTEXPR14 gdut::chrono::year min() GDUT_NOEXCEPT
      {
        return gdut::chrono::year(-32767);
      }

      //***********************************************************************
      /// The maximum year value for which ok() will return <b>true</b>
      //***********************************************************************
      GDUT_NODISCARD
      static GDUT_CONSTEXPR14 gdut::chrono::year max() GDUT_NOEXCEPT
      {
        return gdut::chrono::year(32767);
      }

      //***********************************************************************
      /// Returns <b>true</b> if the year is a leap year
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool is_leap() const GDUT_NOEXCEPT
      {
        return ((value % 4) == 0) &&    // Divisible by 4
               (((value % 100) != 0) || // but not divisible by 100
                ((value % 400) == 0));  // unless divisible by 400
      }

      //***********************************************************************
      /// Conversion operator to unsigned int
      //***********************************************************************
      GDUT_CONSTEXPR14 operator int() const GDUT_NOEXCEPT
      {
        return static_cast<int>(value);
      }

      //***********************************************************************
      /// Compare year with another.
      /// if year < other, returns -1
      /// else if year > other, returns 1
      /// else returns 0
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 int compare(const year& other) const GDUT_NOEXCEPT 
      {
        if (value < other.value) return -1;
        if (value > other.value) return 1;

        return 0;
      }

    private:

      rep value;
    };

    //***********************************************************************
    /// Equality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) == static_cast<unsigned>(y2));
    }

    //***********************************************************************
    /// Inequality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return !(y1 == y2);
    }

    //***********************************************************************
    /// Less-than operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator <(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) < static_cast<unsigned>(y2));
    }

    //***********************************************************************
    /// Less-than-or-equal operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator <=(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) <= static_cast<unsigned>(y2));
    }

    //***********************************************************************
    /// Greater-than operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator >(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) > static_cast<unsigned>(y2));
    }

    //***********************************************************************
    /// Greater-than-or-equal operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator >=(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) >= static_cast<unsigned>(y2));
    }

    //***********************************************************************
    /// Spaceship operator
    //***********************************************************************
#if GDUT_USING_CPP20
    [[nodiscard]] inline constexpr auto operator <=>(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return (static_cast<unsigned>(y1) <=> static_cast<unsigned>(y2));
    }
#endif

    //***********************************************************************
    /// Add gdut::chrono::years to gdut::chrono::year
    ///\return gdut::chrono::year
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year operator +(const gdut::chrono::year& y, const gdut::chrono::years& ys) GDUT_NOEXCEPT
    {
      gdut::chrono::year result(y);

      result += ys;

      return result;
    }

    //***********************************************************************
    /// Add gdut::chrono::year to gdut::chrono::years
    ///\return gdut::chrono::year
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year operator +(const gdut::chrono::years& ys, const gdut::chrono::year& y) GDUT_NOEXCEPT
    {
      gdut::chrono::year result(y);

      result += ys;

      return result;
    }

    //***********************************************************************
    /// Subtract gdut::chrono::years from gdut::chrono::year
    ///\return gdut::chrono::year
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year operator -(const gdut::chrono::year& y, const gdut::chrono::years& ys) GDUT_NOEXCEPT
    {
      gdut::chrono::year result(y);

      result -= ys;

      return result;
    }

    //***********************************************************************
    /// Subtract gdut::chrono::year from gdut::chrono::years
    ///\return gdut::chrono::years
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::year operator -(const gdut::chrono::years& ys, const gdut::chrono::year& y) GDUT_NOEXCEPT
    {
      gdut::chrono::year result(y);

      result -= ys;

      return result;
    }

    //***********************************************************************
    /// Subtract gdut::chrono::year from gdut::chrono::year
    ///\return gdut::chrono::years
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::years operator -(const gdut::chrono::year& y1, const gdut::chrono::year& y2) GDUT_NOEXCEPT
    {
      return gdut::chrono::years(static_cast<int>(static_cast<unsigned>(y1)) - 
                                static_cast<int>(static_cast<unsigned>(y2)));
    }
  }

  //*************************************************************************
  /// Hash function for gdut::chrono::year
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::year>
  {
    size_t operator()(const gdut::chrono::year& y) const
    {
      gdut::chrono::year::rep value = static_cast<gdut::chrono::year::rep>(static_cast<unsigned>(y));
      const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);

      return gdut::private_hash::generic_hash<size_t>(p, p + sizeof(value));
    }
  };
#endif
}

#if GDUT_HAS_CHRONO_LITERALS_YEAR
namespace gdut
{
  inline namespace literals
  {
    inline namespace chrono_literals
    {
      //***********************************************************************
      /// Literal for years
      //***********************************************************************
#if GDUT_USING_VERBOSE_CHRONO_LITERALS
      inline GDUT_CONSTEXPR14 gdut::chrono::year operator ""_year(unsigned long long y) GDUT_NOEXCEPT
#else
      inline GDUT_CONSTEXPR14 gdut::chrono::year operator ""_y(unsigned long long y) GDUT_NOEXCEPT
#endif
      {
        return gdut::chrono::year(static_cast<int16_t>(y));
      }
    }
  }
}
#endif
