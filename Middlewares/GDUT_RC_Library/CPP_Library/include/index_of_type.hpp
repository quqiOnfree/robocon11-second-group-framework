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

#ifndef GDUT_INDEX_OF_TYPE_INCLUDED
#define GDUT_INDEX_OF_TYPE_INCLUDED

#include "platform.hpp"
#include "static_assert.hpp"
#include "integral_limits.hpp"

namespace gdut
{
#if GDUT_USING_CPP11

  //***************************************************************************
  /// Defines a no-position constant.
  //***************************************************************************
  static GDUT_CONSTANT size_t index_of_type_npos = gdut::integral_limits<size_t>::max;

  //***************************************************************************
  /// Finds the index of a type in a variadic type parameter.
  //***************************************************************************
  template <typename T, typename... TTypes>
  struct index_of_type;

  //***************************************************************************
  /// Finds the index of a type in a variadic type parameter.
  //***************************************************************************
  template <typename T, typename T1, typename... TRest>
  struct index_of_type<T, T1, TRest...> : public gdut::integral_constant<size_t, gdut::is_same<T, T1>::value ? 0 :
                                                                                             (gdut::index_of_type<T, TRest...>::value == gdut::index_of_type_npos ? gdut::index_of_type_npos : 
                                                                                              gdut::index_of_type<T, TRest...>::value + 1)>
  {
  };

  //***************************************************************************
  /// Finds the index of a type in a variadic type parameter.
  /// No types left.
  //***************************************************************************
  template <typename T>
  struct index_of_type<T> : public gdut::integral_constant<size_t, gdut::index_of_type_npos>
  {
  };

#if GDUT_USING_CPP17
  //***************************************************************************
  /// Finds the index of a type in a variadic type parameter.
  //***************************************************************************
  template <typename T, typename... TTypes>
  inline constexpr size_t index_of_type_v = gdut::index_of_type<T, TTypes...>::value;
#endif
#endif
}

#endif
