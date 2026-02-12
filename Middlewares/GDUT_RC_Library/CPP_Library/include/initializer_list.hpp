///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Documentation: https://www.etlcpp.com/initializer_list.html

Copyright(c) 2022 John Wellbelove

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

#ifndef GDUT_INITIALIZER_LIST_INCLUDED
#define GDUT_INITIALIZER_LIST_INCLUDED

#include "platform.hpp"

#if GDUT_HAS_INITIALIZER_LIST

#if (GDUT_USING_CPP11 && !defined(GDUT_NO_INITIALIZER_LIST))

#include <stddef.h>

// Use the compiler's std::initializer_list?
#if (GDUT_USING_STL && GDUT_NOT_USING_STLPORT &&                               \
     !defined(GDUT_FORCE_GDUT_INITIALIZER_LIST)) ||                            \
    defined(GDUT_IN_UNIT_TEST) || defined(GDUT_FORCE_STD_INITIALIZER_LIST)

#include <initializer_list>

#else

// Use the ETL's std::initializer_list
namespace std {
#if defined(GDUT_COMPILER_MICROSOFT)

///**************************************************************************
/// A definition of initializer_list that is compatible with the Microsoft
/// compiler
///**************************************************************************
template <typename T> class initializer_list {
public:
  using value_type = T;
  using reference = const T &;
  using const_reference = const T &;
  using size_type = size_t;
  using iterator = const T *;
  using const_iterator = const T *;

  //*************************************************************************
  /// Default constructor
  //*************************************************************************
  constexpr initializer_list() GDUT_NOEXCEPT : pfirst(nullptr),
                                               plast(nullptr) {}

  //*************************************************************************
  /// Constructor
  //*************************************************************************
  constexpr initializer_list(const T *pfirst_, const T *plast_) GDUT_NOEXCEPT
      : pfirst(pfirst_),
        plast(plast_) {}

  //*************************************************************************
  /// Get the beginning of the list.
  //*************************************************************************
  constexpr const T *begin() const GDUT_NOEXCEPT { return pfirst; }

  //*************************************************************************
  /// Get the end of the list.
  //*************************************************************************
  constexpr const T *end() const GDUT_NOEXCEPT { return plast; }

  //*************************************************************************
  /// Get the size of the list.
  //*************************************************************************
  constexpr size_t size() const GDUT_NOEXCEPT {
    return static_cast<size_t>(plast - pfirst);
  }

private:
  const T *pfirst;
  const T *plast;
};

//*************************************************************************
/// Get the beginning of the list.
//*************************************************************************
template <typename T>
constexpr const T *begin(initializer_list<T> init) GDUT_NOEXCEPT {
  return init.begin();
}

//*************************************************************************
/// Get the end of the list.
//*************************************************************************
template <typename T>
constexpr const T *end(initializer_list<T> init) GDUT_NOEXCEPT {
  return init.end();
}

#elif defined(GDUT_COMPILER_GCC) || defined(GDUT_COMPILER_CLANG) ||            \
    defined(GDUT_COMPILER_ARM6) || defined(GDUT_COMPILER_ARM7) ||              \
    defined(GDUT_COMPILER_IAR) || defined(GDUT_COMPILER_TEXAS_INSTRUMENTS) ||  \
    defined(GDUT_COMPILER_INTEL)

///**************************************************************************
/// A definition of initializer_list that is compatible with Clang, GCC and
/// other compilers.
///**************************************************************************
template <class T> class initializer_list {
public:
  using value_type = T;
  using reference = const T &;
  using const_reference = const T &;
  using size_type = size_t;
  using iterator = const T *;
  using const_iterator = const T *;

  //*************************************************************************
  /// Default constructor
  //*************************************************************************
  constexpr initializer_list() GDUT_NOEXCEPT : pfirst(nullptr), length(0) {}

  //*************************************************************************
  /// Get the beginning of the list.
  //*************************************************************************
  constexpr const T *begin() const GDUT_NOEXCEPT { return pfirst; }

  //*************************************************************************
  /// Get the end of the list.
  //*************************************************************************
  constexpr const T *end() const GDUT_NOEXCEPT { return pfirst + length; }

  //*************************************************************************
  /// Get the size of the list.
  //*************************************************************************
  constexpr size_t size() const GDUT_NOEXCEPT { return length; }

private:
  //*************************************************************************
  /// Constructor
  //*************************************************************************
  constexpr initializer_list(const T *pfirst_, size_t length_) GDUT_NOEXCEPT
      : pfirst(pfirst_),
        length(length_) {}

  const T *pfirst;
  size_t length;
};

//*************************************************************************
/// Get the beginning of the list.
//*************************************************************************
template <class T>
constexpr const T *begin(initializer_list<T> init) GDUT_NOEXCEPT {
  return init.begin();
}

//*************************************************************************
/// Get the end of the list.
//*************************************************************************
template <class T>
constexpr const T *end(initializer_list<T> init) GDUT_NOEXCEPT {
  return init.end();
}
#else

#error No definition for initializer_list is currently available for your compiler. Visit https://github.com/ETLCPP/etl/issues to request support.

#endif // Compiler tests
} // namespace std

#endif // (GDUT_USING_STL && GDUT_NOT_USING_STLPORT &&
       // !defined(GDUT_FORCE_GDUT_INITIALIZER_LIST)) ||
       // defined(GDUT_IN_UNIT_TEST) || defined(GDUT_FORCE_STD_INITIALIZER_LIST)
#endif // GDUT_USING_CPP11 && !defined(GDUT_NO_INITIALIZER_LIST)
#endif // GDUT_HAS_INITIALIZER_LIST
#endif // GDUT_INITIALIZER_LIST_INCLUDED
