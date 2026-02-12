///\file
/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

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

#ifndef GDUT_BITSET_NEW_INCLUDED
#define GDUT_BITSET_NEW_INCLUDED

#include "../algorithm.hpp"
#include "../binary.hpp"
#include "../char_traits.hpp"
#include "../enum_type.hpp"
#include "../error_handler.hpp"
#include "../exception.hpp"
#include "../integral_limits.hpp"
#include "../iterator.hpp"
#include "../largest.hpp"
#include "../log.hpp"
#include "../nullptr.hpp"
#include "../platform.hpp"
#include "../smallest.hpp"
#include "../span.hpp"
#include "../static_assert.hpp"
#include "../string.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if GDUT_USING_STL
#include <algorithm>
#endif

#include "minmax_push.hpp"

#if defined(GDUT_COMPILER_KEIL)
#pragma diag_suppress 1300
#endif

#if GDUT_USING_CPP11
#define GDUT_STR(x) x
#define GDUT_STRL(x) L##x
#define GDUT_STRu(x) u##x
#define GDUT_STRU(x) U##x
#else
#define GDUT_STR(x) x
#define GDUT_STRL(x) x
#define GDUT_STRu(x) x
#define GDUT_STRU(x) x
#endif

//*****************************************************************************
///\defgroup bitset bitset
/// Similar to std::bitset but without requiring std::string.
///\ingroup containers
//*****************************************************************************

namespace gdut {
//***************************************************************************
/// Flags to indicate whether the bitset is contained in a single element
/// or spread over an array of elements.
///\ingroup bitset
//***************************************************************************
struct bitset_storage_model {
  enum enum_type { Undefined = 0, Single = 1, Multi = 2 };

  GDUT_DECLARE_ENUM_TYPE(bitset_storage_model, char)
  GDUT_ENUM_TYPE(Undefined, "Undefined")
  GDUT_ENUM_TYPE(Single, "Single")
  GDUT_ENUM_TYPE(Multi, "Multi")
  GDUT_END_ENUM_TYPE
};

//***************************************************************************
/// Exception base for bitset
///\ingroup bitset
//***************************************************************************
class bitset_exception : public gdut::exception {
public:
  bitset_exception(string_type reason_, string_type file_name_,
                   numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
/// Bitset type_too_small exception.
///\ingroup bitset
//***************************************************************************
class bitset_string_too_small : public bitset_exception {
public:
  bitset_string_too_small(string_type file_name_, numeric_type line_number_)
      : bitset_exception(
            GDUT_ERROR_TEXT("bitset:type_too_small", GDUT_BITSET_FILE_ID "A"),
            file_name_, line_number_) {}
};

//***************************************************************************
/// Bitset overflow exception.
///\ingroup bitset
//***************************************************************************
class bitset_overflow : public bitset_exception {
public:
  bitset_overflow(string_type file_name_, numeric_type line_number_)
      : bitset_exception(
            GDUT_ERROR_TEXT("bitset:overflow", GDUT_BITSET_FILE_ID "B"),
            file_name_, line_number_) {}
};

//***************************************************************************
/// Bitset nullptr buffer exception.
///\ingroup bitset
//***************************************************************************
class bitset_invalid_buffer : public bitset_exception {
public:
  bitset_invalid_buffer(string_type file_name_, numeric_type line_number_)
      : bitset_exception(
            GDUT_ERROR_TEXT("bitset:invalid buffer", GDUT_BITSET_FILE_ID "C"),
            file_name_, line_number_) {}
};

//***************************************************************************
namespace private_bitset {
template <typename TElement> class bitset_impl_common {
public:
  typedef TElement element_type;
  typedef TElement *pointer;
  typedef const TElement *const_pointer;
  typedef size_t size_type;

  static GDUT_CONSTANT size_t npos = gdut::integral_limits<size_t>::max;
  static GDUT_CONSTANT size_t Bits_Per_Element =
      gdut::integral_limits<TElement>::bits;
  static GDUT_CONSTANT TElement All_Set_Element =
      gdut::integral_limits<TElement>::max;
  static GDUT_CONSTANT TElement All_Clear_Element = element_type(0);
};

template <typename TElement>
GDUT_CONSTANT size_t bitset_impl_common<TElement>::npos;

template <typename TElement>
GDUT_CONSTANT size_t bitset_impl_common<TElement>::Bits_Per_Element;

template <typename TElement>
GDUT_CONSTANT TElement bitset_impl_common<TElement>::All_Set_Element;

template <typename TElement>
GDUT_CONSTANT TElement bitset_impl_common<TElement>::All_Clear_Element;
} // namespace private_bitset

//*************************************************************************
/// Bitset implementation declaration.
///\ingroup bitset
//*************************************************************************
template <typename TElement, char Bitset_Layout> class bitset_impl;

//*************************************************************************
/// The implementation class for single element gdut::bitset
///\ingroup bitset
//*************************************************************************
template <typename TElement>
class bitset_impl<TElement, gdut::bitset_storage_model::Single>
    : public gdut::private_bitset::bitset_impl_common<TElement> {
public:
  using
      typename gdut::private_bitset::bitset_impl_common<TElement>::element_type;
  using typename gdut::private_bitset::bitset_impl_common<TElement>::pointer;
  using typename gdut::private_bitset::bitset_impl_common<
      TElement>::const_pointer;

  using gdut::private_bitset::bitset_impl_common<TElement>::Bits_Per_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Set_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Clear_Element;

  using gdut::private_bitset::bitset_impl_common<TElement>::npos;

  //*************************************************************************
  /// Set all of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void set_all(pointer pbuffer,
                                       size_t /*number_of_elements*/,
                                       element_type top_mask) GDUT_NOEXCEPT {
    *pbuffer = All_Set_Element & top_mask;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer, size_t position,
                                            bool value = true) {
    const element_type mask = element_type(element_type(1) << position);

    if (value == true) {
      *pbuffer |= mask;
    } else {
      *pbuffer &= ~mask;
    }
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position>
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer,
                                            bool value = true) {
    const element_type mask = element_type(element_type(1) << Position);

    if (value == true) {
      *pbuffer |= mask;
    } else {
      *pbuffer &= ~mask;
    }
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position, bool Value>
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer) {
    const element_type mask = element_type(element_type(1) << Position);

    if (Value == true) {
      *pbuffer |= mask;
    } else {
      *pbuffer &= ~mask;
    }
  }

  //*************************************************************************
  /// Reset all of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  reset_all(pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *pbuffer = All_Clear_Element;
  }

  //*************************************************************************
  /// Reset the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14

      void
      reset_position(pointer pbuffer, size_t position) {
    const element_type mask = element_type(element_type(1) << position);
    *pbuffer &= ~mask;
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t /*number_of_elements*/,
                                           size_t active_bits,
                                           const char *text) GDUT_NOEXCEPT {
    reset_all(pbuffer, 1U);

    if (text != GDUT_NULLPTR) {
      size_t string_length = gdut::strlen(text);

      // Build from the string.
      string_length = gdut::min(active_bits, string_length);

      element_type mask = element_type(element_type(1) << (string_length - 1U));

      for (size_t i = 0U; i < string_length; ++i) {
        if (text[i] == GDUT_STR('1')) {
          *pbuffer |= mask;
        }

        mask >>= 1U;
      }
    }
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t /*number_of_elements*/,
                                           size_t active_bits,
                                           const wchar_t *text) GDUT_NOEXCEPT {
    reset_all(pbuffer, 1U);

    if (text != GDUT_NULLPTR) {
      size_t string_length = gdut::strlen(text);

      // Build from the string.
      string_length = gdut::min(active_bits, string_length);

      element_type mask = element_type(element_type(1) << (string_length - 1U));

      for (size_t i = 0U; i < string_length; ++i) {
        if (text[i] == GDUT_STRL('1')) {
          *pbuffer |= mask;
        }

        mask >>= 1U;
      }
    }
  }

  //*************************************************************************
  /// Set from a u16 string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t /*number_of_elements*/,
                                           size_t active_bits,
                                           const char16_t *text) GDUT_NOEXCEPT {
    reset_all(pbuffer, 1U);

    if (text != GDUT_NULLPTR) {
      size_t string_length = gdut::strlen(text);

      // Build from the string.
      string_length = gdut::min(active_bits, string_length);

      element_type mask = element_type(element_type(1) << (string_length - 1U));

      for (size_t i = 0U; i < string_length; ++i) {
        if (text[i] == GDUT_STRu('1')) {
          *pbuffer |= mask;
        }

        mask >>= 1U;
      }
    }
  }

  //*************************************************************************
  /// Set from a u32 string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t /*number_of_elements*/,
                                           size_t active_bits,
                                           const char32_t *text) GDUT_NOEXCEPT {
    reset_all(pbuffer, 1U);

    if (text != GDUT_NULLPTR) {
      size_t string_length = gdut::strlen(text);

      // Build from the string.
      string_length = gdut::min(active_bits, string_length);

      element_type mask = element_type(element_type(1) << (string_length - 1U));

      for (size_t i = 0U; i < string_length; ++i) {
        if (text[i] == GDUT_STRU('1')) {
          *pbuffer |= mask;
        }

        mask >>= 1U;
      }
    }
  }

  //*************************************************************************
  /// Get as an integral value.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 T value(const_pointer pbuffer,
                                  size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    return T(*pbuffer);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 T
  extract(const_pointer pbuffer, size_t position,
          size_t length = gdut::integral_limits<T>::bits) {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    const unsigned_t Mask = gdut::make_lsb_mask<unsigned_t>(length);
    const unsigned_t Shift = position % Bits_Per_Element;

    unsigned_t value = static_cast<unsigned_t>(*pbuffer >> Shift) & Mask;

    if ((length != gdut::integral_limits<T>::bits) &&
        gdut::integral_limits<T>::is_signed) {
      value = gdut::sign_extend<T>(value, length);
    }

    return static_cast<T>(value);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  //*************************************************************************
#if GDUT_USING_CPP11
  template <typename T, size_t Position,
            size_t Length = gdut::integral_limits<T>::bits>
#else
  template <typename T, size_t Position, size_t Length>
#endif
  static GDUT_CONSTEXPR14 T extract(const_pointer pbuffer) {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    const unsigned_t Mask = gdut::make_lsb_mask<unsigned_t>(Length);
    const unsigned_t Shift = Position % Bits_Per_Element;

    unsigned_t value = static_cast<unsigned_t>(*pbuffer >> Shift) & Mask;

    if ((Length != gdut::integral_limits<T>::bits) &&
        gdut::integral_limits<T>::is_signed) {
      value = gdut::sign_extend<T>(value, Length);
    }

    return static_cast<T>(value);
  }

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will return
  /// <b>false</b>.
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool test(const_pointer pbuffer, size_t position) {
    const element_type mask = element_type(element_type(1) << position);
    return (*pbuffer & mask) != 0U;
  }

  //*************************************************************************
  /// Count the number of bits set.
  //*************************************************************************
  static GDUT_CONSTEXPR14 size_t
  count(const_pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    return gdut::count_bits(*pbuffer);
  }

  //*************************************************************************
  // Are all the bits sets?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool all(const_pointer pbuffer,
                                   size_t /*number_of_elements*/,
                                   element_type top_mask) GDUT_NOEXCEPT {
    return (*pbuffer & top_mask) == top_mask;
  }

  //*************************************************************************
  // Are all the mask bits sets?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool all(const_pointer pbuffer,
                                   size_t /*number_of_elements*/,
                                   element_type top_mask,
                                   element_type mask) GDUT_NOEXCEPT {
    return (*pbuffer & top_mask & mask) == mask;
  }

  //*************************************************************************
  /// Are none of the bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool
  none(const_pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    return *pbuffer == All_Clear_Element;
  }

  //*************************************************************************
  /// Are none of the mask bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool none(const_pointer pbuffer,
                                    size_t /*number_of_elements*/,
                                    element_type mask) GDUT_NOEXCEPT {
    return (*pbuffer & mask) == All_Clear_Element;
  }

  //*************************************************************************
  /// Are any of the bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool
  any(const_pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    return *pbuffer != All_Clear_Element;
  }

  //*************************************************************************
  /// Are any of the mask bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool any(const_pointer pbuffer,
                                   size_t /*number_of_elements*/,
                                   element_type mask) GDUT_NOEXCEPT {
    return (*pbuffer & mask) != All_Clear_Element;
  }

  //*************************************************************************
  /// Flip all of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  flip_all(pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *pbuffer = ~*pbuffer;
  }

  //*************************************************************************
  /// Flip some of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  flip_bits(pointer pbuffer,
            element_type mask = gdut::integral_limits<element_type>::max)
      GDUT_NOEXCEPT {
    *pbuffer ^= mask;
  }

  //*************************************************************************
  /// Flip the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void flip_position(pointer pbuffer, size_t position) {
    const element_type mask = element_type(element_type(1) << position);
    *pbuffer ^= mask;
  }

  //*************************************************************************
  /// Returns a string representing the bitset.
  //*************************************************************************
  template <typename TString>
  static GDUT_CONSTEXPR14 TString to_string(
      const_pointer pbuffer, size_t active_bits,
      typename TString::value_type zero = typename TString::value_type('0'),
      typename TString::value_type one = typename TString::value_type('1')) {
    TString result;

    result.resize(active_bits, '\0');

    // Check that the string type can contain the digits.
    GDUT_ASSERT_OR_RETURN_VALUE(result.size() == active_bits,
                                GDUT_ERROR(gdut::bitset_string_too_small),
                                result);

    for (size_t i = active_bits; i > 0; --i) {
      result[active_bits - i] = test(pbuffer, i - 1) ? one : zero;
    }

    return result;
  }

  //*************************************************************************
  /// Finds the next bit in the specified state.
  ///\param state    The state to search for.
  ///\param position The position to start from.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  static GDUT_CONSTEXPR14 size_t find_next(const_pointer pbuffer,
                                           size_t /*number_of_elements*/,
                                           size_t active_bits, bool state,
                                           size_t position) GDUT_NOEXCEPT {
    if (position < active_bits) {
      // Where to start.
      size_t bit = position;

      element_type mask = 1U << position;

      // Needs checking?
      if ((state && (*pbuffer != All_Clear_Element)) ||
          (!state && (*pbuffer != All_Set_Element))) {
        // For each bit in the element...
        while (bit < active_bits) {
          // Equal to the required state?
          if (((*pbuffer & mask) != 0) == state) {
            return bit;
          }

          // Move on to the next bit.
          mask <<= 1;
          ++bit;
        }
      }
    }

    return npos;
  }

  //*************************************************************************
  /// operator assignment
  /// Assigns rhs to lhs
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_assignment(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
                      size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *lhs_pbuffer = *rhs_pbuffer;
  }

  //*************************************************************************
  /// operator and
  /// AND lhs and rhs and put the result in lhs
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_and(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
               size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *lhs_pbuffer &= *rhs_pbuffer;
  }

  //*************************************************************************
  /// operator or
  /// OR lhs and rhs and put the result in lhs
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_or(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
              size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *lhs_pbuffer |= *rhs_pbuffer;
  }

  //*************************************************************************
  /// operator xor
  /// XOR lhs and rhs and put the result in lhs
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_xor(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
               size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *lhs_pbuffer ^= *rhs_pbuffer;
  }

  //*************************************************************************
  /// operator ~
  /// NOT the value in the buffer
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_not(pointer pbuffer, size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    *pbuffer = ~*pbuffer;
  }

  //*************************************************************************
  /// operator_shift_left
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_shift_left(pointer pbuffer, size_t /*number_of_elements*/,
                      size_t active_bits, size_t shift) GDUT_NOEXCEPT {
    if (shift >= active_bits) {
      reset_all(pbuffer, 1U);
    } else {
      *pbuffer <<= shift;
    }
  }

  //*************************************************************************
  /// operator_shift_right
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_shift_right(pointer pbuffer, size_t /*number_of_elements*/,
                       size_t active_bits, size_t shift) GDUT_NOEXCEPT {
    if (shift >= active_bits) {
      reset_all(pbuffer, 1U);
    } else {
      *pbuffer >>= shift;
    }
  }

  //*************************************************************************
  /// operator_equality
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool
  operator_equality(const_pointer lhs_pbuffer, const_pointer rhs_pbuffer,
                    size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    return (*lhs_pbuffer == *rhs_pbuffer);
  }

  //*************************************************************************
  /// Initialise from an unsigned long long.
  //*************************************************************************
  template <typename TElementType>
  static GDUT_CONSTEXPR14 void
  initialise(pointer pbuffer, size_t /*number_of_elements*/,
             unsigned long long value) GDUT_NOEXCEPT {
    *pbuffer = static_cast<TElementType>(value);
  }

  //*************************************************************************
  /// swap
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  swap(pointer lhs_pbuffer, pointer rhs_pbuffer,
       size_t /*number_of_elements*/) GDUT_NOEXCEPT {
    element_type temp = *lhs_pbuffer;
    *lhs_pbuffer = *rhs_pbuffer;
    *rhs_pbuffer = temp;
  }
};

//*************************************************************************
/// The implementation class for multi element gdut::bitset
///\ingroup bitset
//*************************************************************************
template <typename TElement>
class bitset_impl<TElement, gdut::bitset_storage_model::Multi>
    : public gdut::private_bitset::bitset_impl_common<TElement> {
private:
  typedef gdut::private_bitset::bitset_impl_common<TElement> common;

public:
  using
      typename gdut::private_bitset::bitset_impl_common<TElement>::element_type;
  using typename gdut::private_bitset::bitset_impl_common<TElement>::pointer;
  using typename gdut::private_bitset::bitset_impl_common<
      TElement>::const_pointer;

  using gdut::private_bitset::bitset_impl_common<TElement>::Bits_Per_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Set_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Clear_Element;

  using gdut::private_bitset::bitset_impl_common<TElement>::npos;

  //*************************************************************************
  /// Check to see if the requested extract is contained within one element.
  //*************************************************************************
  template <size_t Position, size_t Length, size_t Bits_Per_Element>
  struct value_is_in_one_element {
    static GDUT_CONSTANT bool value =
        ((Position + Length - 1) >> gdut::log2<Bits_Per_Element>::value) ==
        (Position >> gdut::log2<Bits_Per_Element>::value);
  };

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will return
  /// <b>false</b>.
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool test(const_pointer pbuffer,
                                    size_t position) GDUT_NOEXCEPT {
    size_t index = position >> gdut::log2<Bits_Per_Element>::value;
    element_type mask = element_type(1) << (position & (Bits_Per_Element - 1));

    return (pbuffer[index] & mask) != 0;
  }

  //*************************************************************************
  /// Count the number of bits set.
  //*************************************************************************
  static GDUT_CONSTEXPR14 size_t
  count(const_pointer pbuffer, size_t number_of_elements) GDUT_NOEXCEPT {
    size_t count = 0;

    while (number_of_elements-- != 0) {
      count += gdut::count_bits(*pbuffer++);
    }

    return count;
  }

  //*************************************************************************
  // Are all the bits sets?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool all(const_pointer pbuffer,
                                   size_t number_of_elements,
                                   element_type top_mask) GDUT_NOEXCEPT {
    // All but the last.
    while (number_of_elements-- != 1U) {
      if (*pbuffer++ != All_Set_Element) {
        return false;
      }
    }

    // The last.
    if ((*pbuffer & top_mask) != top_mask) {
      return false;
    }

    return true;
  }

  //*************************************************************************
  /// Are none of the bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool none(const_pointer pbuffer,
                                    size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      if (*pbuffer++ != 0) {
        return false;
      }
    }

    return true;
  }

  //*************************************************************************
  /// Are any of the bits set?
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool any(const_pointer pbuffer,
                                   size_t number_of_elements) GDUT_NOEXCEPT {
    bool any_set = false;

    while (number_of_elements-- != 0) {
      if (*pbuffer++ != All_Clear_Element) {
        any_set = true;
        break;
      }
    }

    return any_set;
  }

  //*************************************************************************
  /// Set all bits
  //*************************************************************************
  static GDUT_CONSTEXPR14 void set_all(pointer pbuffer,
                                       size_t number_of_elements,
                                       element_type top_mask) GDUT_NOEXCEPT {
    while (number_of_elements-- != 1U) {
      *pbuffer++ = All_Set_Element;
    }

    *pbuffer = (All_Set_Element & top_mask);
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer, size_t position,
                                            bool value = true) GDUT_NOEXCEPT {
    size_t index = position >> gdut::log2<Bits_Per_Element>::value;
    element_type bit = element_type(1) << (position & (Bits_Per_Element - 1));

    if (value == true) {
      pbuffer[index] |= bit;
    } else {
      pbuffer[index] &= ~bit;
    }
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position>
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer,
                                            bool value = true) {
    size_t index = Position >> gdut::log2<Bits_Per_Element>::value;
    element_type bit = element_type(1) << (Position & (Bits_Per_Element - 1));

    if (value == true) {
      pbuffer[index] |= bit;
    } else {
      pbuffer[index] &= ~bit;
    }
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position, bool Value>
  static GDUT_CONSTEXPR14 void set_position(pointer pbuffer) {
    size_t index = Position >> gdut::log2<Bits_Per_Element>::value;
    element_type bit = element_type(1) << (Position & (Bits_Per_Element - 1));

    if (Value == true) {
      pbuffer[index] |= bit;
    } else {
      pbuffer[index] &= ~bit;
    }
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t number_of_elements,
                                           size_t total_bits,
                                           const char *text) GDUT_NOEXCEPT {
    if (text == GDUT_NULLPTR) {
      gdut::fill_n(pbuffer, number_of_elements, All_Clear_Element);
    } else {
      size_t string_length = gdut::strlen(text);
      size_t index = gdut::min(number_of_elements - 1U,
                               (string_length / Bits_Per_Element));

      // Only reset elements we need to.
      while (index != number_of_elements) {
        pbuffer[index++] = All_Clear_Element;
      }

      // Build from the string.
      size_t i = gdut::min(total_bits, string_length);

      while (i > 0) {
        set_position(pbuffer, --i, *text++ == GDUT_STR('1'));
      }
    }
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t number_of_elements,
                                           size_t total_bits,
                                           const wchar_t *text) GDUT_NOEXCEPT {
    if (text == GDUT_NULLPTR) {
      gdut::fill_n(pbuffer, number_of_elements, All_Clear_Element);
    } else {
      size_t string_length = gdut::strlen(text);
      size_t index = gdut::min(number_of_elements - 1U,
                               (string_length / Bits_Per_Element));

      // Only reset elements we need to.
      while (index != number_of_elements) {
        pbuffer[index++] = All_Clear_Element;
      }

      // Build from the string.
      size_t i = gdut::min(total_bits, string_length);

      while (i > 0) {
        set_position(pbuffer, --i, *text++ == GDUT_STRL('1'));
      }
    }
  }

  //*************************************************************************
  /// Set from a u16 string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t number_of_elements,
                                           size_t total_bits,
                                           const char16_t *text) GDUT_NOEXCEPT {
    if (text == GDUT_NULLPTR) {
      gdut::fill_n(pbuffer, number_of_elements, All_Clear_Element);
    } else {
      size_t string_length = gdut::strlen(text);
      size_t index = gdut::min(number_of_elements - 1U,
                               (string_length / Bits_Per_Element));

      // Only reset elements we need to.
      while (index != number_of_elements) {
        pbuffer[index++] = All_Clear_Element;
      }

      // Build from the string.
      size_t i = gdut::min(total_bits, string_length);

      while (i > 0) {
        set_position(pbuffer, --i, *text++ == GDUT_STRu('1'));
      }
    }
  }

  //*************************************************************************
  /// Set from a u32 string.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void from_string(pointer pbuffer,
                                           size_t number_of_elements,
                                           size_t total_bits,
                                           const char32_t *text) GDUT_NOEXCEPT {
    if (text == GDUT_NULLPTR) {
      gdut::fill_n(pbuffer, number_of_elements, All_Clear_Element);
    } else {
      size_t string_length = gdut::strlen(text);
      size_t index = gdut::min(number_of_elements - 1U,
                               (string_length / Bits_Per_Element));

      // Only reset elements we need to.
      while (index != number_of_elements) {
        pbuffer[index++] = All_Clear_Element;
      }

      // Build from the string.
      size_t i = gdut::min(total_bits, string_length);

      while (i > 0) {
        set_position(pbuffer, --i, *text++ == GDUT_STRU('1'));
      }
    }
  }

  //*************************************************************************
  /// Reset all of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  reset_all(pointer pbuffer, size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0U) {
      *pbuffer++ = All_Clear_Element;
    }
  }

  //*************************************************************************
  /// Reset the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void reset_position(pointer pbuffer,
                                              size_t position) GDUT_NOEXCEPT {
    const size_t index = position >> gdut::log2<Bits_Per_Element>::value;
    const element_type bit = element_type(1)
                             << (position & (Bits_Per_Element - 1));

    pbuffer[index] &= ~bit;
  }

  //*************************************************************************
  /// Get as a value.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 T value(const_pointer pbuffer,
                                  size_t number_of_elements) GDUT_NOEXCEPT {
    T v = T(0);

    const bool OK =
        (sizeof(T) * CHAR_BIT) >= (number_of_elements * Bits_Per_Element);

    if (OK) {
      uint_least8_t shift = 0U;

      for (size_t i = 0UL; i < number_of_elements; ++i) {
        v |= T(typename gdut::make_unsigned<T>::type(pbuffer[i]) << shift);
        shift += uint_least8_t(Bits_Per_Element);
      }
    }

    return v;
  }

  //*************************************************************************
  /// Extract an value from multiple elements.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 typename gdut::make_unsigned<T>::type
  extract_from_multiple_elements(const element_type *pbuffer, int element_index,
                                 size_t active_bits_in_msb,
                                 size_t length) GDUT_NOEXCEPT {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    unsigned_t value(0);

    // Extract the first element, if partially filled.
    if (active_bits_in_msb < Bits_Per_Element) {
      element_type mask = gdut::make_lsb_mask<element_type>(active_bits_in_msb);
      value = pbuffer[element_index] & mask;
      length -= active_bits_in_msb;
      if (length >= Bits_Per_Element) {
        value = value << Bits_Per_Element;
      }
      --element_index;
    }

    // Loop through the fully filled elements
    while (length >= Bits_Per_Element) {
      value |= pbuffer[element_index];
      length -= Bits_Per_Element;
      if (length >= Bits_Per_Element) {
        value = value << Bits_Per_Element;
      }
      --element_index;
    }

    // Extract the last element, if partially filled.
    if (length != 0) {
      value = value << length;
      element_type mask = gdut::make_lsb_mask<element_type>(length);
      value |= (pbuffer[element_index] >> (Bits_Per_Element - length)) & mask;
    }

    return value;
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  /// Run time position and length.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 typename gdut::make_unsigned<T>::type
  extract_from_buffer(const_pointer pbuffer, size_t position,
                      size_t length) GDUT_NOEXCEPT {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    unsigned_t value(0);

    const int Msb_Element_Index =
        (position + length - 1) >> gdut::log2<Bits_Per_Element>::value;
    const int Lsb_Element_Index =
        position >> gdut::log2<Bits_Per_Element>::value;

    // Is the value contained within one element?
    if (Msb_Element_Index == Lsb_Element_Index) {
      const unsigned_t Mask = gdut::make_lsb_mask<unsigned_t>(length);
      const unsigned_t Shift = position % Bits_Per_Element;

      value =
          static_cast<unsigned_t>(pbuffer[Msb_Element_Index] >> Shift) & Mask;
    } else {
      // Get the number of active bits in the msb element
      size_t active_bits_in_msb =
          (position + length) - (Msb_Element_Index * Bits_Per_Element);

      // Start with index of the element containing the msb.
      int element_index = Msb_Element_Index;

      value = extract_from_multiple_elements<T>(pbuffer, element_index,
                                                active_bits_in_msb, length);
    }

    return value;
  }

  //*************************************************************************
  /// Extract an integral value when the value fits in one element.
  /// Compile time position and length.
  //*************************************************************************
  template <typename T, size_t Position, size_t Length>
  static GDUT_CONSTEXPR14 typename gdut::enable_if<
      value_is_in_one_element<Position, Length, Bits_Per_Element>::value,
      typename gdut::make_unsigned<T>::type>::type
  extract_from_buffer(const_pointer pbuffer) {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    const int Element_Index =
        (Position + Length - 1) >> gdut::log2<Bits_Per_Element>::value;
    const unsigned_t Mask = gdut::lsb_mask<unsigned_t, Length>::value;
    const unsigned_t Shift = Position % Bits_Per_Element;

    return static_cast<unsigned_t>(pbuffer[Element_Index] >> Shift) & Mask;
  }

  //*************************************************************************
  /// Extract an integral value when the value spans more than one element.
  /// Compile time position and length.
  //*************************************************************************
  template <typename T, size_t Position, size_t Length>
  static GDUT_CONSTEXPR14 typename gdut::enable_if<
      !value_is_in_one_element<Position, Length, Bits_Per_Element>::value,
      typename gdut::make_unsigned<T>::type>::type
  extract_from_buffer(const_pointer pbuffer) {
    // Start with index of the element containing the msb.
    const int Msb_Element_Index =
        (Position + Length - 1) >> gdut::log2<Bits_Per_Element>::value;

    // Get the number of active bits in the first element
    const size_t Active_Bits_In_Msb =
        ((Position + Length - 1) % Bits_Per_Element) + 1;

    return extract_from_multiple_elements<T>(pbuffer, Msb_Element_Index,
                                             Active_Bits_In_Msb, Length);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  //*************************************************************************
  template <typename T>
  static GDUT_CONSTEXPR14 T
  extract(const_pointer pbuffer, size_t position,
          size_t length = gdut::integral_limits<T>::bits) {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    unsigned_t value =
        extract_from_buffer<unsigned_t>(pbuffer, position, length);

    if ((length != gdut::integral_limits<T>::bits) &&
        gdut::integral_limits<T>::is_signed) {
      value = gdut::sign_extend<T>(value, length);
    }

    return static_cast<T>(value);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  //*************************************************************************
  template <typename T, size_t Position, size_t Length>
  static GDUT_CONSTEXPR14 T extract(const_pointer pbuffer) {
    typedef typename gdut::make_unsigned<T>::type unsigned_t;

    unsigned_t value =
        extract_from_buffer<unsigned_t, Position, Length>(pbuffer);

    if ((Length != gdut::integral_limits<T>::bits) &&
        gdut::integral_limits<T>::is_signed) {
      value = gdut::sign_extend<T>(value, Length);
    }

    return static_cast<T>(value);
  }

  //*************************************************************************
  /// Flip all of the bits.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  flip_all(pointer pbuffer, size_t number_of_elements) GDUT_NOEXCEPT {
    operator_not(pbuffer, number_of_elements);
  }

  //*************************************************************************
  /// Flip the bit at the position.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void flip_position(pointer pbuffer,
                                             size_t position) GDUT_NOEXCEPT {
    const size_t index = position >> gdut::log2<Bits_Per_Element>::value;
    const element_type bit = element_type(1)
                             << (position & (Bits_Per_Element - 1));

    pbuffer[index] ^= bit;
  }

  //*************************************************************************
  /// Finds the next bit in the specified state.
  ///\param state    The state to search for.
  ///\param position The position to start from.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  static GDUT_CONSTEXPR14 size_t find_next(const_pointer pbuffer,
                                           size_t number_of_elements,
                                           size_t total_bits, bool state,
                                           size_t position) GDUT_NOEXCEPT {
    // Where to start.
    size_t index = position >> log2<Bits_Per_Element>::value;
    size_t bit = position & (Bits_Per_Element - 1);

    element_type mask = 1 << bit;

    // For each element in the bitset...
    while (index < number_of_elements) {
      element_type value = pbuffer[index];

      // Needs checking?
      if ((state && (value != All_Clear_Element)) ||
          (!state && (value != All_Set_Element))) {
        // For each bit in the element...
        while ((bit < Bits_Per_Element) && (position < total_bits)) {
          // Equal to the required state?
          if (((value & mask) != 0) == state) {
            return position;
          }

          // Move on to the next bit.
          mask <<= 1;
          ++position;
          ++bit;
        }
      } else {
        position += (Bits_Per_Element - bit);
      }

      // Start at the beginning for all other elements.
      bit = 0;
      mask = 1;

      ++index;
    }

    return npos;
  }

  //*************************************************************************
  /// Returns a string representing the bitset.
  //*************************************************************************
  template <typename TString>
  static GDUT_CONSTEXPR14 TString to_string(const_pointer pbuffer,
                                            size_t active_bits,
                                            typename TString::value_type zero,
                                            typename TString::value_type one) {
    TString result;

    result.resize(active_bits, '\0');

    // Check that the string type can contain the digits.
    GDUT_ASSERT_OR_RETURN_VALUE(result.size() == active_bits,
                                GDUT_ERROR(gdut::bitset_string_too_small),
                                result);

    for (size_t i = active_bits; i > 0; --i) {
      result[active_bits - i] = test(pbuffer, i - 1) ? one : zero;
    }

    return result;
  }

  //*************************************************************************
  /// operator assignment
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_assignment(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
                      size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      *lhs_pbuffer = *rhs_pbuffer;
      ++lhs_pbuffer;
      ++rhs_pbuffer;
    }
  }

  //*************************************************************************
  /// operator and
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_and(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
               size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      *lhs_pbuffer &= *rhs_pbuffer;
      ++lhs_pbuffer;
      ++rhs_pbuffer;
    }
  }

  //*************************************************************************
  /// operator or
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_or(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
              size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      *lhs_pbuffer |= *rhs_pbuffer;
      ++lhs_pbuffer;
      ++rhs_pbuffer;
    }
  }

  //*************************************************************************
  /// operator xor
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_xor(pointer lhs_pbuffer, const_pointer rhs_pbuffer,
               size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      *lhs_pbuffer ^= *rhs_pbuffer;
      ++lhs_pbuffer;
      ++rhs_pbuffer;
    }
  }

  //*************************************************************************
  /// operator not
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_not(pointer pbuffer, size_t number_of_elements) GDUT_NOEXCEPT {
    while (number_of_elements-- != 0) {
      *pbuffer = ~*pbuffer;
      ++pbuffer;
    }
  }

  //*************************************************************************
  /// operator_shift_left
  //*************************************************************************
  static GDUT_CONSTEXPR14 void operator_shift_left(pointer pbuffer,
                                                   size_t number_of_elements,
                                                   size_t active_bits,
                                                   size_t shift) GDUT_NOEXCEPT {
    if (shift >= active_bits) {
      reset_all(pbuffer, number_of_elements);
    } else {
      // The place where the elements are split when shifting.
      const size_t split_position =
          Bits_Per_Element - (shift % Bits_Per_Element);

      // Where we are shifting from.
      int src_index = int(number_of_elements - (shift / Bits_Per_Element) - 1U);

      // Where we are shifting to.
      int dst_index = int(number_of_elements - 1U);

      // Shift control constants.
      const size_t lsb_shift = Bits_Per_Element - split_position;
      const size_t msb_shift = split_position;

      const element_type lsb_mask =
          element_type(gdut::integral_limits<element_type>::max >>
                       (Bits_Per_Element - split_position));
      const element_type msb_mask =
          gdut::integral_limits<element_type>::max - lsb_mask;
      const element_type lsb_shifted_mask = element_type(lsb_mask << lsb_shift);

      // First lsb.
      element_type lsb =
          element_type((pbuffer[src_index] & lsb_mask) << lsb_shift);
      pbuffer[dst_index] = lsb;
      --src_index;

      // Now do the shifting.
      while (src_index >= 0) {
        // Shift msb.
        element_type msb =
            element_type((pbuffer[src_index] & msb_mask) >> msb_shift);
        pbuffer[dst_index] = pbuffer[dst_index] | msb;
        --dst_index;

        // Shift lsb.
        lsb = element_type((pbuffer[src_index] & lsb_mask) << lsb_shift);
        pbuffer[dst_index] = lsb;
        --src_index;
      }

      // Clear the remaining bits.
      // First lsb.
      pbuffer[dst_index] &= lsb_shifted_mask;

      // The other remaining bytes on the right.
      for (int i = 0; i < dst_index; ++i) {
        pbuffer[i] = 0;
      }
    }
  }

  //*************************************************************************
  /// operator_shift_right
  //*************************************************************************
  static GDUT_CONSTEXPR14 void
  operator_shift_right(pointer pbuffer, size_t number_of_elements,
                       size_t active_bits, size_t shift) GDUT_NOEXCEPT {
    if (shift >= active_bits) {
      reset_all(pbuffer, number_of_elements);
    } else {
      // The place where the elements are split when shifting.
      const size_t split_position = shift % Bits_Per_Element;

      // Where we are shifting from.
      int src_index = int(shift / Bits_Per_Element);

      // Where we are shifting to.
      int dst_index = 0;

      // Shift control constants.
      const size_t lsb_shift = Bits_Per_Element - split_position;
      const size_t msb_shift = split_position;

      const element_type lsb_mask =
          element_type(gdut::integral_limits<element_type>::max >>
                       (Bits_Per_Element - split_position));
      const element_type msb_mask =
          gdut::integral_limits<element_type>::max - lsb_mask;
      const element_type msb_shifted_mask = element_type(msb_mask >> msb_shift);

      // Now do the shifting.
      while (src_index < int(number_of_elements - 1)) {
        // Shift msb.
        element_type msb =
            element_type((pbuffer[src_index] & msb_mask) >> msb_shift);
        ++src_index;

        // Shift lsb.
        element_type lsb =
            element_type((pbuffer[src_index] & lsb_mask) << lsb_shift);

        // Combine them.
        pbuffer[dst_index] = lsb | msb;
        ++dst_index;
      }

      // Final msb.
      element_type msb =
          element_type((pbuffer[src_index] & msb_mask) >> msb_shift);
      pbuffer[dst_index] = msb;

      // Clear the remaining bits.
      // First msb.
      pbuffer[dst_index] &= msb_shifted_mask;
      ++dst_index;

      // The other remaining bytes.
      while (dst_index < int(number_of_elements)) {
        pbuffer[dst_index] = 0;
        ++dst_index;
      }
    }
  }

  //*************************************************************************
  /// operator_equality
  //*************************************************************************
  static GDUT_CONSTEXPR14 bool
  operator_equality(const_pointer lhs_pbuffer, const_pointer rhs_pbuffer,
                    size_t number_of_elements) GDUT_NOEXCEPT {
    return gdut::equal(lhs_pbuffer, lhs_pbuffer + number_of_elements,
                       rhs_pbuffer);
  }

  //*************************************************************************
  /// Initialise from an unsigned long long.
  /// Enabled if the number of bits in the element equals the number of bits in
  /// an unsigned long long.
  //*************************************************************************
  template <typename TElementType>
  static GDUT_CONSTEXPR14 typename gdut::enable_if<
      gdut::integral_limits<TElementType>::bits ==
          gdut::integral_limits<unsigned long long>::bits,
      void>::type
  initialise(pointer pbuffer, size_t number_of_elements,
             unsigned long long value) GDUT_NOEXCEPT {
    size_t i = 0UL;

    // Set the non-zero elements.
    pbuffer[i++] = value;

    // Clear the remaining elements.
    while (i != number_of_elements) {
      pbuffer[i++] = All_Clear_Element;
    }
  }

  //*************************************************************************
  /// Initialise from an unsigned long long.
  /// Enabled if the number of bits in the element doesn't equal the number of
  /// bits in an unsigned long long.
  //*************************************************************************
  template <typename TElementType>
  static GDUT_CONSTEXPR14 typename gdut::enable_if<
      gdut::integral_limits<TElementType>::bits !=
          gdut::integral_limits<unsigned long long>::bits,
      void>::type
  initialise(pointer pbuffer, size_t number_of_elements,
             unsigned long long value) GDUT_NOEXCEPT {
    size_t i = 0UL;

    // Set the non-zero elements.
    const unsigned long long Shift = gdut::integral_limits<element_type>::bits;

    while ((value != 0) && (i != number_of_elements)) {
      pbuffer[i++] = value & All_Set_Element;
      value = value >> Shift;
    }

    // Clear the remaining elements.
    while (i != number_of_elements) {
      pbuffer[i++] = All_Clear_Element;
    }
  }

  //*************************************************************************
  /// Swap bitset buffers.
  //*************************************************************************
  static GDUT_CONSTEXPR14 void swap(pointer pbuffer1, pointer pbuffer2,
                                    size_t number_of_elements) {
    gdut::swap_ranges(pbuffer1, pbuffer1 + number_of_elements, pbuffer2);
  }
};

namespace private_bitset {
//***************************************************************************
template <size_t Active_Bits, typename TElement>
class bitset_common
    : public gdut::private_bitset::bitset_impl_common<TElement> {
public:
  typedef
      typename gdut::private_bitset::bitset_impl_common<TElement>::element_type
          element_type;

  using gdut::private_bitset::bitset_impl_common<TElement>::Bits_Per_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Set_Element;
  using gdut::private_bitset::bitset_impl_common<TElement>::All_Clear_Element;

  static GDUT_CONSTANT size_t Number_Of_Elements =
      (Active_Bits % Bits_Per_Element == 0)
          ? Active_Bits / Bits_Per_Element
          : Active_Bits / Bits_Per_Element + 1;
  static GDUT_CONSTANT size_t Size = Active_Bits;
  static GDUT_CONSTANT size_t Allocated_Bits =
      Number_Of_Elements * Bits_Per_Element;

#if GDUT_USING_CPP11
  static GDUT_CONSTANT gdut::bitset_storage_model Storage_Model =
      (bitset_common<Active_Bits, TElement>::Number_Of_Elements == 1U)
          ? gdut::bitset_storage_model::Single
          : gdut::bitset_storage_model::Multi;
#else
  static GDUT_CONSTANT gdut::bitset_storage_model Storage_Model;
#endif

  typedef gdut::span<element_type, Number_Of_Elements> span_type;
  typedef gdut::span<const element_type, Number_Of_Elements> const_span_type;

private:
  static GDUT_CONSTANT size_t Top_Mask_Shift =
      ((Bits_Per_Element -
        ((Number_Of_Elements * Bits_Per_Element) - Active_Bits)) %
       Bits_Per_Element);

public:
  static GDUT_CONSTANT TElement Top_Mask =
      element_type(Top_Mask_Shift == 0 ? All_Set_Element
                                       : ~(All_Set_Element << Top_Mask_Shift));
};

template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT size_t bitset_common<Active_Bits, TElement>::Number_Of_Elements;

template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT size_t bitset_common<Active_Bits, TElement>::Size;

#if GDUT_USING_CPP11
template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT gdut::bitset_storage_model
    bitset_common<Active_Bits, TElement>::Storage_Model;
#else
template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT gdut::bitset_storage_model
    bitset_common<Active_Bits, TElement>::Storage_Model =
        (bitset_common<Active_Bits, TElement>::Number_Of_Elements == 1U)
            ? gdut::bitset_storage_model::Single
            : gdut::bitset_storage_model::Multi;
#endif

template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT size_t bitset_common<Active_Bits, TElement>::Top_Mask_Shift;

template <size_t Active_Bits, typename TElement>
GDUT_CONSTANT TElement bitset_common<Active_Bits, TElement>::Top_Mask;
} // namespace private_bitset

//***************************************************************************
/// Bitset forward declaration
//***************************************************************************
template <size_t Active_Bits = 0U, typename TElement = unsigned char>
class bitset;

//***************************************************************************
/// Specialisation for zero bits.
//***************************************************************************
template <>
class bitset<0U, unsigned char>
    : public gdut::private_bitset::bitset_common<0U, unsigned char> {
public:
  typedef gdut::private_bitset::bitset_common<0U, unsigned char>::element_type
      element_type;
  typedef gdut::private_bitset::bitset_common<0U, unsigned char>::span_type
      span_type;
  typedef gdut::private_bitset::bitset_common<
      0U, unsigned char>::const_span_type const_span_type;

  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::Bits_Per_Element;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::All_Set_Element;
  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::All_Clear_Element;
  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::Number_Of_Elements;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Size;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Storage_Model;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Top_Mask;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Allocated_Bits;
};

//*************************************************************************
/// The bitset top level template.
//*************************************************************************
template <size_t Active_Bits, typename TElement>
class bitset
    : public gdut::private_bitset::bitset_common<Active_Bits, TElement> {
public:
  GDUT_STATIC_ASSERT(gdut::is_unsigned<TElement>::value,
                     "The element type must be unsigned");

  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::element_type element_type;
  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::span_type span_type;
  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::const_span_type const_span_type;

  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Bits_Per_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::All_Set_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::All_Clear_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Number_Of_Elements;
  using gdut::private_bitset::bitset_common<Active_Bits, TElement>::Size;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Storage_Model;
  using gdut::private_bitset::bitset_common<Active_Bits, TElement>::Top_Mask;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Allocated_Bits;

  //*************************************************************************
  /// The reference type returned.
  //*************************************************************************
  class bit_reference {
  public:
    friend class bitset;

    //*******************************
    /// Copy constructor.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference(const bit_reference &other) GDUT_NOEXCEPT
        : p_bitset(other.p_bitset),
          position(other.position) {}

    //*******************************
    /// Conversion operator.
    //*******************************
    GDUT_CONSTEXPR14 operator bool() const GDUT_NOEXCEPT {
      return p_bitset->test(position);
    }

    //*******************************
    /// Assignment operator.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &operator=(bool b) GDUT_NOEXCEPT {
      p_bitset->set(position, b);
      return *this;
    }

    //*******************************
    /// Assignment operator.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &
    operator=(const bit_reference &r) GDUT_NOEXCEPT {
      p_bitset->set(position, bool(r));
      return *this;
    }

    //*******************************
    /// Flip the bit.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &flip() GDUT_NOEXCEPT {
      p_bitset->flip(position);
      return *this;
    }

    //*******************************
    /// Return the logical inverse of the bit.
    //*******************************
    GDUT_CONSTEXPR14 bool operator~() const GDUT_NOEXCEPT {
      return !p_bitset->test(position);
    }

  private:
    //*******************************
    /// Default constructor.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference() GDUT_NOEXCEPT : p_bitset(GDUT_NULLPTR),
                                                     position(0) {}

    //*******************************
    /// Constructor.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference(bitset<Active_Bits, TElement> &r_bitset,
                                   size_t position_) GDUT_NOEXCEPT
        : p_bitset(&r_bitset),
          position(position_) {}

    bitset<Active_Bits, TElement> *p_bitset; ///< The bitset.
    size_t position;                         ///< The position in the bitset.
  };

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset() GDUT_NOEXCEPT : buffer() {
    implementation::reset_all(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Copy constructor.
  //*************************************************************************
  GDUT_CONSTEXPR14
  bitset(const bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT : buffer() {
    implementation::operator_assignment(buffer, other.buffer,
                                        Number_Of_Elements);
  }

  //*************************************************************************
  /// Construct from a value.
  //*************************************************************************
  template <typename TValue>
  GDUT_CONSTEXPR14
  bitset(TValue value,
         typename gdut::enable_if<is_integral<TValue>::value>::type * = 0)
      GDUT_NOEXCEPT : buffer() {
    implementation::template initialise<element_type>(
        buffer, Number_Of_Elements, value);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset(
      TPString text,
      typename gdut::enable_if<is_same<TPString, const char *>::value>::type * =
          0) GDUT_NOEXCEPT : buffer() {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset(
      TPString text,
      typename gdut::enable_if<is_same<TPString, const wchar_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : buffer() {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset(
      TPString text,
      typename gdut::enable_if<is_same<TPString, const char16_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : buffer() {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset(
      TPString text,
      typename gdut::enable_if<is_same<TPString, const char32_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : buffer() {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Assignment operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset &
  operator=(const bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_assignment(buffer, other.buffer,
                                        Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Set all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &set() GDUT_NOEXCEPT {
    implementation::set_all(buffer, Number_Of_Elements, Top_Mask);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14
  bitset<Active_Bits, TElement> &set(size_t position, bool value = true) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::set_position(buffer, position, value);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position>
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &set(bool value = true) {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    implementation::template set_position<Position>(buffer, value);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position, bool Value>
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &set() {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    implementation::template set_position<Position, Value>(buffer);

    return *this;
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char *>::value,
                               bitset<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const wchar_t *>::value,
                               bitset<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a char16 string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char16_t *>::value,
                               bitset<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a char32 string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char32_t *>::value,
                               bitset<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  from_string(const char *text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  from_string(const wchar_t *text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a u16 string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  from_string(const char16_t *text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a u32 string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  from_string(const char32_t *text) GDUT_NOEXCEPT {
    implementation::from_string(buffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Get as an integral value.
  //*************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_integral<T>::value, T>::type
      value() const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");
    GDUT_STATIC_ASSERT(gdut::integral_limits<T>::bits >=
                           (Number_Of_Elements * Bits_Per_Element),
                       "Type too small");

    return implementation::template value<T>(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  /// Run time position and length.
  //*************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 T extract(
      size_t position, size_t length = gdut::integral_limits<T>::bits) const {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");

    GDUT_ASSERT_OR_RETURN_VALUE(length <= gdut::integral_limits<T>::bits,
                                GDUT_ERROR(bitset_overflow), 0);
    GDUT_ASSERT_OR_RETURN_VALUE((position + length) <= Active_Bits,
                                GDUT_ERROR(bitset_overflow), 0);

    return implementation::template extract<T>(buffer, position, length);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  /// Compile time position and length.
  //*************************************************************************
#if GDUT_USING_CPP11
  template <typename T, size_t Position,
            size_t Length = gdut::integral_limits<T>::bits>
#else
  template <typename T, size_t Position, size_t Length>
#endif
  GDUT_CONSTEXPR14 T extract() const {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");
    GDUT_STATIC_ASSERT(Length <= gdut::integral_limits<T>::bits,
                       "Length is larger that the required type");
    GDUT_STATIC_ASSERT((Position + Length) <= Active_Bits,
                       "Position/Length overflows bitset");

    return implementation::template extract<T, Position, Length>(buffer);
  }

  //*************************************************************************
  /// Get as an unsigned long.
  //*************************************************************************
  unsigned long to_ulong() const {
    GDUT_ASSERT(gdut::integral_limits<unsigned long>::bits >= Active_Bits,
                GDUT_ERROR(gdut::bitset_overflow));

    return implementation::template value<unsigned long>(buffer,
                                                         Number_Of_Elements);
  }

  //*************************************************************************
  /// Get as an unsigned long long.
  //*************************************************************************
  unsigned long long to_ullong() const {
    GDUT_ASSERT(gdut::integral_limits<unsigned long long>::bits >= Active_Bits,
                GDUT_ERROR(gdut::bitset_overflow));

    return implementation::template value<unsigned long long>(
        buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Reset all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &reset() GDUT_NOEXCEPT {
    implementation::reset_all(buffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Reset the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &reset(size_t position) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::reset_position(buffer, position);

    return *this;
  }

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will return
  /// <b>false</b>.
  //*************************************************************************
  GDUT_CONSTEXPR14 bool test(size_t position) const {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), false);

    return implementation::test(buffer, position);
  }

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will not compile.
  //*************************************************************************
  template <size_t Position> GDUT_CONSTEXPR14 bool test() const {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    return implementation::test(buffer, Position);
  }

  //*************************************************************************
  /// The number of bits in the bitset.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t size() GDUT_NOEXCEPT { return Active_Bits; }

  //*************************************************************************
  /// The number of storage elements in the bitset.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t number_of_elements() GDUT_NOEXCEPT {
    return Number_Of_Elements;
  }

  //*************************************************************************
  /// The value of a set element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type all_set_element() GDUT_NOEXCEPT {
    return All_Set_Element;
  }

  //*************************************************************************
  /// The value of a clear element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type all_clear_element() GDUT_NOEXCEPT {
    return All_Clear_Element;
  }

  //*************************************************************************
  /// The number of bits in an element.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t bits_per_element() GDUT_NOEXCEPT {
    return Bits_Per_Element;
  }

  //*************************************************************************
  /// The mask for the msb element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type top_mask() GDUT_NOEXCEPT {
    return Top_Mask;
  }

  //*************************************************************************
  /// The total number of bits of storage, including unused.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t allocated_bits() GDUT_NOEXCEPT {
    return Number_Of_Elements * Bits_Per_Element;
  }

  //*************************************************************************
  /// The storage model for the bitset.
  /// gdut::bitset_storage_model::Single
  /// gdut::bitset_storage_model::Multi
  //*************************************************************************
  static GDUT_CONSTEXPR gdut::bitset_storage_model
  storage_model() GDUT_NOEXCEPT {
    return Storage_Model;
  }

  //*************************************************************************
  /// Count the number of bits set.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t count() const GDUT_NOEXCEPT {
    return implementation::count(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  // Are all the bits sets?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool all() const GDUT_NOEXCEPT {
    return implementation::all(buffer, Number_Of_Elements, Top_Mask);
  }

  //*************************************************************************
  // Are all the mask bits sets?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool all(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::all(buffer, Number_Of_Elements, Top_Mask, mask);
  }

  //*************************************************************************
  /// Are none of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool none() const GDUT_NOEXCEPT {
    return implementation::none(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Are none of the mask bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool none(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::none(buffer, Number_Of_Elements, mask);
  }

  //*************************************************************************
  /// Are any of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool any() const GDUT_NOEXCEPT {
    return implementation::any(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Are any of the mask bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool any(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::any(buffer, Number_Of_Elements, mask);
  }

  //*************************************************************************
  /// Flip all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &flip() GDUT_NOEXCEPT {
    implementation::flip_all(buffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Flip the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &flip(size_t position) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::flip_position(buffer, position);

    return *this;
  }

  //*************************************************************************
  /// Read [] operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bool operator[](size_t position) const GDUT_NOEXCEPT {
    return implementation::test(buffer, position);
  }

  //*************************************************************************
  /// Write [] operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bit_reference operator[](size_t position) GDUT_NOEXCEPT {
    return bit_reference(*this, position);
  }

  //*************************************************************************
  /// Returns a string representing the bitset.
  //*************************************************************************
#if GDUT_USING_CPP11
  template <typename TString = gdut::string<Active_Bits>>
#else
  template <typename TString>
#endif
  GDUT_CONSTEXPR14 TString to_string(
      typename TString::value_type zero = typename TString::value_type('0'),
      typename TString::value_type one =
          typename TString::value_type('1')) const {
    return implementation::template to_string<TString>(buffer, Active_Bits,
                                                       zero, one);
  }

  //*************************************************************************
  /// Finds the first bit in the specified state.
  ///\param state The state to search for.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t find_first(bool state) const GDUT_NOEXCEPT {
    return implementation::find_next(buffer, Number_Of_Elements, Active_Bits,
                                     state, 0);
  }

  //*************************************************************************
  /// Finds the next bit in the specified state.
  ///\param state    The state to search for.
  ///\param position The position to start from.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t find_next(bool state,
                                    size_t position) const GDUT_NOEXCEPT {
    return implementation::find_next(buffer, Number_Of_Elements, Active_Bits,
                                     state, position);
  }

  //*************************************************************************
  /// operator &
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator&(const bitset<Active_Bits, TElement> &other) const GDUT_NOEXCEPT {
    gdut::bitset<Active_Bits, TElement> temp(*this);

    implementation::operator_and(temp.buffer, other.buffer, Number_Of_Elements);

    return temp;
  }

  //*************************************************************************
  /// operator &=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  operator&=(const bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_and(buffer, other.buffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator |
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator|(const bitset<Active_Bits, TElement> &other) const GDUT_NOEXCEPT {
    gdut::bitset<Active_Bits, TElement> temp(*this);

    implementation::operator_or(temp.buffer, other.buffer, Number_Of_Elements);

    return temp;
  }

  //*************************************************************************
  /// operator |=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  operator|=(const bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_or(&buffer[0], &other.buffer[0],
                                Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator ^
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator^(const bitset<Active_Bits, TElement> &other) const GDUT_NOEXCEPT {
    gdut::bitset<Active_Bits, TElement> temp(*this);

    implementation::operator_xor(temp.buffer, other.buffer, Number_Of_Elements);

    return temp;
  }

  //*************************************************************************
  /// operator ^=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  operator^=(const bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_xor(buffer, other.buffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator ~
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator~() const GDUT_NOEXCEPT {
    gdut::bitset<Active_Bits, TElement> temp(*this);

    implementation::flip_all(temp.buffer, Number_Of_Elements);

    return temp;
  }

  //*************************************************************************
  /// operator <<
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator<<(size_t shift) const GDUT_NOEXCEPT {
    gdut::bitset<Active_Bits, TElement> temp(*this);

    implementation::operator_shift_left(temp.buffer, Number_Of_Elements,
                                        Active_Bits, shift);

    return temp;
  }

  //*************************************************************************
  /// operator <<=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  operator<<=(size_t shift) GDUT_NOEXCEPT {
    if (shift >= Active_Bits) {
      implementation::reset_all(buffer, Number_Of_Elements);
    } else {
      implementation::operator_shift_left(buffer, Number_Of_Elements,
                                          Active_Bits, shift);
    }

    return *this;
  }

  //*************************************************************************
  /// operator >>
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement>
  operator>>(size_t shift) const GDUT_NOEXCEPT {
    bitset<Active_Bits, TElement> temp(*this);

    implementation::operator_shift_right(temp.buffer, Number_Of_Elements,
                                         Active_Bits, shift);

    return temp;
  }

  //*************************************************************************
  /// operator >>=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset<Active_Bits, TElement> &
  operator>>=(size_t shift) GDUT_NOEXCEPT {
    if (shift >= Active_Bits) {
      implementation::reset_all(buffer, Number_Of_Elements);
    } else {
      implementation::operator_shift_right(buffer, Number_Of_Elements,
                                           Active_Bits, shift);
    }

    return *this;
  }

  //*************************************************************************
  /// operator ==
  //*************************************************************************
  friend GDUT_CONSTEXPR14 bool
  operator==(const bitset<Active_Bits, TElement> &lhs,
             const bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
    return implementation::operator_equality(lhs.buffer, rhs.buffer,
                                             lhs.Number_Of_Elements);
  }

  //*************************************************************************
  /// operator !=
  //*************************************************************************
  friend GDUT_CONSTEXPR14 bool
  operator!=(const bitset<Active_Bits, TElement> &lhs,
             const bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// swap
  //*************************************************************************
  GDUT_CONSTEXPR14 void
  swap(gdut::bitset<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::swap(buffer, other.buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// span
  /// Returns a span of the underlying buffer.
  //*************************************************************************
  GDUT_CONSTEXPR14 span_type span() GDUT_NOEXCEPT {
    return span_type(buffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// span
  /// Returns a const span of the underlying buffer.
  //*************************************************************************
  GDUT_CONSTEXPR14 const_span_type span() const GDUT_NOEXCEPT {
    return const_span_type(buffer, Number_Of_Elements);
  }

private:
  // The implementation of the bitset functionality.
  typedef gdut::bitset_impl<element_type,
                            (Number_Of_Elements == 1U)
                                ? gdut::bitset_storage_model::Single
                                : gdut::bitset_storage_model::Multi>
      implementation;

  // The storage for the bitset.
  element_type buffer[Number_Of_Elements];
};

//***************************************************************************
/// operator &
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bitset<Active_Bits>
operator&(const bitset<Active_Bits, TElement> &lhs,
          const bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  bitset<Active_Bits> temp(lhs);
  temp &= rhs;
  return temp;
}

//***************************************************************************
/// operator |
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bitset<Active_Bits>
operator|(const bitset<Active_Bits, TElement> &lhs,
          const bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  bitset<Active_Bits> temp(lhs);
  temp |= rhs;
  return temp;
}

//***************************************************************************
/// operator ^
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bitset<Active_Bits>
operator^(const bitset<Active_Bits, TElement> &lhs,
          const bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  bitset<Active_Bits> temp(lhs);
  temp ^= rhs;
  return temp;
}
} // namespace gdut

//***************************************************************************
/// operator !=
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator!=(const gdut::bitset<Active_Bits, TElement> &lhs,
           const gdut::bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// swap
//*************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 void
swap(gdut::bitset<Active_Bits, TElement> &lhs,
     gdut::bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  lhs.swap(rhs);
}

//***************************************************************************
/// bitset_ext
//***************************************************************************
namespace gdut {
//***************************************************************************
template <size_t Active_Bits = 0U, typename TElement = unsigned char>
class bitset_ext;

//***************************************************************************
/// Specialisation for zero bits.
//***************************************************************************
template <>
class bitset_ext<0U, unsigned char>
    : public gdut::private_bitset::bitset_common<0U, unsigned char> {
public:
  typedef size_t size_type;

  typedef gdut::private_bitset::bitset_common<0U, unsigned char>::element_type
      element_type;
  typedef gdut::private_bitset::bitset_common<0U, unsigned char>::span_type
      span_type;
  typedef gdut::private_bitset::bitset_common<
      0U, unsigned char>::const_span_type const_span_type;

  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::Bits_Per_Element;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::All_Set_Element;
  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::All_Clear_Element;
  using gdut::private_bitset::bitset_common<0U,
                                            unsigned char>::Number_Of_Elements;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Size;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Storage_Model;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Top_Mask;
  using gdut::private_bitset::bitset_common<0U, unsigned char>::Allocated_Bits;
};

//*************************************************************************
/// A bitset that uses externally declared storage.
//*************************************************************************
template <size_t Active_Bits, typename TElement>
class bitset_ext
    : public gdut::private_bitset::bitset_common<Active_Bits, TElement> {
public:
  GDUT_STATIC_ASSERT(gdut::is_unsigned<TElement>::value,
                     "The element type must be unsigned");

  typedef size_t size_type;

  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::element_type element_type;
  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::span_type span_type;
  typedef typename gdut::private_bitset::bitset_common<
      Active_Bits, TElement>::const_span_type const_span_type;

  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Bits_Per_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::All_Set_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::All_Clear_Element;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Number_Of_Elements;
  using gdut::private_bitset::bitset_common<Active_Bits, TElement>::Size;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Storage_Model;
  using gdut::private_bitset::bitset_common<Active_Bits, TElement>::Top_Mask;
  using gdut::private_bitset::bitset_common<Active_Bits,
                                            TElement>::Allocated_Bits;

  typedef gdut::array<TElement, Number_Of_Elements> buffer_type;

  //*************************************************************************
  /// The reference type returned.
  //*************************************************************************
  class bit_reference {
  public:
    friend class bitset_ext;

    //*******************************
    /// Copy constructor
    //*******************************
    GDUT_CONSTEXPR14 bit_reference(const bit_reference &other) GDUT_NOEXCEPT
        : p_bitset(other.p_bitset),
          position(other.position) {}

    //*******************************
    /// Conversion operator.
    //*******************************
    GDUT_CONSTEXPR14 operator bool() const GDUT_NOEXCEPT {
      return p_bitset->test(position);
    }

    //*******************************
    /// Assignment operator.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &operator=(bool b) GDUT_NOEXCEPT {
      p_bitset->set(position, b);
      return *this;
    }

    //*******************************
    /// Assignment operator.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &
    operator=(const bit_reference &r) GDUT_NOEXCEPT {
      p_bitset->set(position, bool(r));
      return *this;
    }

    //*******************************
    /// Flip the bit.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference &flip() GDUT_NOEXCEPT {
      p_bitset->flip(position);
      return *this;
    }

    //*******************************
    /// Return the logical inverse of the bit.
    //*******************************
    GDUT_CONSTEXPR14 bool operator~() const GDUT_NOEXCEPT {
      return !p_bitset->test(position);
    }

  private:
    //*******************************
    /// Default constructor.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference() GDUT_NOEXCEPT : p_bitset(GDUT_NULLPTR),
                                                     position(0) {}

    //*******************************
    /// Constructor.
    //*******************************
    GDUT_CONSTEXPR14 bit_reference(bitset_ext<Active_Bits, TElement> &r_bitset,
                                   size_t position_) GDUT_NOEXCEPT
        : p_bitset(&r_bitset),
          position(position_) {}

    bitset_ext<Active_Bits, TElement> *p_bitset; ///< The bitset.
    size_t position; ///< The position in the bitset.
  };

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR14 explicit bitset_ext(element_type *pbuffer_)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::reset_all(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR14 explicit bitset_ext(buffer_type &buffer)
      : pbuffer(buffer.data()) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::reset_all(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Construct copy.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext(const bitset_ext<Active_Bits, TElement> &other,
                              element_type *pbuffer_)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::operator_assignment(pbuffer, other.pbuffer,
                                        Number_Of_Elements);
  }

  //*************************************************************************
  /// Construct copy.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext(const bitset_ext<Active_Bits, TElement> &other,
                              buffer_type &buffer) GDUT_NOEXCEPT
      : pbuffer(buffer.data()) {
    implementation::operator_assignment(pbuffer, other.pbuffer,
                                        Number_Of_Elements);
  }

  //*************************************************************************
  /// Copy Constructor (Deleted).
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext(const bitset_ext<Active_Bits, TElement> &other)
      GDUT_NOEXCEPT GDUT_DELETE;

  //*************************************************************************
  /// Construct from a value.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext(unsigned long long value, element_type *pbuffer_)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::template initialise<element_type>(
        pbuffer, Number_Of_Elements, value);
  }

  //*************************************************************************
  /// Construct from a value.
  //*************************************************************************
  template <typename TValue>
  GDUT_CONSTEXPR14
  bitset_ext(TValue value, buffer_type &buffer,
             typename gdut::enable_if<is_integral<TValue>::value>::type * = 0)
      GDUT_NOEXCEPT : pbuffer(buffer.data()) {
    implementation::template initialise<element_type>(
        pbuffer, Number_Of_Elements, value);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, element_type *pbuffer_,
      typename gdut::enable_if<is_same<TPString, const char *>::value>::type * =
          0)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, buffer_type &buffer,
      typename gdut::enable_if<is_same<TPString, const char *>::value>::type * =
          0) GDUT_NOEXCEPT : pbuffer(buffer.data()) {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, element_type *pbuffer_,
      typename gdut::enable_if<is_same<TPString, const wchar_t *>::value>::type
          * = 0)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, buffer_type &buffer,
      typename gdut::enable_if<is_same<TPString, const wchar_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : pbuffer(buffer.data()) {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, element_type *pbuffer_,
      typename gdut::enable_if<is_same<TPString, const char16_t *>::value>::type
          * = 0)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, buffer_type &buffer,
      typename gdut::enable_if<is_same<TPString, const char16_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : pbuffer(buffer.data()) {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, element_type *pbuffer_,
      typename gdut::enable_if<is_same<TPString, const char32_t *>::value>::type
          * = 0)
      : pbuffer(pbuffer_) {
    GDUT_ASSERT(pbuffer != GDUT_NULLPTR,
                GDUT_ERROR(gdut::bitset_invalid_buffer));
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Construct from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14 bitset_ext(
      TPString text, buffer_type &buffer,
      typename gdut::enable_if<is_same<TPString, const char32_t *>::value>::type
          * = 0) GDUT_NOEXCEPT : pbuffer(buffer.data()) {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);
  }

  //*************************************************************************
  /// Assignment operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext &
  operator=(const bitset_ext<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_assignment(pbuffer, other.pbuffer,
                                        Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Set all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &set() GDUT_NOEXCEPT {
    implementation::set_all(pbuffer, Number_Of_Elements, Top_Mask);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14
  bitset_ext<Active_Bits, TElement> &set(size_t position, bool value = true) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::set_position(pbuffer, position, value);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position>
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &set(bool value = true) {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    implementation::template set_position<Position>(pbuffer, value);

    return *this;
  }

  //*************************************************************************
  /// Set the bit at the position.
  //*************************************************************************
  template <size_t Position, bool Value>
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &set() {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    implementation::template set_position<Position, Value>(pbuffer);

    return *this;
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char *>::value,
                               bitset_ext<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const wchar_t *>::value,
                               bitset_ext<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a char16 string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char16_t *>::value,
                               bitset_ext<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a char32 string.
  //*************************************************************************
  template <typename TPString>
  GDUT_CONSTEXPR14
      typename gdut::enable_if<gdut::is_same<TPString, const char32_t *>::value,
                               bitset_ext<Active_Bits, TElement> &>::type
      set(TPString text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  from_string(const char *text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a wide string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  from_string(const wchar_t *text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a u16 string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  from_string(const char16_t *text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Set from a u32 string.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  from_string(const char32_t *text) GDUT_NOEXCEPT {
    implementation::from_string(pbuffer, Number_Of_Elements, Active_Bits, text);

    return *this;
  }

  //*************************************************************************
  /// Get as an integral value.
  //*************************************************************************
  template <typename T> GDUT_CONSTEXPR14 T value() const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");
    GDUT_STATIC_ASSERT(gdut::integral_limits<T>::bits >=
                           (Number_Of_Elements * Bits_Per_Element),
                       "Type too small");

    return implementation::template value<T>(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  /// Run time position and length.
  //*************************************************************************
  template <typename T>
  GDUT_CONSTEXPR14 T extract(size_t position,
                             size_t length = gdut::integral_limits<T>::bits) {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");

    GDUT_ASSERT_OR_RETURN_VALUE(length <= gdut::integral_limits<T>::bits,
                                GDUT_ERROR(bitset_overflow), 0);
    GDUT_ASSERT_OR_RETURN_VALUE((position + length) <= Active_Bits,
                                GDUT_ERROR(bitset_overflow), 0);

    return implementation::template extract<T>(pbuffer, position, length);
  }

  //*************************************************************************
  /// Extract an integral value from an arbitrary position and length.
  /// Compile time position and length.
  //*************************************************************************
#if GDUT_USING_CPP11
  template <typename T, size_t Position,
            size_t Length = gdut::integral_limits<T>::bits>
#else
  template <typename T, size_t Position, size_t Length>
#endif
  GDUT_CONSTEXPR14 T extract() const {
    GDUT_STATIC_ASSERT(gdut::is_integral<T>::value,
                       "Only integral types are supported");
    GDUT_STATIC_ASSERT(Length <= gdut::integral_limits<T>::bits,
                       "Length is larger that the required type");
    GDUT_STATIC_ASSERT((Position + Length) <= Active_Bits,
                       "Position/Length overflows bitset");

    return implementation::template extract<T, Position, Length>(pbuffer);
  }

  //*************************************************************************
  /// Get as an unsigned long.
  //*************************************************************************
  unsigned long to_ulong() const {
    GDUT_ASSERT(gdut::integral_limits<unsigned long>::bits >= Active_Bits,
                GDUT_ERROR(gdut::bitset_overflow));

    return implementation::template value<unsigned long>(pbuffer,
                                                         Number_Of_Elements);
  }

  //*************************************************************************
  /// Get as an unsigned long long.
  //*************************************************************************
  unsigned long long to_ullong() const {
    GDUT_ASSERT(gdut::integral_limits<unsigned long long>::bits >= Active_Bits,
                GDUT_ERROR(gdut::bitset_overflow));

    return implementation::template value<unsigned long long>(
        pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Reset all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &reset() GDUT_NOEXCEPT {
    implementation::reset_all(pbuffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Reset the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &reset(size_t position) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::reset_position(pbuffer, position);

    return *this;
  }

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will return
  /// <b>false</b>.
  //*************************************************************************
  GDUT_CONSTEXPR14 bool test(size_t position) const {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), false);

    return implementation::test(pbuffer, position);
  }

  //*************************************************************************
  /// Tests a bit at a position.
  /// Positions greater than the number of configured bits will not compile.
  //*************************************************************************
  template <size_t Position> GDUT_CONSTEXPR14 bool test() const {
    GDUT_STATIC_ASSERT(Position < Active_Bits, "Position out of bounds");

    return implementation::test(pbuffer, Position);
  }

  //*************************************************************************
  /// The number of bits in the bitset.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t size() GDUT_NOEXCEPT { return Active_Bits; }

  //*************************************************************************
  /// The number of storage elements in the bitset.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t number_of_elements() GDUT_NOEXCEPT {
    return Number_Of_Elements;
  }

  //*************************************************************************
  /// The value of a set element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type all_set_element() GDUT_NOEXCEPT {
    return All_Set_Element;
  }

  //*************************************************************************
  /// The value of a clear element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type all_clear_element() GDUT_NOEXCEPT {
    return All_Clear_Element;
  }

  //*************************************************************************
  /// The mask for the msb element.
  //*************************************************************************
  static GDUT_CONSTEXPR element_type top_mask() GDUT_NOEXCEPT {
    return Top_Mask;
  }

  //*************************************************************************
  /// The number of bits in an element.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t bits_per_element() GDUT_NOEXCEPT {
    return Bits_Per_Element;
  }

  //*************************************************************************
  /// The total number of bits of storage, including unused.
  //*************************************************************************
  static GDUT_CONSTEXPR size_t allocated_bits() GDUT_NOEXCEPT {
    return Number_Of_Elements * Bits_Per_Element;
  }

  //*************************************************************************
  /// The storage model for the bitset.
  /// gdut::bitset_storage_model::Single
  /// gdut::bitset_storage_model::Multi
  //*************************************************************************
  static GDUT_CONSTEXPR gdut::bitset_storage_model
  storage_model() GDUT_NOEXCEPT {
    return Storage_Model;
  }

  //*************************************************************************
  /// Count the number of bits set.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t count() const GDUT_NOEXCEPT {
    return implementation::count(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  // Are all the bits sets?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool all() const GDUT_NOEXCEPT {
    return implementation::all(pbuffer, Number_Of_Elements, Top_Mask);
  }

  //*************************************************************************
  // Are all the mask bits sets?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool all(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::all(pbuffer, Number_Of_Elements, Top_Mask, mask);
  }

  //*************************************************************************
  /// Are none of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool none() const GDUT_NOEXCEPT {
    return implementation::none(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Are none of the mask bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool none(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::none(pbuffer, Number_Of_Elements, mask);
  }

  //*************************************************************************
  /// Are any of the bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool any() const GDUT_NOEXCEPT {
    return implementation::any(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// Are any of the mask bits set?
  //*************************************************************************
  GDUT_CONSTEXPR14 bool any(element_type mask) const GDUT_NOEXCEPT {
    GDUT_STATIC_ASSERT(Storage_Model == gdut::bitset_storage_model::Single,
                       "Not supported for 'Multi' bitset storage model");

    return implementation::any(pbuffer, Number_Of_Elements, mask);
  }

  //*************************************************************************
  /// Flip all of the bits.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &flip() GDUT_NOEXCEPT {
    implementation::flip_all(pbuffer, Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// Flip the bit at the position.
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &flip(size_t position) {
    GDUT_ASSERT_OR_RETURN_VALUE(position < Active_Bits,
                                GDUT_ERROR(bitset_overflow), *this);

    implementation::flip_position(pbuffer, position);

    return *this;
  }

  //*************************************************************************
  /// Read [] operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bool operator[](size_t position) const GDUT_NOEXCEPT {
    return implementation::test(pbuffer, position);
  }

  //*************************************************************************
  /// Write [] operator.
  //*************************************************************************
  GDUT_CONSTEXPR14 bit_reference operator[](size_t position) GDUT_NOEXCEPT {
    return bit_reference(*this, position);
  }

  //*************************************************************************
  /// Returns a string representing the bitset.
  //*************************************************************************
#if GDUT_USING_CPP11
  template <typename TString = gdut::string<Active_Bits>>
#else
  template <typename TString>
#endif
  GDUT_CONSTEXPR14 TString to_string(
      typename TString::value_type zero = typename TString::value_type('0'),
      typename TString::value_type one =
          typename TString::value_type('1')) const {
    return implementation::template to_string<TString>(pbuffer, Active_Bits,
                                                       zero, one);
  }

  //*************************************************************************
  /// Finds the first bit in the specified state.
  ///\param state The state to search for.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t find_first(bool state) const GDUT_NOEXCEPT {
    return implementation::find_next(pbuffer, Number_Of_Elements, Active_Bits,
                                     state, 0);
  }

  //*************************************************************************
  /// Finds the next bit in the specified state.
  ///\param state    The state to search for.
  ///\param position The position to start from.
  ///\returns The position of the bit or npos if none were found.
  //*************************************************************************
  GDUT_CONSTEXPR14 size_t find_next(bool state,
                                    size_t position) const GDUT_NOEXCEPT {
    return implementation::find_next(pbuffer, Number_Of_Elements, Active_Bits,
                                     state, position);
  }

  //*************************************************************************
  /// operator &=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  operator&=(const bitset_ext<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_and(&pbuffer[0], &other.pbuffer[0],
                                 Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator |=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  operator|=(const bitset_ext<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_or(&pbuffer[0], &other.pbuffer[0],
                                Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator ^=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  operator^=(const bitset_ext<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::operator_xor(&pbuffer[0], &other.pbuffer[0],
                                 Number_Of_Elements);

    return *this;
  }

  //*************************************************************************
  /// operator <<=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  operator<<=(size_t shift) GDUT_NOEXCEPT {
    implementation::operator_shift_left(pbuffer, Number_Of_Elements,
                                        Active_Bits, shift);

    return *this;
  }

  //*************************************************************************
  /// operator >>=
  //*************************************************************************
  GDUT_CONSTEXPR14 bitset_ext<Active_Bits, TElement> &
  operator>>=(size_t shift) GDUT_NOEXCEPT {
    implementation::operator_shift_right(pbuffer, Number_Of_Elements,
                                         Active_Bits, shift);

    return *this;
  }

  //*************************************************************************
  /// operator ==
  //*************************************************************************
  friend GDUT_CONSTEXPR14 bool
  operator==(const bitset_ext<Active_Bits, TElement> &lhs,
             const bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
    return implementation::operator_equality(lhs.pbuffer, rhs.pbuffer,
                                             lhs.Number_Of_Elements);
  }

  //*************************************************************************
  /// operator !=
  //*************************************************************************
  friend GDUT_CONSTEXPR14 bool
  operator!=(const bitset_ext<Active_Bits, TElement> &lhs,
             const bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// swap
  //*************************************************************************
  GDUT_CONSTEXPR14 void
  swap(gdut::bitset_ext<Active_Bits, TElement> &other) GDUT_NOEXCEPT {
    implementation::swap(pbuffer, other.pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// span
  /// Returns a span of the underlying pbuffer.
  //*************************************************************************
  GDUT_CONSTEXPR14 span_type span() GDUT_NOEXCEPT {
    return span_type(pbuffer, Number_Of_Elements);
  }

  //*************************************************************************
  /// span
  /// Returns a const span of the underlying pbuffer.
  //*************************************************************************
  GDUT_CONSTEXPR14 const_span_type span() const GDUT_NOEXCEPT {
    return const_span_type(pbuffer, Number_Of_Elements);
  }

private:
  // The implementation of the bitset functionality.
  typedef gdut::bitset_impl<element_type,
                            (Number_Of_Elements == 1U)
                                ? gdut::bitset_storage_model::Single
                                : gdut::bitset_storage_model::Multi>
      implementation;

  // Pointer to the storage for the bitset.
  element_type *pbuffer;
};
} // namespace gdut

//***************************************************************************
/// operator !=
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator!=(const gdut::bitset_ext<Active_Bits, TElement> &lhs,
           const gdut::bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//*************************************************************************
/// swap
//*************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 void
swap(gdut::bitset_ext<Active_Bits, TElement> &lhs,
     gdut::bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  lhs.swap(rhs);
}

namespace gdut {
namespace private_bitset {
//*************************************************************************
/// Compare bitset spans.
/// Assumes that the lhs span's element type is larger than rhs's.
//*************************************************************************
template <typename TLhsSpan, typename TRhsSpan>
bool compare_bitset_spans(const TLhsSpan &lhs_span, const TRhsSpan &rhs_span) {
  typedef typename TLhsSpan::value_type lhs_element_t;
  typedef typename TRhsSpan::value_type rhs_element_t;

  const int steps =
      static_cast<int>(sizeof(lhs_element_t) / sizeof(rhs_element_t));

  typename TLhsSpan::iterator lhs_itr = lhs_span.begin();
  typename TRhsSpan::iterator rhs_itr = rhs_span.begin();

  while (lhs_itr != lhs_span.end()) {
    const lhs_element_t &lhs_value = *lhs_itr;

    // Build the rhs element in terms of the lhs element type.
    lhs_element_t rhs_value = 0;

    const int shift_step = gdut::integral_limits<rhs_element_t>::bits;
    int shift = 0;

    for (int i = 0; i < steps; ++i) {
      rhs_value |= (static_cast<lhs_element_t>(*rhs_itr) << shift);
      ++rhs_itr;
      shift += shift_step;
    }

    if (lhs_value != rhs_value) {
      return false;
    }

    ++lhs_itr;
  }

  return true;
}
} // namespace private_bitset
} // namespace gdut

//***************************************************************************
/// operator ==
/// bitset
/// Different element types
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    !gdut::is_same<TLhsElement, TRhsElement>::value, bool>::type
operator==(const gdut::bitset<Active_Bits, TLhsElement> &lhs,
           const gdut::bitset<Active_Bits, TRhsElement> &rhs) GDUT_NOEXCEPT {
  // Get a span of each type.
  typename gdut::bitset<Active_Bits, TLhsElement>::const_span_type lhs_span =
      lhs.span();
  typename gdut::bitset<Active_Bits, TRhsElement>::const_span_type rhs_span =
      rhs.span();

  // Put the bitset with the largest element type as the first argument.
  if GDUT_IF_CONSTEXPR (sizeof(TLhsElement) > sizeof(TRhsElement)) {
    return gdut::private_bitset::compare_bitset_spans(lhs_span, rhs_span);
  } else {
    return gdut::private_bitset::compare_bitset_spans(rhs_span, lhs_span);
  }
}

//***************************************************************************
/// operator !=
/// bitset
/// Different element types
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    !gdut::is_same<TLhsElement, TRhsElement>::value, bool>::type
operator!=(const gdut::bitset<Active_Bits, TLhsElement> &lhs,
           const gdut::bitset<Active_Bits, TRhsElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***************************************************************************
/// operator ==
/// bitset_ext
/// Different element types
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14
    typename gdut::enable_if<!gdut::is_same<TLhsElement, TRhsElement>::value,
                             bool>::type
    operator==(const gdut::bitset_ext<Active_Bits, TLhsElement> &lhs,
               const gdut::bitset_ext<Active_Bits, TRhsElement> &rhs)
        GDUT_NOEXCEPT {
  // Get a span of each type.
  typename gdut::bitset_ext<Active_Bits, TLhsElement>::const_span_type
      lhs_span = lhs.span();
  typename gdut::bitset_ext<Active_Bits, TRhsElement>::const_span_type
      rhs_span = rhs.span();

  // Put the bitset with the largest element type as the first argument.
  if GDUT_IF_CONSTEXPR (sizeof(TLhsElement) > sizeof(TRhsElement)) {
    return gdut::private_bitset::compare_bitset_spans(lhs_span, rhs_span);
  } else {
    return gdut::private_bitset::compare_bitset_spans(rhs_span, lhs_span);
  }
}

//***************************************************************************
/// operator !=
/// bitset_ext
/// Different element types
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14
    typename gdut::enable_if<!gdut::is_same<TLhsElement, TRhsElement>::value,
                             bool>::type
    operator!=(const gdut::bitset_ext<Active_Bits, TLhsElement> &lhs,
               const gdut::bitset_ext<Active_Bits, TRhsElement> &rhs)
        GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***************************************************************************
/// operator ==
/// bitset compared with bitset_ext, same element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator==(const gdut::bitset<Active_Bits, TElement> &lhs,
           const gdut::bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  const char Storage_Model = gdut::bitset<Active_Bits, TElement>::Storage_Model;
  const size_t Number_Of_Elements =
      gdut::bitset<Active_Bits, TElement>::Number_Of_Elements;

  typename gdut::bitset<Active_Bits, TElement>::const_span_type lhs_span =
      lhs.span();
  typename gdut::bitset_ext<Active_Bits, TElement>::const_span_type rhs_span =
      rhs.span();

  typedef gdut::bitset_impl<TElement, Storage_Model> implementation;

  return implementation::operator_equality(lhs_span.begin(), rhs_span.begin(),
                                           Number_Of_Elements);
}

//***************************************************************************
/// operator !=
/// bitset compared with bitset_ext, same element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator!=(const gdut::bitset<Active_Bits, TElement> &lhs,
           const gdut::bitset_ext<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***************************************************************************
/// operator ==
/// bitset_ext compared with bitset, same element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator==(const gdut::bitset_ext<Active_Bits, TElement> &lhs,
           const gdut::bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  const char Storage_Model = gdut::bitset<Active_Bits, TElement>::Storage_Model;
  const size_t Number_Of_Elements =
      gdut::bitset<Active_Bits, TElement>::Number_Of_Elements;

  typename gdut::bitset_ext<Active_Bits, TElement>::const_span_type lhs_span =
      lhs.span();
  typename gdut::bitset<Active_Bits, TElement>::const_span_type rhs_span =
      rhs.span();

  typedef gdut::bitset_impl<TElement, Storage_Model> implementation;

  return implementation::operator_equality(lhs_span.begin(), rhs_span.begin(),
                                           Number_Of_Elements);
}

//***************************************************************************
/// operator !=
/// bitset_ext compared with bitset, same element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TElement>
GDUT_CONSTEXPR14 bool
operator!=(const gdut::bitset_ext<Active_Bits, TElement> &lhs,
           const gdut::bitset<Active_Bits, TElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***************************************************************************
/// operator ==
/// bitset compared with bitset_ext, different element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14
    typename gdut::enable_if<!gdut::is_same<TLhsElement, TRhsElement>::value,
                             bool>::type
    operator==(const gdut::bitset<Active_Bits, TLhsElement> &lhs,
               const gdut::bitset_ext<Active_Bits, TRhsElement> &rhs)
        GDUT_NOEXCEPT {
  // Get a span of each type.
  typename gdut::bitset<Active_Bits, TLhsElement>::const_span_type lhs_span =
      lhs.span();
  typename gdut::bitset_ext<Active_Bits, TRhsElement>::const_span_type
      rhs_span = rhs.span();

  // Put the bitset with the largest element type as the first argument.
  if GDUT_IF_CONSTEXPR (sizeof(TLhsElement) > sizeof(TRhsElement)) {
    return gdut::private_bitset::compare_bitset_spans(lhs_span, rhs_span);
  } else {
    return gdut::private_bitset::compare_bitset_spans(rhs_span, lhs_span);
  }
}

//***************************************************************************
/// operator !=
/// bitset compared with bitset_ext, different element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14
    typename gdut::enable_if<!gdut::is_same<TLhsElement, TRhsElement>::value,
                             bool>::type
    operator!=(const gdut::bitset<Active_Bits, TLhsElement> &lhs,
               const gdut::bitset_ext<Active_Bits, TRhsElement> &rhs)
        GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

//***************************************************************************
/// operator ==
/// bitset_ext compared with bitset, different element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    !gdut::is_same<TLhsElement, TRhsElement>::value, bool>::type
operator==(const gdut::bitset_ext<Active_Bits, TLhsElement> &lhs,
           const gdut::bitset<Active_Bits, TRhsElement> &rhs) GDUT_NOEXCEPT {
  // Get a span of each type.
  typename gdut::bitset_ext<Active_Bits, TLhsElement>::const_span_type
      lhs_span = lhs.span();
  typename gdut::bitset<Active_Bits, TRhsElement>::const_span_type rhs_span =
      rhs.span();

  // Put the bitset with the largest element type as the first argument.
  if GDUT_IF_CONSTEXPR (sizeof(TLhsElement) > sizeof(TRhsElement)) {
    return gdut::private_bitset::compare_bitset_spans(lhs_span, rhs_span);
  } else {
    return gdut::private_bitset::compare_bitset_spans(rhs_span, lhs_span);
  }
}

//***************************************************************************
/// operator !=
/// bitset_ext compared with bitset, different element types.
///\ingroup bitset
//***************************************************************************
template <size_t Active_Bits, typename TLhsElement, typename TRhsElement>
GDUT_CONSTEXPR14 typename gdut::enable_if<
    !gdut::is_same<TLhsElement, TRhsElement>::value, bool>::type
operator!=(const gdut::bitset_ext<Active_Bits, TLhsElement> &lhs,
           const gdut::bitset<Active_Bits, TRhsElement> &rhs) GDUT_NOEXCEPT {
  return !(lhs == rhs);
}

#include "minmax_pop.hpp"

#endif
