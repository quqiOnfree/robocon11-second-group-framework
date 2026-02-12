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

#ifndef GDUT_TUPLE_INCLUDED
#define GDUT_TUPLE_INCLUDED

#include "platform.hpp"

#if GDUT_NOT_USING_CPP11 && !defined(GDUT_IN_UNIT_TEST)
#error NOT SUPPORTED FOR C++03 OR BELOW
#endif

#if GDUT_USING_CPP11

#if GDUT_USING_STL
#include <tuple>
#endif

#include "functional.hpp"
#include "nth_type.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

#include "private/tuple_element.hpp"
#include "private/tuple_size.hpp"

namespace gdut {
//***************************************************************************
/// A tuple type
/// gdut::tuple<TTypes...>
//***************************************************************************
template <typename... TTypes> class tuple;

//***************************************************************************
/// Type trait to check if a type is an gdut::tuple
/// Default implementation.
//***************************************************************************
template <typename T> struct is_tuple : gdut::false_type {};

//***************************************************************************
/// Type trait to check if a type is an gdut::tuple
/// Specialisation for gdut::tuple
//***************************************************************************
template <typename... TTypes>
struct is_tuple<gdut::tuple<TTypes...>> : gdut::true_type {};

namespace private_tuple {
//***************************************************************************
/// Get the base of a tuple type whose head type is T.
//***************************************************************************
template <typename T, typename TTuple> struct tuple_type_base;

// Specialisation for an empty tuple
template <typename T> struct tuple_type_base<T, tuple<>> {
  using type = void;
};

// Recursive definition of the type.
template <typename T, typename THead, typename... TTail>
struct tuple_type_base<T, tuple<THead, TTail...>> {
  using type =
      gdut::conditional_t<gdut::is_same<T, THead>::value,
                          tuple<THead, TTail...>,
                          typename tuple_type_base<T, tuple<TTail...>>::type>;
};

// Get the base of a tuple type whose head type is T.
template <typename T, typename TTuple>
using tuple_type_base_t = typename tuple_type_base<T, TTuple>::type;

//***************************************************************************
/// ignore
/// An object of unspecified type such that any value can be assigned to it with
/// no effect. Intended for use with gdut::tie when unpacking a gdut::tuple, as
/// a placeholder for the arguments that are not used.
/// https://en.cppreference.com/w/cpp/utility/tuple/ignore
//***************************************************************************
struct ignore_t {
  template <typename T>
  GDUT_CONSTEXPR ignore_t operator=(T &&) const GDUT_NOEXCEPT {
    return *this;
  }
};
} // namespace private_tuple

//***************************************************************************
/// Empty tuple
//***************************************************************************
template <> class tuple<> {
public:
  using value_type = void;   ///< The type contained by this tuple.
  using this_type = tuple<>; ///< The type of this tuple.
  using base_type = void;    ///< The type of the base tuple.
  using index_sequence_type =
      gdut::make_index_sequence<0>; ///< The index_sequence type for this tuple.

  //*********************************
  // No-op copy_assignment for the base case
  //*********************************
  GDUT_CONSTEXPR14
  void copy_assignment(const this_type & /*other*/) {}

  //*********************************
  // No-op forward_assignment for the base case
  //*********************************
  GDUT_CONSTEXPR14
  void forward_assignment(this_type && /*other*/) {}

  //*********************************
  // No-op swap for the base case
  //*********************************
  GDUT_CONSTEXPR14
  void swap(this_type & /*other*/) {}

  //*********************************
  // Returns the size of the base case.
  // Always zero.
  //*********************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR
  static size_t size() { return 0U; }
};

//***************************************************************************
/// Tuple
//***************************************************************************
template <typename THead, typename... TTail>
class tuple<THead, TTail...> : public tuple<TTail...> {
private:
  //*********************************
  /// Helper function to calculate the number
  /// of types from a type list.
  //*********************************
  template <typename... UTypes> static constexpr size_t number_of_types() {
    return sizeof...(UTypes);
  }

public:
  //*********************************
  /// Friends
  //*********************************
  template <typename... UTypes> friend class tuple;

  template <size_t Index, typename... TTypes>
  GDUT_CONSTEXPR14 friend gdut::tuple_element_t<Index, gdut::tuple<TTypes...>> &
  get(tuple<TTypes...> &);

  template <size_t Index, typename... TTypes>
  GDUT_CONSTEXPR14 friend gdut::tuple_element_t<Index,
                                                gdut::tuple<TTypes...>> &&
  get(tuple<TTypes...> &&);

  template <size_t Index, typename... TTypes>
  GDUT_CONSTEXPR14 friend const gdut::tuple_element_t<Index,
                                                      gdut::tuple<TTypes...>> &
  get(const tuple<TTypes...> &);

  template <size_t Index, typename... TTypes>
  GDUT_CONSTEXPR14 friend const gdut::tuple_element_t<Index,
                                                      gdut::tuple<TTypes...>> &&
  get(const tuple<TTypes...> &&);

  template <typename T, typename... TTypes>
  GDUT_CONSTEXPR14 friend T &get(tuple<TTypes...> &);

  template <typename T, typename... TTypes>
  GDUT_CONSTEXPR14 friend T &&get(tuple<TTypes...> &&);

  template <typename T, typename... TTypes>
  GDUT_CONSTEXPR14 friend const T &get(const tuple<TTypes...> &);

  template <typename T, typename... TTypes>
  GDUT_CONSTEXPR14 friend const T &&get(const tuple<TTypes...> &&);

  //*********************************
  /// Types
  //*********************************
  using value_type = THead; ///< The type contained by this tuple.
  using this_type = tuple<THead, TTail...>; ///< The type of this tuple.
  using base_type = tuple<TTail...>;        ///< The type of the base tuple.
  using index_sequence_type = gdut::make_index_sequence<number_of_types<
      THead, TTail...>()>; ///< The index_sequence type for this tuple.

  //*********************************
  /// Default constructor.
  //*********************************
  GDUT_CONSTEXPR14
  tuple() : value() {}

  //*********************************
  /// Copy constructor.
  //*********************************
  GDUT_CONSTEXPR14
  tuple(const tuple<THead, TTail...> &other) = default;

  //*********************************
  /// Move constructor.
  //*********************************
  GDUT_CONSTEXPR14
  tuple(tuple<THead, TTail...> &&other) = default;

  //*********************************
  /// Copy assignment.
  //*********************************
  GDUT_CONSTEXPR14
  tuple &operator=(const tuple<THead, TTail...> &other) = default;

  //*********************************
  /// Move assignment.
  //*********************************
  GDUT_CONSTEXPR14
  tuple &operator=(tuple<THead, TTail...> &&other) = default;

  //*********************************
  /// Copy construct from lvalue reference tuple type.
  /// Implicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 tuple(tuple<UHead, UTail...> &other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Copy construct from lvalue reference tuple type.
  /// Explicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  !gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(tuple<UHead, UTail...> &other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Copy construct from const lvalue reference tuple type.
  /// Implicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 tuple(const tuple<UHead, UTail...> &other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Copy construct from const lvalue reference tuple type.
  /// Explicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  !gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(const tuple<UHead, UTail...> &other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Move construct from rvalue reference tuple type.
  /// Implicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 tuple(tuple<UHead, UTail...> &&other)
      : base_type(gdut::forward<tuple<UTail...>>(other.get_base())),
        value(gdut::forward<UHead>(other.get_value())) {}

  //*********************************
  /// Move construct from rvalue reference tuple type.
  /// Explicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  !gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(tuple<UHead, UTail...> &&other)
      : base_type(gdut::forward<tuple<UTail...>>(other.get_base())),
        value(gdut::forward<UHead>(other.get_value())) {}

  //*********************************
  /// Construct from const rvalue reference tuple type.
  /// Implicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 tuple(const tuple<UHead, UTail...> &&other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Construct from const rvalue reference tuple type.
  /// Explicit conversion
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()) &&
                                  (number_of_types<THead, TTail...>() >= 1U) &&
                                  !gdut::is_convertible<UHead, THead>::value,
                              int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(const tuple<UHead, UTail...> &&other)
      : base_type(other.get_base()), value(other.get_value()) {}

  //*********************************
  /// Construct from arguments.
  //*********************************
  GDUT_CONSTEXPR14
  tuple(const THead &head, const TTail &...tail)
      : base_type(tail...), value(head) {}

  //*********************************
  /// Construct from arguments.
  /// Implicit conversion.
  //*********************************
  template <
      typename UHead, typename... UTail,
      gdut::enable_if_t<!is_tuple<gdut::remove_reference_t<UHead>>::value &&
                            (number_of_types<THead, TTail...>() ==
                             number_of_types<UHead, UTail...>()) &&
                            (number_of_types<THead, TTail...>() >= 1U) &&
                            gdut::is_convertible<UHead, THead>::value,
                        int> = 0>
  GDUT_CONSTEXPR14 tuple(UHead &&head, UTail &&...tail) GDUT_NOEXCEPT
      : base_type(gdut::forward<UTail>(tail)...),
        value(gdut::forward<UHead>(head)) {}

  //*********************************
  /// Construct from arguments.
  /// explicit conversion.
  //*********************************
  template <
      typename UHead, typename... UTail,
      gdut::enable_if_t<!is_tuple<gdut::remove_reference_t<UHead>>::value &&
                            (number_of_types<THead, TTail...>() ==
                             number_of_types<UHead, UTail...>()) &&
                            (number_of_types<THead, TTail...>() >= 1U) &&
                            !gdut::is_convertible<UHead, THead>::value,
                        int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(UHead &&head, UTail &&...tail) GDUT_NOEXCEPT
      : base_type(gdut::forward<UTail>(tail)...),
        value(gdut::forward<UHead>(head)) {}

  //*********************************
  /// Construct from lvalue reference pair.
  /// Implicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<
          number_of_types<THead, TTail...>() == 2U &&
              gdut::is_convertible<U1, THead>::value &&
              gdut::is_convertible<U2, typename base_type::value_type>::value,
          int> = 0>
  GDUT_CONSTEXPR14 tuple(GDUT_OR_STD::pair<U1, U2> &p) GDUT_NOEXCEPT
      : base_type(p.second),
        value(p.first) {}

  //*********************************
  /// Construct from lvalue reference pair.
  /// Explicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<number_of_types<THead, TTail...>() == 2U &&
                            (!gdut::is_convertible<U1, THead>::value ||
                             !gdut::is_convertible<
                                 U2, typename base_type::value_type>::value),
                        int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(GDUT_OR_STD::pair<U1, U2> &p) GDUT_NOEXCEPT
      : base_type(p.second),
        value(p.first) {}

  //*********************************
  /// Construct from const lvalue reference pair.
  /// Implicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<
          number_of_types<THead, TTail...>() == 2U &&
              gdut::is_convertible<U1, THead>::value &&
              gdut::is_convertible<U2, typename base_type::value_type>::value,
          int> = 0>
  GDUT_CONSTEXPR14 tuple(const GDUT_OR_STD::pair<U1, U2> &p) GDUT_NOEXCEPT
      : base_type(p.second),
        value(p.first) {}

  //*********************************
  /// Construct from const lvalue reference pair.
  /// Explicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<number_of_types<THead, TTail...>() == 2U &&
                            (!gdut::is_convertible<U1, THead>::value ||
                             !gdut::is_convertible<
                                 U2, typename base_type::value_type>::value),
                        int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(const GDUT_OR_STD::pair<U1, U2> &p)
      GDUT_NOEXCEPT : base_type(p.second),
                      value(p.first) {}

  //*********************************
  /// Construct from rvalue reference pair.
  /// Implicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<
          number_of_types<THead, TTail...>() == 2U &&
              gdut::is_convertible<U1, THead>::value &&
              gdut::is_convertible<U2, typename base_type::value_type>::value,
          int> = 0>
  GDUT_CONSTEXPR14 tuple(GDUT_OR_STD::pair<U1, U2> &&p) GDUT_NOEXCEPT
      : base_type(gdut::forward<U2>(p.second)),
        value(gdut::forward<U1>(p.first)) {}

  //*********************************
  /// Construct from rvalue reference pair.
  /// Explicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<number_of_types<THead, TTail...>() == 2U &&
                            (!gdut::is_convertible<U1, THead>::value ||
                             !gdut::is_convertible<
                                 U2, typename base_type::value_type>::value),
                        int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(GDUT_OR_STD::pair<U1, U2> &&p) GDUT_NOEXCEPT
      : base_type(gdut::forward<U2>(p.second)),
        value(gdut::forward<U1>(p.first)) {}

  //*********************************
  /// Construct from const rvalue reference pair.
  /// Implicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<
          number_of_types<THead, TTail...>() == 2U &&
              gdut::is_convertible<U1, THead>::value &&
              gdut::is_convertible<U2, typename base_type::value_type>::value,
          int> = 0>
  GDUT_CONSTEXPR14 tuple(const GDUT_OR_STD::pair<U1, U2> &&p) GDUT_NOEXCEPT
      : base_type(gdut::forward<U2>(p.second)),
        value(gdut::forward<U1>(p.first)) {}

  //*********************************
  /// Construct from const rvalue reference pair.
  /// Explicit conversion.
  //*********************************
  template <
      typename U1, typename U2,
      gdut::enable_if_t<number_of_types<THead, TTail...>() == 2U &&
                            (!gdut::is_convertible<U1, THead>::value ||
                             !gdut::is_convertible<
                                 U2, typename base_type::value_type>::value),
                        int> = 0>
  GDUT_CONSTEXPR14 explicit tuple(const GDUT_OR_STD::pair<U1, U2> &&p)
      GDUT_NOEXCEPT : base_type(p.second),
                      value(p.first) {}

  //*********************************
  /// Copy assign from other tuple type.
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()),
                              int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(const tuple<UHead, UTail...> &other) {
    copy_assignment(other);

    return *this;
  }

  //*********************************
  /// Move assign from other tuple type.
  //*********************************
  template <typename UHead, typename... UTail,
            gdut::enable_if_t<(number_of_types<THead, TTail...>() ==
                               number_of_types<UHead, UTail...>()),
                              int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(tuple<UHead, UTail...> &&other) {
    forward_assignment(gdut::forward<tuple<UHead, UTail...>>(other));

    return *this;
  }

  //*********************************
  /// Assign from lvalue pair tuple type.
  //*********************************
  template <typename U1, typename U2,
            size_t NTypes = number_of_types<THead, TTail...>,
            gdut::enable_if_t<NTypes == 2U, int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(pair<U1, U2> &p) {
    get_value() = p.first;
    get_base().get_value() = p.second;

    return *this;
  }

  //*********************************
  /// Assign from const lvalue pair tuple type.
  //*********************************
  template <typename U1, typename U2,
            size_t NTypes = number_of_types<THead, TTail...>,
            gdut::enable_if_t<NTypes == 2U, int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(const pair<U1, U2> &p) {
    get_value() = p.first;
    get_base().get_value() = p.second;

    return *this;
  }

  //*********************************
  /// Assign from rvalue pair tuple type.
  //*********************************
  template <typename U1, typename U2,
            size_t NTypes = number_of_types<THead, TTail...>,
            gdut::enable_if_t<NTypes == 2U, int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(pair<U1, U2> &&p) {
    get_value() = gdut::forward<U1>(p.first);
    get_base().get_value() = gdut::forward<U2>(p.second);

    return *this;
  }

  //*********************************
  /// Assign from const rvalue pair tuple type.
  //*********************************
  template <typename U1, typename U2,
            size_t NTypes = number_of_types<THead, TTail...>,
            gdut::enable_if_t<NTypes == 2U, int> = 0>
  GDUT_CONSTEXPR14 tuple &operator=(const pair<U1, U2> &&p) {
    get_value() = gdut::forward<U1>(p.first);
    get_base().get_value() = gdut::forward<U2>(p.second);

    return *this;
  }

  //*********************************
  /// Swaps this tuple with another.
  //*********************************
  GDUT_CONSTEXPR14
  void swap(this_type &other) {
    using GDUT_OR_STD::swap;

    // Swap the head
    swap(get_value(), other.get_value());

    auto &this_base = get_base();
    auto &other_base = other.get_base();

    // Recursively swap the tail by calling the base class's swap
    // implementation.
    this_base.swap(other_base);
  }

  //*********************************
  /// Returns the number of elements in the tuple.
  //*********************************
  GDUT_NODISCARD
  constexpr static size_t size() { return number_of_types<THead, TTail...>(); }

protected:
  //*********************************
  /// Returns a reference to the head value.
  //*********************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  THead &get_value() { return value; }

  //*********************************
  /// Returns a const reference to the head value.
  //*********************************
  GDUT_CONSTEXPR
  const THead &get_value() const { return value; }

  //*********************************
  /// Get a reference to the base class.
  //*********************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  base_type &get_base() { return static_cast<base_type &>(*this); }

  //*********************************
  /// Get a const reference to the base class.
  //*********************************
  GDUT_NODISCARD
  GDUT_CONSTEXPR14
  const base_type &get_base() const {
    return static_cast<const base_type &>(*this);
  }

  //*********************************
  /// Handles copy assignment from another tuple.
  //*********************************
  template <typename UHead, typename... UTail>
  GDUT_CONSTEXPR14 void copy_assignment(const tuple<UHead, UTail...> &other) {
    // Assign the head
    this->value = other.get_value();

    // Get the base classes
    auto &this_base = get_base();
    const auto &other_base = other.get_base();

    // Recursively assign the tail by calling the base class's assignment
    // implementation
    this_base.copy_assignment(other_base);
  }

  //*********************************
  /// Handles move assignment from another tuple.
  //*********************************
  template <typename UHead, typename... UTail>
  GDUT_CONSTEXPR14 void forward_assignment(tuple<UHead, UTail...> &&other) {
    // Assign the head
    this->value = gdut::forward<UHead>(other.get_value());

    auto &this_base = get_base();
    auto &&other_base = other.get_base();

    // Recursively assign the tail by calling the base class's move assignment
    // implementation
    this_base.forward_assignment(gdut::forward<tuple<UTail...>>(other_base));
  }

private:
  THead value;
};

#if GDUT_USING_CPP17
//***************************************************************************
/// Template deduction guideline from variadic arguments.
//***************************************************************************
template <typename... TArgs> tuple(TArgs... args) -> tuple<TArgs...>;

//***************************************************************************
/// Template deduction guideline from pair.
//***************************************************************************
template <typename T1, typename T2>
tuple(GDUT_OR_STD::pair<T1, T2>) -> tuple<T1, T2>;
#endif

//***************************************************************************
/// Gets the element type at the index in the tuple.
//***************************************************************************
template <size_t Index, typename... TTypes>
struct tuple_element<Index, gdut::tuple<TTypes...>> {
  using type = gdut::nth_type_t<Index, TTypes...>;
};

//***************************************************************************
/// Gets the size of the tuple.
//***************************************************************************
template <typename... TTypes>
struct tuple_size<gdut::tuple<TTypes...>>
    : gdut::integral_constant<size_t, sizeof...(TTypes)> {};

//***************************************************************************
/// Gets the common type of a tuple.
//***************************************************************************
template <typename... Types> struct common_type<gdut::tuple<Types...>> {
  using type = gdut::common_type_t<Types...>;
};

//***************************************************************************
/// Extracts the element at Index from the tuple.
/// Index must be an integer value in sizeof...(TTypes)).
/// Returns a reference.
//***************************************************************************
template <size_t Index, typename... TTypes>
GDUT_NODISCARD
    GDUT_CONSTEXPR14 gdut::tuple_element_t<Index, gdut::tuple<TTypes...>> &
    get(tuple<TTypes...> &t) {
  GDUT_STATIC_ASSERT(Index < sizeof...(TTypes),
                     "gdut::get<Index> - Index out of range");

  // Get the type at this index.
  using tuple_type = gdut::nth_base_t<Index, tuple<TTypes...>> &;

  // Cast the tuple to the selected type and get the value.
  return static_cast<tuple_type>(t).get_value();
}

//***************************************************************************
/// Extracts the element at Index from the tuple.
/// Index must be an integer value in [?0?, sizeof...(TTypes)).
/// Returns a const reference.
//***************************************************************************
template <size_t Index, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 const
    gdut::tuple_element_t<Index, gdut::tuple<TTypes...>> &
    get(const tuple<TTypes...> &t) {
  GDUT_STATIC_ASSERT(Index < sizeof...(TTypes),
                     "gdut::get<Index> - Index out of range");

  // Get the type at this index.
  using tuple_type = const gdut::nth_base_t<Index, tuple<TTypes...>> &;

  // Cast the tuple to the selected type and get the value.
  return static_cast<tuple_type>(t).get_value();
}

//***************************************************************************
/// Extracts the element at Index from the tuple.
/// Index must be an integer value in [?0?, sizeof...(TTypes)).
/// Returns an rvalue reference.
//***************************************************************************
template <size_t Index, typename... TTypes>
GDUT_NODISCARD
    GDUT_CONSTEXPR14 gdut::tuple_element_t<Index, gdut::tuple<TTypes...>> &&
    get(tuple<TTypes...> &&t) {
  GDUT_STATIC_ASSERT(Index < sizeof...(TTypes),
                     "gdut::get<Index> - Index out of range");

  // Get the type at this index.
  using tuple_type = gdut::nth_base_t<Index, tuple<TTypes...>> &&;

  // Cast the tuple to the selected type and get the value.
  return gdut::move(static_cast<tuple_type>(t).get_value());
}

//***************************************************************************
/// Extracts the element at Index from the tuple.
/// Index must be an integer value in [?0?, sizeof...(TTypes)).
/// Returns a const rvalue reference.
//***************************************************************************
template <size_t Index, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 const
    gdut::tuple_element_t<Index, gdut::tuple<TTypes...>> &&
    get(const tuple<TTypes...> &&t) {
  GDUT_STATIC_ASSERT(Index < sizeof...(TTypes),
                     "gdut::get<Index> - Index out of range");

  // Get the type at this index.
  using tuple_type = const gdut::nth_base_t<Index, gdut::tuple<TTypes...>> &&;

  // Cast the tuple to the selected type and get the value.
  return gdut::move(static_cast<tuple_type>(t).get_value());
}

//***************************************************************************
/// Extracts the element with type T from the tuple.
/// Static asserts if the tuple contain more than one T, or does not contain a T
/// element. Returns a reference.
//***************************************************************************
template <typename T, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 T &get(tuple<TTypes...> &t) {
  GDUT_STATIC_ASSERT(
      !(gdut::has_duplicates_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple contains duplicate instances of T");
  GDUT_STATIC_ASSERT(
      (gdut::is_one_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple does not contain the specified type");

  // Get the tuple base type that contains a T
  using tuple_type =
      gdut::private_tuple::tuple_type_base_t<T, tuple<TTypes...>> &;

  // Cast the tuple to the selected type and get the value.
  return static_cast<tuple_type>(t).get_value();
}

//***************************************************************************
/// Extracts the element with type T from the tuple.
/// Static asserts if the tuple contain more than one T, or does not contain a T
/// element. Returns a const reference.
//***************************************************************************
template <typename T, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 const T &get(const tuple<TTypes...> &t) {
  GDUT_STATIC_ASSERT(
      !(gdut::has_duplicates_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple contains duplicate instances of T");
  GDUT_STATIC_ASSERT(
      (gdut::is_one_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple does not contain the specified type");

  // Get the tuple base type that contains a T
  using tuple_type =
      const gdut::private_tuple::tuple_type_base_t<T, tuple<TTypes...>> &;

  // Cast the tuple to the selected type and get the value.
  return static_cast<tuple_type>(t).get_value();
}

//***************************************************************************
/// Extracts the element with type T from the tuple.
/// Static asserts if the tuple contain more than one T, or does not contain a T
/// element. Returns an rvalue reference.
//***************************************************************************
template <typename T, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 T &&get(tuple<TTypes...> &&t) {
  GDUT_STATIC_ASSERT(
      !(gdut::has_duplicates_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple contains duplicate instances of T");
  GDUT_STATIC_ASSERT(
      (gdut::is_one_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple does not contain the specified type");

  // Get the tuple base type that contains a T
  using tuple_type =
      gdut::private_tuple::tuple_type_base_t<T, tuple<TTypes...>> &&;

  // Cast the tuple to the selected type and get the value.
  return gdut::move(static_cast<tuple_type>(t).get_value());
}

//***************************************************************************
/// Extracts the element with type T from the tuple.
/// Static asserts if the tuple contain more than one T, or does not contain a T
/// element. Returns a const rvalue reference.
//***************************************************************************
template <typename T, typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 const T &&get(const tuple<TTypes...> &&t) {
  GDUT_STATIC_ASSERT(
      !(gdut::has_duplicates_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple contains duplicate instances of T");
  GDUT_STATIC_ASSERT(
      (gdut::is_one_of<T, TTypes...>::value),
      "gdut::get<Type> - Tuple does not contain the specified type");

  // Get the tuple base type that contains a T
  using tuple_type =
      const gdut::private_tuple::tuple_type_base_t<T, tuple<TTypes...>> &&;

  // Cast the tuple to the selected type and get the value.
  return gdut::move(static_cast<tuple_type>(t).get_value());
}

#if GDUT_USING_CPP17
inline constexpr private_tuple::ignore_t ignore;
#else
static constexpr private_tuple::ignore_t ignore;
#endif

//***************************************************************************
/// Creates a tuple of references to the provided arguments.
//***************************************************************************
template <typename... TTypes>
GDUT_CONSTEXPR gdut::tuple<TTypes &...> tie(TTypes &...args) {
  return {args...};
}

//***************************************************************************
// Creates a tuple from the provided arguments.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::tuple<gdut::unwrap_ref_decay_t<TTypes>...>
make_tuple(TTypes &&...args) {
  return gdut::tuple<unwrap_ref_decay_t<TTypes>...>(
      gdut::forward<TTypes>(args)...);
}

//***************************************************************************
/// Creates a new tuple by selecting elements from another, given a run time
/// index sequence. Static asserts if the number of indices does not match the
/// tuple size.
//***************************************************************************
template <typename TTuple, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
select_from_tuple(TTuple &&tuple, gdut::index_sequence<Indices...>)
    -> gdut::tuple<gdut::tuple_element_t<Indices, gdut::decay_t<TTuple>>...> {
  GDUT_STATIC_ASSERT(sizeof...(Indices) <=
                         gdut::tuple_size<gdut::decay_t<TTuple>>::value,
                     "Number of indices is greater than the tuple size");

  return gdut::make_tuple(
      gdut::forward<gdut::tuple_element_t<Indices, gdut::decay_t<TTuple>>>(
          gdut::get<Indices>(gdut::forward<TTuple>(tuple)))...);
}

//***************************************************************************
/// Creates a new tuple by selecting elements from another, given a template
/// parameter index sequence. Static asserts if the number of indices does not
/// match the tuple size.
//***************************************************************************
template <size_t... Indices, typename TTuple>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto select_from_tuple(TTuple &&tuple)
    -> gdut::tuple<gdut::tuple_element_t<Indices, gdut::decay_t<TTuple>>...> {
  return select_from_tuple(gdut::forward<TTuple>(tuple),
                           gdut::index_sequence<Indices...>{});
}

//***************************************************************************
/// Forwards the arguments as a tuple.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 gdut::tuple<TTypes &&...>
forward_as_tuple(TTypes &&...args) {
  return tuple<TTypes &&...>(gdut::forward<TTypes>(args)...);
}

namespace private_tuple {
//**********************************
// Helper to concatenate two tuples
//**********************************
template <typename Tuple1, typename Tuple2, size_t... Index1, size_t... Index2>
GDUT_CONSTEXPR14 auto
tuple_cat_impl(Tuple1 &&t1, gdut::index_sequence<Index1...>, Tuple2 &&t2,
               gdut::index_sequence<Index2...>)
    -> gdut::tuple<gdut::tuple_element_t<Index1, gdut::decay_t<Tuple1>>...,
                   gdut::tuple_element_t<Index2, gdut::decay_t<Tuple2>>...> {
  return gdut::tuple<gdut::tuple_element_t<Index1, gdut::decay_t<Tuple1>>...,
                     gdut::tuple_element_t<Index2, gdut::decay_t<Tuple2>>...>(
      gdut::get<Index1>(gdut::forward<Tuple1>(t1))...,
      gdut::get<Index2>(gdut::forward<Tuple2>(t2))...);
}
} // namespace private_tuple

//***************************************************************************
/// Base case for concatenating one tuple
//***************************************************************************
template <typename Tuple>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto tuple_cat(Tuple &&t) -> Tuple {
  return gdut::forward<Tuple>(t);
}

//***************************************************************************
/// Recursive case for concatenating multiple tuples
//***************************************************************************
template <typename Tuple1, typename Tuple2, typename... Tuples>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto tuple_cat(Tuple1 &&t1, Tuple2 &&t2,
                                               Tuples &&...ts)
    -> decltype(private_tuple::tuple_cat_impl(
        gdut::forward<Tuple1>(t1),
        gdut::make_index_sequence<
            gdut::tuple_size<gdut::decay_t<Tuple1>>::value>{},
        gdut::forward<Tuple2>(t2),
        gdut::make_index_sequence<
            gdut::tuple_size<gdut::decay_t<Tuple2>>::value>{})) {
  auto concatenated = private_tuple::tuple_cat_impl(
      gdut::forward<Tuple1>(t1),
      gdut::make_index_sequence<
          gdut::tuple_size<gdut::decay_t<Tuple1>>::value>{},
      gdut::forward<Tuple2>(t2),
      gdut::make_index_sequence<
          gdut::tuple_size<gdut::decay_t<Tuple2>>::value>{});

  return tuple_cat(gdut::move(concatenated), gdut::forward<Tuples>(ts)...);
}

#if GDUT_USING_STL
//***************************************************************************
// Tuple conversion functions.
// From ETL to STL
//***************************************************************************
namespace private_tuple {
///*********************************
template <typename TEtl_Tuple, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_std_impl(const TEtl_Tuple &etl_tuple, gdut::index_sequence<Indices...>)
    -> std::tuple<typename gdut::tuple_element_t<Indices, TEtl_Tuple>...> {
  return std::tuple<gdut::tuple_element_t<Indices, TEtl_Tuple>...>(
      gdut::get<Indices>(etl_tuple)...);
}

///*********************************
template <typename TEtl_Tuple, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_std_impl(TEtl_Tuple &&etl_tuple, gdut::index_sequence<Indices...>)
    -> std::tuple<gdut::tuple_element_t<Indices, TEtl_Tuple>...> {
  return std::tuple<gdut::tuple_element_t<Indices, TEtl_Tuple>...>(
      gdut::move(gdut::get<Indices>(etl_tuple))...);
}
} // namespace private_tuple

//***************************************************************************
/// Converts an gdut::tuple to a std::tuple.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_std(const gdut::tuple<TTypes...> &etl_tuple)
    -> std::tuple<gdut::decay_t<TTypes>...> {
  return private_tuple::to_std_impl(etl_tuple,
                                    gdut::make_index_sequence_for<TTypes...>());
}

//***************************************************************************
/// Converts an gdut::tuple to a std::tuple.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto to_std(gdut::tuple<TTypes...> &&etl_tuple)
    -> std::tuple<gdut::decay_t<TTypes>...> {
  return private_tuple::to_std_impl(gdut::move(etl_tuple),
                                    gdut::make_index_sequence_for<TTypes...>());
}

//***************************************************************************
/// Tuple conversion functions.
/// From STL to ETL
//***************************************************************************
namespace private_tuple {
///*********************************
template <typename TStd_Tuple, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_etl_impl(const TStd_Tuple &std_tuple, gdut::index_sequence<Indices...>)
    -> gdut::tuple<typename std::tuple_element<Indices, TStd_Tuple>::type...> {
  return gdut::tuple<typename std::tuple_element<Indices, TStd_Tuple>::type...>(
      std::get<Indices>(std_tuple)...);
}

///*********************************
template <typename TStd_Tuple, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_etl_impl(TStd_Tuple &&std_tuple, gdut::index_sequence<Indices...>)
    -> gdut::tuple<typename std::tuple_element<Indices, TStd_Tuple>::type...> {
  return gdut::tuple<typename std::tuple_element<Indices, TStd_Tuple>::type...>(
      std::move(std::get<Indices>(std_tuple))...);
}
} // namespace private_tuple

//***************************************************************************
/// Converts a std::tuple to an gdut::tuple.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto
to_etl(const std::tuple<TTypes...> &std_tuple)
    -> gdut::tuple<gdut::decay_t<TTypes>...> {
  return private_tuple::to_etl_impl(std_tuple,
                                    gdut::make_index_sequence_for<TTypes...>());
}

//***************************************************************************
/// Converts a std::tuple to an gdut::tuple.
//***************************************************************************
template <typename... TTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 auto to_etl(std::tuple<TTypes...> &&std_tuple)
    -> gdut::tuple<gdut::decay_t<TTypes>...> {
  return private_tuple::to_etl_impl(gdut::move(std_tuple),
                                    gdut::make_index_sequence_for<TTypes...>());
}
#endif

namespace private_tuple {
//***************************************************************************
/// Equality
//***************************************************************************
// When there are no indices left to compare.
template <typename TTuple1, typename TTuple2>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool tuple_equality(const TTuple1 & /*lhs*/,
                                                    const TTuple2 & /*rhs*/,
                                                    gdut::index_sequence<>) {
  return true;
}

// Recursive case: compare the current element and recurse.
template <typename TTuple1, typename TTuple2, size_t Index, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
tuple_equality(const TTuple1 &lhs, const TTuple2 &rhs,
               gdut::index_sequence<Index, Indices...>) {
  return gdut::get<Index>(lhs) == gdut::get<Index>(rhs) &&
         tuple_equality(lhs, rhs, gdut::index_sequence<Indices...>{});
}

//***************************************************************************
/// Less than
//***************************************************************************
// When there are no indices left to compare.
template <typename TTuple1, typename TTuple2>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool tuple_less_than(const TTuple1 & /*lhs*/,
                                                     const TTuple2 & /*rhs*/,
                                                     gdut::index_sequence<>) {
  return false;
}

// Recursively compare the current element and the rest.
template <typename TTuple1, typename TTuple2, size_t Index, size_t... Indices>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
tuple_less_than(const TTuple1 &lhs, const TTuple2 &rhs,
                gdut::index_sequence<Index, Indices...>) {
  if (get<Index>(lhs) < get<Index>(rhs)) {
    return true;
  }

  if (get<Index>(rhs) < get<Index>(lhs)) {
    return false;
  }

  return tuple_less_than(lhs, rhs, gdut::index_sequence<Indices...>{});
}
} // namespace private_tuple

//***************************************************************************
/// Equality operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator==(const gdut::tuple<TTypes...> &lhs,
           const gdut::tuple<UTypes...> &rhs) {
  GDUT_STATIC_ASSERT(sizeof...(TTypes) == sizeof...(UTypes),
                     "Cannot compare tuples of different sizes");

  // Compare each element of the tuples.
  return private_tuple::tuple_equality(
      lhs, rhs, gdut::make_index_sequence<gdut::tuple<TTypes...>::size()>{});
}

//***************************************************************************
/// Inequality operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator!=(const gdut::tuple<TTypes...> &lhs,
           const gdut::tuple<UTypes...> &rhs) {
  return !(lhs == rhs);
}

//***************************************************************************
/// Less than operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator<(const gdut::tuple<TTypes...> &lhs,
          const gdut::tuple<UTypes...> &rhs) {
  GDUT_STATIC_ASSERT(sizeof...(TTypes) == sizeof...(UTypes),
                     "Cannot compare tuples of different sizes");

  // Compare the elements.
  return private_tuple::tuple_less_than(
      lhs, rhs, gdut::make_index_sequence<gdut::tuple<TTypes...>::size()>{});
}

//***************************************************************************
/// Less than or equals operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator<=(const gdut::tuple<TTypes...> &lhs,
           const gdut::tuple<UTypes...> &rhs) {
  return !(rhs < lhs);
}

//***************************************************************************
/// Greater than operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator>(const gdut::tuple<TTypes...> &lhs,
          const gdut::tuple<UTypes...> &rhs) {
  return (rhs < lhs);
}

//***************************************************************************
/// Greater than or equals operator.
//***************************************************************************
template <typename... TTypes, typename... UTypes>
GDUT_NODISCARD GDUT_CONSTEXPR14 bool
operator>=(const gdut::tuple<TTypes...> &lhs,
           const gdut::tuple<UTypes...> &rhs) {
  return !(lhs < rhs);
}

//***************************************************************************
/// Swap two tuples.
//***************************************************************************
template <typename... TTypes>
GDUT_CONSTEXPR14 void swap(gdut::tuple<TTypes...> &lhs,
                           gdut::tuple<TTypes...> &rhs) {
  lhs.swap(rhs);
}
} // namespace gdut

namespace std {
#if GDUT_NOT_USING_STL && !((defined(GDUT_DEVELOPMENT_OS_APPLE) ||             \
                             (GDUT_COMPILER_FULL_VERSION >= 190000)) &&        \
                            defined(GDUT_COMPILER_CLANG))
template <typename T> struct tuple_size;

template <size_t Index, typename TType> struct tuple_element;
#endif

//***************************************************************************
/// Specialisation of tuple_size to allow the use of C++ structured bindings.
//***************************************************************************
template <typename... Types>
struct tuple_size<gdut::tuple<Types...>>
    : gdut::integral_constant<size_t, sizeof...(Types)> {};

//***************************************************************************
/// Specialisation of tuple_element to allow the use of C++ structured bindings.
//***************************************************************************
template <size_t Index, typename... Types>
struct tuple_element<Index, gdut::tuple<Types...>> {
  using type = typename gdut::nth_type_t<Index, Types...>;
};
} // namespace std

#endif
#endif
