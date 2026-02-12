/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef GDUT_PARAMETER_PACK
#define GDUT_PARAMETER_PACK

#include "platform.hpp"
#include "type_traits.hpp"

#include <stdint.h>

#if GDUT_CPP11_NOT_SUPPORTED
  #if !defined(GDUT_IN_UNIT_TEST)
    #error NOT SUPPORTED FOR C++03 OR BELOW
  #endif
#else
namespace gdut
{
  //***************************************************************************
  /// parameter_pack
  //***************************************************************************
  template <typename... TTypes>
  class parameter_pack
  {
  public:

    static constexpr size_t size = sizeof...(TTypes);

    //***************************************************************************
    /// index_of_type
    //***************************************************************************
    template <typename T>
    class index_of_type
    {
    private:

      //***********************************
      template <typename Type, typename T1, typename... TRest>
      struct index_of_type_helper
      {
        static constexpr size_t value = gdut::is_same<Type, T1>::value ? 1 : 1 + index_of_type_helper<Type, TRest...>::value;
      };

      //***********************************
      template <typename Type, typename T1>
      struct index_of_type_helper<Type, T1>
      {
        static constexpr size_t value = 1;
      };

    public:

      static_assert(gdut::is_one_of<T, TTypes...>::value, "T is not in parameter pack");

      /// The index value.
      static constexpr size_t value = index_of_type_helper<T, TTypes...>::value - 1;
    };

#if GDUT_USING_CPP17
    template <typename T>
    static constexpr size_t index_of_type_v = index_of_type<T>::value;
#endif

    //***************************************************************************
    /// type_from_index
    //***************************************************************************
    template <size_t Index>
    class type_from_index
    {
    private:

      //***********************************
      template <size_t Desired_Index, size_t Current_Index, typename T1, typename... TRest>
      struct type_from_index_helper
      {
        using type = typename gdut::conditional<Desired_Index == Current_Index, T1, typename type_from_index_helper<Desired_Index, Current_Index + 1, TRest...>::type>::type;
      };

      //***********************************
      template <size_t Desired_Index, size_t Current_Index, typename T1>
      struct type_from_index_helper<Desired_Index, Current_Index, T1>
      {
        using type = T1;
      };

    public:

      static_assert(Index < sizeof...(TTypes), "Index out of bounds of parameter pack");

      /// Template alias
      using type = typename type_from_index_helper<Index, 0, TTypes...>::type;
    };

    //***********************************
    template <size_t Index>
    using type_from_index_t = typename type_from_index<Index>::type;
  };

  //***********************************
  template <size_t Index, typename... TTypes>
  using parameter_pack_t = typename gdut::parameter_pack<TTypes...>::template type_from_index_t<Index>;

  //***********************************
  template <typename... TTypes>
  constexpr size_t parameter_pack<TTypes...>::size;

#if GDUT_USING_CPP17
  template <typename T, typename... TTypes>
  inline constexpr size_t parameter_pack_v = gdut::parameter_pack<TTypes...>::template index_of_type<T>::value;
#endif

#if GDUT_USING_CPP17 && !GDUT_USING_GCC_COMPILER && !GDUT_USING_CLANG_COMPILER
  //***********************************
  template <typename... TTypes>
  template <typename T>
  constexpr size_t parameter_pack<TTypes...>::template index_of_type<T>::value;
#else
  //***********************************
  template <typename... TTypes>
  template <typename T>
  constexpr size_t parameter_pack<TTypes...>::index_of_type<T>::value;
#endif
}
#endif
#endif
