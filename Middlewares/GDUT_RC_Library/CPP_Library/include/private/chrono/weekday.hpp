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

#include <string.h>

namespace gdut
{
  namespace chrono
  {
    class weekday;
    class weekday_indexed;
    class weekday_last;
    struct last_spec;

    GDUT_CONSTEXPR14 gdut::chrono::weekday operator +(const gdut::chrono::weekday& m, const gdut::chrono::days& ds)   GDUT_NOEXCEPT;
    GDUT_CONSTEXPR14 gdut::chrono::weekday operator +(const gdut::chrono::days& ds,   const gdut::chrono::weekday& m) GDUT_NOEXCEPT;
    GDUT_CONSTEXPR14 gdut::chrono::weekday operator -(const gdut::chrono::weekday& m, const gdut::chrono::days& ds)   GDUT_NOEXCEPT;

    //***********************************************************************
    /// weekday
    //***********************************************************************
    class weekday
    {
    public:

      //***********************************************************************
      /// Default constructor
      //***********************************************************************
      GDUT_CONSTEXPR weekday() GDUT_NOEXCEPT
        : value(255U)
      {
      }

      //***********************************************************************
      /// Construct from unsigned
      //***********************************************************************
      GDUT_CONSTEXPR explicit weekday(unsigned value_) GDUT_NOEXCEPT
        : value(value_ == 7U ? 0U :value_)
      {
      }

      //*************************************************************************
      /// Construct from sys_days.
      //*************************************************************************
      GDUT_CONSTEXPR14 weekday(const gdut::chrono::sys_days& sd) GDUT_NOEXCEPT
        : value(255U)
      {
        // Get number of days since epoch.
        gdut::chrono::days days_since_epoch = sd.time_since_epoch();

        // Convert to weekday. Beginning of the epoch was a Thursday (4).
        value = (days_since_epoch.count() + 4) % 7;
      }

      //*************************************************************************
      /// Construct from local_days.
      //*************************************************************************
      GDUT_CONSTEXPR14 weekday(const gdut::chrono::local_days& ld) GDUT_NOEXCEPT
        : value(255U)
      {
        weekday wd(sys_days(ld.time_since_epoch()));

        value = wd.c_encoding();
      }

      //***********************************************************************
      /// Copy constructor
      //***********************************************************************
      GDUT_CONSTEXPR14 weekday(const gdut::chrono::weekday& other) GDUT_NOEXCEPT
        : value(other.value)
      {
      }

      //***********************************************************************
      /// Assignment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday& operator =(const gdut::chrono::weekday& rhs) GDUT_NOEXCEPT
      {
        value = rhs.value;

        return *this;
      }

      //***********************************************************************
      /// Pre-increment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday& operator ++() GDUT_NOEXCEPT
      {
        *this += gdut::chrono::days(1);

        return *this;
      }

      //***********************************************************************
      /// Post-increment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday operator ++(int) GDUT_NOEXCEPT
      {
        const gdut::chrono::weekday temp = *this;
        
        *this += gdut::chrono::days(1);

        return temp;
      }

      //***********************************************************************
      /// Pre-decrement operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday& operator --() GDUT_NOEXCEPT
      {
        *this -= gdut::chrono::days(1);

        return *this;
      }

      //***********************************************************************
      /// Post-decrement operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday operator --(int) GDUT_NOEXCEPT
      {
        gdut::chrono::weekday temp = *this;

        *this -= gdut::chrono::days(1);

        return temp;
      }

      //***********************************************************************
      /// Plus-equals operator adding gdut::chrono::days
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday& operator +=(const gdut::chrono::days& ds) GDUT_NOEXCEPT
      {
        *this = *this + ds;

        return *this;
      }

      //***********************************************************************
      /// Minus-equals operator subtracting gdut::chrono::days
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday& operator -=(const gdut::chrono::days& ds) GDUT_NOEXCEPT
      {
        *this = *this - ds;

        return *this;
      }

      //***********************************************************************
      /// Returns <b>true</b> if the weekday is within the valid 1 to 31 range
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {
        return (c_encoding() <= 6U);
      }

      //***********************************************************************
      /// The minimum weekday value for which ok() will return <b>true</b>
      /// C encoding.
      //***********************************************************************
      GDUT_NODISCARD
      static GDUT_CONSTEXPR14 unsigned min() GDUT_NOEXCEPT
      {
        return 0;
      }

      //***********************************************************************
      /// The maximum weekday value for which ok() will return <b>true</b>
      /// C encoding.
      //***********************************************************************
      GDUT_NODISCARD
      static GDUT_CONSTEXPR14 unsigned max() GDUT_NOEXCEPT
      {
        return 6;
      }

      //***********************************************************************
      /// Get the C encoding of the weekday
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 unsigned c_encoding() const GDUT_NOEXCEPT
      {
        return value;
      }

      //***********************************************************************
      /// Get the ISO encoding of the weekday
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 unsigned iso_encoding() const GDUT_NOEXCEPT
      {
        return (value == 0U) ? 7U : value;
      }

      //***********************************************************************
      /// Index operator from index
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday_indexed operator[](unsigned index) const GDUT_NOEXCEPT;
        
      //***********************************************************************
      /// Index operator from gdut::chrono::last_spec
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 gdut::chrono::weekday_last operator[](gdut::chrono::last_spec last) const GDUT_NOEXCEPT;

      //***********************************************************************
      /// Returns <b>true</b> if the day is a weekend.
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_CONSTEXPR14 bool is_weekend() const GDUT_NOEXCEPT
      {
        return (c_encoding() == 0U) || (c_encoding() == 6U);
      }

    private:

      // The weekday value in C encoding.
      unsigned char value;
    };

    //***********************************************************************
    /// Equality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::weekday& wd1, const gdut::chrono::weekday& wd2) GDUT_NOEXCEPT
    {
      return (wd1.c_encoding() == wd2.c_encoding());
    }

    //***********************************************************************
    /// Inequality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::weekday& wd1, const gdut::chrono::weekday& wd2) GDUT_NOEXCEPT
    {
      return !(wd1 == wd2);
    }

    //***********************************************************************
    /// Add gdut::chrono::days to gdut::chrono::weekday
    ///\return gdut::chrono::weekday
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::weekday operator +(const gdut::chrono::weekday& wd, const gdut::chrono::days& ds) GDUT_NOEXCEPT
    {
      int delta = ds.count() % 7;

      unsigned int value = wd.c_encoding();

      // Adjust to allow a limited +-7 weekday delta
      value %= 7U;
      value += 7U;
      value += delta;
      value %= 7U;

      return gdut::chrono::weekday(value);
    }

    //***********************************************************************
    /// Add gdut::chrono::weekday to gdut::chrono::days
    ///\return gdut::chrono::weekday
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::weekday operator +(const gdut::chrono::days& ds, const gdut::chrono::weekday& wd) GDUT_NOEXCEPT
    {
      return wd + ds;
    }

    //***********************************************************************
    /// Subtract gdut::chrono::days from gdut::chrono::weekday
    ///\return gdut::chrono::weekday
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::weekday operator -(const gdut::chrono::weekday& m, const gdut::chrono::days& ds) GDUT_NOEXCEPT
    {
      return m + gdut::chrono::days(-ds.count());
    }

    //***********************************************************************
    /// Subtract gdut::chrono::weekday from gdut::chrono::weekday
    ///\return gdut::chrono::days
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::days operator -(const gdut::chrono::weekday& wd1, const gdut::chrono::weekday& wd2) GDUT_NOEXCEPT
    {
      if (wd1.ok() && wd2.ok())
      {
        int diff = static_cast<int>(wd1.c_encoding()) - static_cast<int>(wd2.c_encoding());
        
        return gdut::chrono::days((diff + 7) % 7);
      }

      return gdut::chrono::days(0);
    }

#if GDUT_USING_CPP17
    inline constexpr gdut::chrono::weekday Sunday{ 0U };
    inline constexpr gdut::chrono::weekday Monday{ 1U };
    inline constexpr gdut::chrono::weekday Tuesday{ 2U };
    inline constexpr gdut::chrono::weekday Wednesday{ 3U };
    inline constexpr gdut::chrono::weekday Thursday{ 4U };
    inline constexpr gdut::chrono::weekday Friday{ 5U };
    inline constexpr gdut::chrono::weekday Saturday{ 6U };
#else
    static GDUT_CONSTANT gdut::chrono::weekday Sunday{ 0U };
    static GDUT_CONSTANT gdut::chrono::weekday Monday{ 1U };
    static GDUT_CONSTANT gdut::chrono::weekday Tuesday{ 2U };
    static GDUT_CONSTANT gdut::chrono::weekday Wednesday{ 3U };
    static GDUT_CONSTANT gdut::chrono::weekday Thursday{ 4U };
    static GDUT_CONSTANT gdut::chrono::weekday Friday{ 5U };
    static GDUT_CONSTANT gdut::chrono::weekday Saturday{ 6U };
#endif

    //***********************************************************************
    /// weekday_indexed
    //***********************************************************************
    class weekday_indexed
    {
    public:

      //***********************************************************************
      /// Default constructor
      //***********************************************************************
      GDUT_CONSTEXPR weekday_indexed() GDUT_NOEXCEPT
        : wd()
        , i()
      {
      }

      //***********************************************************************
      /// Construct from weekday and index
      //***********************************************************************
      GDUT_CONSTEXPR14 weekday_indexed(const gdut::chrono::weekday& wd_, unsigned index_) GDUT_NOEXCEPT
        : wd(wd_)
        , i(static_cast<uint_least8_t>(index_))
      {
      }

      //***********************************************************************
      /// Copy constructor
      //***********************************************************************
      GDUT_CONSTEXPR14 weekday_indexed(const gdut::chrono::weekday_indexed& other) GDUT_NOEXCEPT
        : wd(other.wd)
        , i(other.i)
      {
      }

      //***********************************************************************
      /// Assignment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday_indexed& operator =(const gdut::chrono::weekday_indexed& rhs) GDUT_NOEXCEPT
      {
        wd = rhs.wd;
        i  = rhs.i;

        return *this;
      }

      //***********************************************************************
      /// Get weekday
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::chrono::weekday weekday() const GDUT_NOEXCEPT
      {
        return wd;
      }

      //***********************************************************************
      /// Get index
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_NODISCARD GDUT_CONSTEXPR14 unsigned index() const GDUT_NOEXCEPT
      {
        return i;
      }

      //***********************************************************************
      /// Returns <b>true</b> if the weekday and index are valid
      //***********************************************************************
      GDUT_NODISCARD
      GDUT_NODISCARD GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {
        return wd.ok() && (i >= 1U) && (i <= 5U);
      }

    private:

      gdut::chrono::weekday wd;
      uint_least8_t i;
    };

    //***********************************************************************
    /// Equality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::weekday_indexed& wd1, const gdut::chrono::weekday_indexed& wd2) GDUT_NOEXCEPT
    {
      return (wd1.weekday() == wd2.weekday()) && 
             (wd1.index()   == wd2.index());
    }

    //***********************************************************************
    /// Inequality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator !=(const gdut::chrono::weekday_indexed& wd1, const gdut::chrono::weekday_indexed& wd2) GDUT_NOEXCEPT
    {
      return !(wd1 == wd2);
    }

    //***********************************************************************
    /// weekday_last
    //***********************************************************************
    class weekday_last
    {
    public:

      //***********************************************************************
      /// Construct from unsigned
      //***********************************************************************
      GDUT_CONSTEXPR14 explicit weekday_last(const gdut::chrono::weekday& wd_) GDUT_NOEXCEPT
        : wd(wd_)
      {
      }

      //***********************************************************************
      /// Copy constructor
      //***********************************************************************
      GDUT_CONSTEXPR14 weekday_last(const gdut::chrono::weekday_last& other) GDUT_NOEXCEPT
        : wd(other.wd)
      {
      }

      //***********************************************************************
      /// Assignment operator
      //***********************************************************************
      GDUT_CONSTEXPR14 gdut::chrono::weekday_last& operator =(const gdut::chrono::weekday_last& rhs) GDUT_NOEXCEPT
      {
        wd = rhs.wd;

        return *this;
      }

      //***********************************************************************
      /// Get weekday
      //***********************************************************************
      GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::chrono::weekday weekday() const GDUT_NOEXCEPT
      {
        return wd;
      }

      //***********************************************************************
      /// Returns <b>true</b> if the weekday is valid
      //***********************************************************************
      GDUT_NODISCARD GDUT_CONSTEXPR14 bool ok() const GDUT_NOEXCEPT
      {
        return wd.ok();
      }

    private:

      gdut::chrono::weekday wd;
    };

    //***********************************************************************
    /// Equality operator
    //***********************************************************************
    inline GDUT_CONSTEXPR14 bool operator ==(const gdut::chrono::weekday_last& wd1, const gdut::chrono::weekday_last& wd2) GDUT_NOEXCEPT
    {
      return (wd1.weekday() == wd2.weekday());
    }

    //***********************************************************************
    /// weekday index operator from index
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::weekday_indexed gdut::chrono::weekday::operator[](unsigned index) const GDUT_NOEXCEPT
    {
      return gdut::chrono::weekday_indexed(*this, index);
    }

    //***********************************************************************
    /// Index operator from gdut::chrono::last_spec
    //***********************************************************************
    inline GDUT_CONSTEXPR14 gdut::chrono::weekday_last gdut::chrono::weekday::operator[](gdut::chrono::last_spec) const GDUT_NOEXCEPT
    {
      return gdut::chrono::weekday_last(*this);
    }
  }

  //*************************************************************************
  /// Hash function for gdut::chrono::weekday
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::weekday>
  {
    size_t operator()(const gdut::chrono::weekday& wd) const
    {
      unsigned value = wd.c_encoding();
      const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);

      return gdut::private_hash::generic_hash<size_t>(p, p + sizeof(unsigned));
    }
  };
#endif

  //*************************************************************************
  /// Hash function for gdut::chrono::weekday_indexed
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::weekday_indexed>
  {
    size_t operator()(const gdut::chrono::weekday_indexed& wdi) const
    {
      unsigned int a = wdi.weekday().c_encoding();
      unsigned int b = wdi.index();

      uint8_t buffer[sizeof(a) + sizeof(b)];

      memcpy(buffer,             &a, sizeof(a));
      memcpy(buffer + sizeof(a), &b, sizeof(b));

      return gdut::private_hash::generic_hash<size_t>(buffer, buffer + sizeof(a) + sizeof(b));
    }
  };
#endif

  //*************************************************************************
  /// Hash function for gdut::chrono::weekday_last
  //*************************************************************************
#if GDUT_USING_8BIT_TYPES
  template <>
  struct hash<gdut::chrono::weekday_last>
  {
    size_t operator()(const gdut::chrono::weekday_last& wdl) const
    {
      return gdut::hash<gdut::chrono::weekday>()(wdl.weekday());
    }
  };
#endif
}
