///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2025 John Wellbelove, Mark Kitson

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

#ifndef GDUT_SIGNAL_INCLUDED
#define GDUT_SIGNAL_INCLUDED

#include <cstddef>

#include "platform.hpp"

#if GDUT_NOT_USING_CPP11 && !defined(GDUT_IN_UNIT_TEST)
#error NOT SUPPORTED FOR C++03 OR BELOW
#endif

#if GDUT_USING_CPP11

#include "exception.hpp"
#include "error_handler.hpp"
#include "delegate.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"
#include "initializer_list.hpp"
#include "span.hpp"
#include "file_error_numbers.hpp"

//*****************************************************************************
///\defgroup signal signal
/// Container that handles storing and calling callbacks.
///\ingroup containers
//*****************************************************************************

namespace gdut
{
  //***************************************************************************
  ///\ingroup signal
  /// The base class for signal exceptions.
  //***************************************************************************
  class signal_exception : public exception
  {
  public:
    signal_exception(string_type reason_, string_type file_name_, numeric_type line_number_) 
      : exception{reason_, file_name_, line_number_}
    {
    }
  };

  //***************************************************************************
  ///\ingroup signal
  /// Signal full exception.
  //***************************************************************************
  class signal_full : public signal_exception
  {
  public:
    signal_full(string_type file_name_, numeric_type line_number_)
      : signal_exception{GDUT_ERROR_TEXT("signal:full", GDUT_SIGNAL_FILE_ID"A"), file_name_, line_number_}
    {
    }
  };

  //***************************************************************************
  ///\ingroup signal
  ///
  ///\brief A lightweight signal class designed for efficient memory usage and 
  /// ability to store in ROM.
  ///
  ///\tparam TFunction: Callback signature.
  ///\tparam Size:      Maximum number of slots that can be connected to the signal.
  ///\tparam TSlot:     Function-object type or container type that can be invoked. Default gdut::delegate<TFunction>.
  //***************************************************************************
  template <typename TFunction, size_t Size, typename TSlot = gdut::delegate<TFunction>>
  class signal
  {
  public:

    using slot_type = TSlot;
    using size_type = size_t;
    using span_type = gdut::span<const slot_type>;

    //*************************************************************************
    ///\brief  Construct the signal from a variadic list of slots.
    ///
    ///\param slots: Variadic list of slots.
    //*************************************************************************
    template <typename... TSlots>
    GDUT_CONSTEXPR14 explicit signal(TSlots&&... slots) GDUT_NOEXCEPT
      : slot_list{gdut::forward<TSlots>(slots)...}
      , slot_list_end{slot_list + sizeof...(slots)}
    {
      static_assert((gdut::are_all_same<slot_type, gdut::decay_t<TSlots>...>::value), "All slots must be slot_type");
      static_assert(sizeof...(slots) <= Size, "Number of slots exceeds capacity");
    }

    //*************************************************************************
    ///\brief Connects a slot to the signal.
    /// Ignores the slot if it has already been connected.
    ///
    ///\param slot: The slot to connect.
    ///\return <b>false</b> if not all slots could be connected.
    //*************************************************************************
    bool connect(const slot_type& slot) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
    {
      if (!connected(slot))
      {
        GDUT_ASSERT_OR_RETURN_VALUE(!full(), GDUT_ERROR(signal_full), false);
        append_slot(slot);
      }

      return true;
    }

#if GDUT_HAS_INITIALIZER_LIST && GDUT_USING_CPP17
    //*************************************************************************
    ///\brief Connects slots to the signal.
    /// Ignores the slots if it has already been connected.
    ///
    ///\param slots: std::initializer_list of slots to connect.
    ///\return <b>false</b> if not all slots could be connected.
    //*************************************************************************
    bool connect(std::initializer_list<const slot_type> slots) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
    {
      for (const slot_type& slot : slots)
      {
        if (!connected(slot))
        {
          GDUT_ASSERT_OR_RETURN_VALUE(!full(), GDUT_ERROR(signal_full), false);
          append_slot(slot);
        }
      }

      return true;
    }
#endif

    //*************************************************************************
    ///\brief Connects slots to the signal.
    /// Ignores the slots if it has already been connected.
    ///
    ///\param slots: gdut::span of slots to connect.
    ///\return <b>false</b> if not all slots could be connected.
    //*************************************************************************
    bool connect(const span_type slots) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
    {
      for (const slot_type& slot : slots)
      {
        if (!connected(slot))
        {
          GDUT_ASSERT_OR_RETURN_VALUE(!full(), GDUT_ERROR(signal_full), false);
          append_slot(slot);
        }
      }

      return true;
    }

    //*************************************************************************
    ///\brief Disconnects a slot from the signal.
    ///
    ///\param slot: To disconnect.
    //*************************************************************************
    void disconnect(const slot_type& slot) GDUT_NOEXCEPT
    {
      const auto end_itr = end();
      const auto itr     = gdut::find(begin(), end_itr, slot);

      if (itr != end_itr)
      {
        // Shifts all elements after 'itr' one position to the left.
        gdut::copy(gdut::next(itr), end_itr, itr);
        slot_list_end = gdut::prev(slot_list_end);
      }
    }

#if GDUT_HAS_INITIALIZER_LIST && GDUT_USING_CPP17
    //*************************************************************************
    ///\brief Disconnects multiple slots from the signal.
    ///
    ///\param slot: std::intializer_list of slots to disconnect.
    //*************************************************************************
    void disconnect(std::initializer_list<const slot_type> slots) GDUT_NOEXCEPT
    {
      for (const slot_type& slot : slots)
      {
        disconnect(slot);
      }
    }
#endif

    //*************************************************************************
    ///\brief Disconnects multiple slots from the signal.
    ///
    ///\param slot: gdut::span of slots to disconnect.
    //*************************************************************************
    void disconnect(const span_type slots) GDUT_NOEXCEPT
    {
      for (const slot_type& slot : slots)
      {
        disconnect(slot);
      }
    }

    //*************************************************************************
    ///\brief Disconnects all slots from the signal.
    //*************************************************************************
    void disconnect_all() GDUT_NOEXCEPT
    {
      slot_list_end = begin();
    }

    //*************************************************************************
    ///\brief Checks if a slot is connected to the signal.
    ///
    ///\param slot: To check.
    ///\return <b>true</b> if the slot is connected.
    //*************************************************************************
    GDUT_CONSTEXPR14 bool connected(const slot_type& slot) const GDUT_NOEXCEPT
    {
      return gdut::any_of(begin(), end(), [&slot](const slot_type& s) { return s == slot; });
    }

    //*************************************************************************
    ///\return <b>true</b> if the signal has no slots connected.
    //*************************************************************************
    GDUT_CONSTEXPR14 bool empty() const GDUT_NOEXCEPT
    {
      return begin() == end();
    }

    //*************************************************************************
    ///\return <b>true</b> if the signal has the maximum number of slots connected.
    //*************************************************************************
    GDUT_CONSTEXPR14 bool full() const GDUT_NOEXCEPT
    {
      return size() == max_size();
    }

    //*************************************************************************
    ///\return Total number of slots that can be connected.
    //*************************************************************************
    GDUT_CONSTEXPR14 size_type max_size() const GDUT_NOEXCEPT
    {
      return Size;
    }

    //*************************************************************************
    ///\return Total slots currently connected.
    //*************************************************************************
    GDUT_CONSTEXPR14 size_type size() const GDUT_NOEXCEPT
    {
      return gdut::distance(begin(), end());
    }

    //*************************************************************************
    ///\return Total empty slots available.
    //*************************************************************************
    GDUT_CONSTEXPR14 size_type available() const GDUT_NOEXCEPT
    {
      return max_size() - size();
    }

    //*************************************************************************
    ///\brief Invokes all the slots connected to the signal.
    /// Checks if the slot is valid to call.
    /// 
    ///\param args: Arguments to pass to the slots.
    //*************************************************************************
    template <typename... TArgs>
    void operator()(TArgs&&... args) const GDUT_NOEXCEPT
    {
      for (const slot_type& slot : *this)
      {
        if (slot_is_valid(slot))
        {
          slot(gdut::forward<TArgs>(args)...);
        }
      }
    }

  private:

    using iterator       = slot_type*;
    using const_iterator = const slot_type*;

    slot_type slot_list[Size];
    iterator  slot_list_end;

    //*************************************************************************
    /// Appends a slot to the slot list.
    //*************************************************************************
    void append_slot(const slot_type& slot) GDUT_NOEXCEPT
    {
      (*slot_list_end) = slot;
      slot_list_end    = gdut::next(slot_list_end);
    }

    //*************************************************************************
    /// For a delegate slot type.
    //*************************************************************************
    template <typename TSlotType, typename... TArgs>
    static 
    typename gdut::enable_if_t<gdut::is_delegate<TSlotType>::value, bool>
      slot_is_valid(const TSlotType& s) GDUT_NOEXCEPT
    {
      return s.is_valid();
    }

    //*************************************************************************
    /// For a non-delegate slot type.
    //*************************************************************************
    template <typename TSlotType, typename... TArgs>
    static 
    typename gdut::enable_if_t<!gdut::is_delegate<TSlotType>::value, bool>
      slot_is_valid(const TSlotType&) GDUT_NOEXCEPT
    {
      return true;
    }

    //*************************************************************************
    ///\return Iterator to the beginning of the connected slots.
    //*************************************************************************
    GDUT_CONSTEXPR14 iterator begin() GDUT_NOEXCEPT
    {
      return slot_list;
    }

    //*************************************************************************
    ///\return Const Iterator to the beginning of the connected slots.
    //*************************************************************************
    GDUT_CONSTEXPR14 const_iterator begin() const GDUT_NOEXCEPT
    {
      return slot_list;
    }

    //*************************************************************************
    ///\return Iterator to the end of the connected slots.
    //*************************************************************************
    GDUT_CONSTEXPR14 iterator end() GDUT_NOEXCEPT
    {
      return slot_list_end;
    }

    //*************************************************************************
    ///\return Const Iterator to the end of the connected slots.
    //*************************************************************************
    GDUT_CONSTEXPR14 const_iterator end() const GDUT_NOEXCEPT
    {
      return slot_list_end;
    }
  };
}

#endif
#endif
