///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2025 BMW AG

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

#ifndef GDUT_CONCEPTS_INCLUDED
#define GDUT_CONCEPTS_INCLUDED

#include "platform.hpp"

#include "utility.hpp"
#include "type_traits.hpp"

#if GDUT_NOT_USING_CPP20 && !defined(GDUT_IN_UNIT_TEST)
  #error NOT SUPPORTED FOR BELOW C++20
#endif

#if GDUT_USING_CPP20

#if GDUT_USING_STL
  #include <concepts>
#endif

namespace gdut
{

#if GDUT_USING_STL

  using std::same_as;
  using std::derived_from;
  using std::convertible_to;
  using std::common_reference_with;
  using std::common_with;
  using std::integral;
  using std::signed_integral;
  using std::unsigned_integral;
  using std::floating_point;
  using std::assignable_from;

#else // not GDUT_USING_STL

  namespace private_concepts
  {
    template <typename T, typename U>
    concept same_as_helper = gdut::is_same_v<T, U>;
  }

  //***************************************************************************
  template <typename T, typename U>
  concept same_as = private_concepts::same_as_helper<T, U> && private_concepts::same_as_helper<U, T>;

  //***************************************************************************
  template <typename Derived, typename Base>
  concept derived_from =
    gdut::is_base_of_v<Base, Derived> &&
    gdut::is_convertible_v<const volatile Derived*, const volatile Base*>;

  //***************************************************************************
  template <typename From, typename To>
  concept convertible_to =
    gdut::is_convertible_v<From, To> &&
    requires {
      static_cast<To>(gdut::declval<From>());
    };

  //***************************************************************************
  template< class T, typename U >
  concept common_reference_with =
    gdut::same_as<gdut::common_reference_t<T, U>, gdut::common_reference_t<U, T>> &&
    gdut::convertible_to<T, gdut::common_reference_t<T, U>> &&
    gdut::convertible_to<U, gdut::common_reference_t<T, U>>;

  //***************************************************************************
  template <typename T, typename U>
  concept common_with =
    gdut::same_as<gdut::common_type_t<T, U>, gdut::common_type_t<U, T>> &&
    requires {
        static_cast<gdut::common_type_t<T, U>>(gdut::declval<T>());
        static_cast<gdut::common_type_t<T, U>>(gdut::declval<U>());
    } &&
    gdut::common_reference_with<
        gdut::add_lvalue_reference_t<const T>,
        gdut::add_lvalue_reference_t<const U>> &&
    gdut::common_reference_with<
        gdut::add_lvalue_reference_t<gdut::common_type_t<T, U>>,
        gdut::common_reference_t<
            gdut::add_lvalue_reference_t<const T>,
            gdut::add_lvalue_reference_t<const U>>>;

  //***************************************************************************
  template <typename T>
  concept integral = gdut::is_integral_v<T>;

  //***************************************************************************
  template <typename T>
  concept signed_integral = gdut::integral<T> && gdut::is_signed_v<T>;

  //***************************************************************************
  template <typename T>
  concept unsigned_integral = gdut::integral<T> && !gdut::signed_integral<T>;

  //***************************************************************************
  template <typename T>
  concept floating_point = gdut::is_floating_point_v<T>;

  //***************************************************************************
  template <typename LHS, typename RHS>
  concept assignable_from =
    gdut::is_lvalue_reference_v<LHS> &&
    gdut::common_reference_with<
        const gdut::remove_reference_t<LHS>&,
        const gdut::remove_reference_t<RHS>&> &&
    requires(LHS lhs, RHS&& rhs) {
        { lhs = gdut::forward<RHS>(rhs) } -> gdut::same_as<LHS>;
    };

#endif
}
#endif
#endif
