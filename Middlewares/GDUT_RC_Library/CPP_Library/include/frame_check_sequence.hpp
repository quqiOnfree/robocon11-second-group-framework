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

#ifndef GDUT_FRAME_CHECK_SEQUENCE_INCLUDED
#define GDUT_FRAME_CHECK_SEQUENCE_INCLUDED

#include "binary.hpp"
#include "iterator.hpp"
#include "platform.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"

#include <stdint.h>

GDUT_STATIC_ASSERT(
    GDUT_USING_8BIT_TYPES,
    "This file does not currently support targets with no 8bit type");

///\defgroup frame_check_sequence Frame check sequence calculation
///\ingroup maths

namespace gdut {
namespace private_frame_check_sequence {
//***************************************************
/// add_insert_iterator
/// An output iterator used to add new values.
//***************************************************
template <typename TFrame_Check_Sequence>
class add_insert_iterator
    : public gdut::iterator<GDUT_OR_STD::output_iterator_tag,
                            typename TFrame_Check_Sequence::value_type> {
public:
  //***********************************
  explicit add_insert_iterator(TFrame_Check_Sequence &fcs) GDUT_NOEXCEPT
      : p_fcs(&fcs) {}

  //***********************************
  add_insert_iterator &operator*() GDUT_NOEXCEPT { return *this; }

  //***********************************
  add_insert_iterator &operator++() GDUT_NOEXCEPT { return *this; }

  //***********************************
  add_insert_iterator &operator++(int) GDUT_NOEXCEPT { return *this; }

  //***********************************
  add_insert_iterator &operator=(uint8_t value) {
    p_fcs->add(value);
    return *this;
  }

private:
  TFrame_Check_Sequence *p_fcs;
};
} // namespace private_frame_check_sequence

//***************************************************************************
/// Calculates a frame check sequence according to the specified policy.
///\tparam TPolicy The type used to enact the policy.
///\ingroup frame_check_sequence
//***************************************************************************
template <typename TPolicy> class frame_check_sequence {
public:
  typedef TPolicy policy_type;
  typedef typename policy_type::value_type value_type;
  typedef private_frame_check_sequence::add_insert_iterator<
      frame_check_sequence<TPolicy>>
      add_insert_iterator;

  GDUT_STATIC_ASSERT(gdut::is_unsigned<value_type>::value,
                     "Signed frame check type not supported");

  //*************************************************************************
  /// Default constructor.
  //*************************************************************************
  GDUT_CONSTEXPR14 frame_check_sequence() : frame_check() { reset(); }

  //*************************************************************************
  /// Constructor from range.
  /// \param begin Start of the range.
  /// \param end   End of the range.
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR14 frame_check_sequence(TIterator begin, const TIterator end)
      : frame_check() {
    GDUT_STATIC_ASSERT(
        sizeof(typename gdut::iterator_traits<TIterator>::value_type) == 1,
        "Type not supported");

    reset();
    add(begin, end);
  }

  //*************************************************************************
  /// Resets the FCS to the initial state.
  //*************************************************************************
  GDUT_CONSTEXPR14 void reset() { frame_check = policy.initial(); }

  //*************************************************************************
  /// Adds a range.
  /// \param begin
  /// \param end
  //*************************************************************************
  template <typename TIterator>
  GDUT_CONSTEXPR14 void add(TIterator begin, const TIterator end) {
    GDUT_STATIC_ASSERT(
        sizeof(typename gdut::iterator_traits<TIterator>::value_type) == 1,
        "Type not supported");

    while (begin != end) {
      frame_check = policy.add(frame_check, *begin);
      ++begin;
    }
  }

  //*************************************************************************
  /// \param value The uint8_t to add to the FCS.
  //*************************************************************************
  GDUT_CONSTEXPR14 void add(uint8_t value_) {
    frame_check = policy.add(frame_check, value_);
  }

  //*************************************************************************
  /// Gets the FCS value.
  //*************************************************************************
  GDUT_CONSTEXPR14 value_type value() const {
    return policy.final(frame_check);
  }

  //*************************************************************************
  /// Conversion operator to value_type.
  //*************************************************************************
  GDUT_CONSTEXPR14 operator value_type() const {
    return policy.final(frame_check);
  }

  //*************************************************************************
  /// Gets an add_insert_iterator for input.
  //*************************************************************************
  GDUT_CONSTEXPR14 add_insert_iterator input() {
    return add_insert_iterator(*this);
  }

private:
  value_type frame_check;
  policy_type policy;
};
} // namespace gdut

#endif
