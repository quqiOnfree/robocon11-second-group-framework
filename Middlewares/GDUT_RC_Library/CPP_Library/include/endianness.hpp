///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2014 John Wellbelove

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

#ifndef GDUT_ENDIAN_INCLUDED
#define GDUT_ENDIAN_INCLUDED

#include "platform.hpp"
#include "enum_type.hpp"
#include "binary.hpp"

#include <stdint.h>

#if GDUT_USING_CPP20 && GDUT_USING_STL
  #include <bit>
#endif

///\defgroup endian endian
/// Constants & utilities for endianness
///\ingroup utilities

// Have we not already defined GDUT_ENDIAN_NATIVE?
#if !defined(GDUT_ENDIAN_NATIVE)
  // Can we use the C++20 definitions?
  #if GDUT_USING_CPP20 && GDUT_USING_STL
    #define GDUT_ENDIAN_LITTLE static_cast<int>(std::endian::little)
    #define GDUT_ENDIAN_BIG    static_cast<int>(std::endian::big)
    #define GDUT_ENDIAN_NATIVE static_cast<int>(std::endian::native)
  // Is this the IAR compiler?
  #elif defined(GDUT_COMPILER_IAR) && defined(__LITTLE_ENDIAN__)
    #define GDUT_ENDIAN_LITTLE 0
    #define GDUT_ENDIAN_BIG    1
    #if __LITTLE_ENDIAN__ == 1
      #define GDUT_ENDIAN_NATIVE GDUT_ENDIAN_LITTLE
    #elif __LITTLE_ENDIAN__ == 0
      #define GDUT_ENDIAN_NATIVE GDUT_ENDIAN_BIG
    #endif
  // If not can we use the compiler macros?
  #elif defined(__BYTE_ORDER__)
    // Test the two versions of the macro we are likely to see.
    #if defined(__ORDER_LITTLE_ENDIAN__)
      #define GDUT_ENDIAN_LITTLE __ORDER_LITTLE_ENDIAN__
      #define GDUT_ENDIAN_BIG    __ORDER_BIG_ENDIAN__
      #define GDUT_ENDIAN_NATIVE __BYTE_ORDER__
    #elif defined(__LITTLE_ENDIAN__)
      #define GDUT_ENDIAN_LITTLE __LITTLE_ENDIAN__
      #define GDUT_ENDIAN_BIG    __BIG_ENDIAN__
      #define GDUT_ENDIAN_NATIVE __BYTE_ORDER__
    #endif
  #else
    // The user needs to define GDUT_ENDIAN_NATIVE.
    #error Unable to determine native endianness at compile time. GDUT_ENDIAN_NATIVE must be defined either as 0 for 'little endian' or 1 for 'big endian'.
  #endif
#else
  // Default values for little and big endianness.
  #define GDUT_ENDIAN_LITTLE 0
  #define GDUT_ENDIAN_BIG    1
#endif

// If true, then the endianness of the platform can be constexpr.
#if (GDUT_USING_CPP11 && defined(GDUT_ENDIAN_NATIVE))
  #define GDUT_HAS_CONSTEXPR_ENDIANNESS 1
#else
  #define GDUT_HAS_CONSTEXPR_ENDIANNESS 0
#endif

namespace gdut
{
  //***************************************************************************
  /// Constants to denote endianness of operations.
  ///\ingroup endian
  //***************************************************************************
  struct endian
  {
    enum enum_type
    {
      little  = GDUT_ENDIAN_LITTLE,
      big     = GDUT_ENDIAN_BIG,
      native  = GDUT_ENDIAN_NATIVE
    };

    GDUT_DECLARE_ENUM_TYPE(endian, int)
    GDUT_ENUM_TYPE(little, "little")
    GDUT_ENUM_TYPE(big,    "big")
    GDUT_END_ENUM_TYPE
  };

  //***************************************************************************
  /// Checks the endianness of the platform.
  ///\ingroup endian
  //***************************************************************************
  struct endianness
  {
    gdut::endian operator ()() const
    {
      return gdut::endian(*this);
    }

#if GDUT_HAS_CONSTEXPR_ENDIANNESS
    GDUT_CONSTEXPR
#endif
    operator gdut::endian() const
    {
      return get();
    }

#if GDUT_HAS_CONSTEXPR_ENDIANNESS
    static GDUT_CONSTEXPR gdut::endian value()
#else
    static gdut::endian value()
#endif
    {
      return get();
    }

  private:

#if GDUT_HAS_CONSTEXPR_ENDIANNESS
    static GDUT_CONSTEXPR gdut::endian get()
    {
      return gdut::endian::native;
    }
#else
    static gdut::endian get()
    {
      static const uint32_t i = 0xFFFF0000;

      return (*reinterpret_cast<const unsigned char*>(&i) == 0) ? gdut::endian::little : gdut::endian::big;
    }
#endif
  };

  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
    typename gdut::enable_if<gdut::is_integral<T>::value, T>::type
    ntoh(T value)
  {
    if (endianness::value() == endian::little)
    {
      return gdut::reverse_bytes(value);
    }
    else
    {
      return value;
    }
  }

  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
    typename gdut::enable_if<gdut::is_integral<T>::value, T>::type
    hton(T value)
  {
    if (endianness::value() == endian::little)
    {
      return gdut::reverse_bytes(value);
    }
    else
    {
      return value;
    }
  }
}

#endif
