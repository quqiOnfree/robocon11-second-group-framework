///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2018 John Wellbelove

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

#ifndef GDUT_ABSOLUTE_INCLUDED
#define GDUT_ABSOLUTE_INCLUDED

#include "integral_limits.hpp"
#include "platform.hpp"
#include "type_traits.hpp"

namespace gdut {
//***************************************************************************
// For signed types.
//***************************************************************************
template <typename T>
GDUT_NODISCARD GDUT_CONSTEXPR
    typename gdut::enable_if<gdut::is_signed<T>::value, T>::type
    absolute(T value) GDUT_NOEXCEPT {
  return (value < T(0)) ? -value : value;
}

//***************************************************************************
// For unsigned types.
//***************************************************************************
template <typename T>
GDUT_NODISCARD GDUT_CONSTEXPR
    typename gdut::enable_if<gdut::is_unsigned<T>::value, T>::type
    absolute(T value) GDUT_NOEXCEPT {
  return value;
}

//***************************************************************************
// For signed types.
// Returns the result as the unsigned type.
//***************************************************************************
template <typename T>
GDUT_NODISCARD GDUT_CONSTEXPR
    typename gdut::enable_if<gdut::is_signed<T>::value,
                             typename gdut::make_unsigned<T>::type>::type
    absolute_unsigned(T value) GDUT_NOEXCEPT {
  typedef typename gdut::make_unsigned<T>::type TReturn;

  return (value == gdut::integral_limits<T>::min)
             ? (gdut::integral_limits<TReturn>::max / 2U) + 1U
         : (value < T(0)) ? TReturn(-value)
                          : TReturn(value);
}

//***************************************************************************
// For unsigned types.
// Returns the result as the unsigned type.
//***************************************************************************
template <typename T>
GDUT_NODISCARD GDUT_CONSTEXPR
    typename gdut::enable_if<gdut::is_unsigned<T>::value, T>::type
    absolute_unsigned(T value) GDUT_NOEXCEPT {
  return gdut::absolute(value);
}
} // namespace gdut

#endif
