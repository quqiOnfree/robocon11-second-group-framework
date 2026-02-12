///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2021 John Wellbelove

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

#ifndef GDUT_SUCCESSOR_INCLUDED
#define GDUT_SUCCESSOR_INCLUDED

#include "error_handler.hpp"
#include "exception.hpp"
#include "file_error_numbers.hpp"
#include "nullptr.hpp"
#include "platform.hpp"

namespace gdut {
//***************************************************************************
/// Exception for the successor.
//***************************************************************************
class successor_exception : public gdut::exception {
public:
  successor_exception(string_type reason_, string_type file_name_,
                      numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
/// Invalid exception for successor.
//***************************************************************************
class successor_invalid : public gdut::successor_exception {
public:
  successor_invalid(string_type file_name_, numeric_type line_number_)
      : gdut::successor_exception(
            GDUT_ERROR_TEXT("successor:invalid", GDUT_SUCCESSOR_FILE_ID "A"),
            file_name_, line_number_) {}
};

//***************************************************************************
/// Adds successor traits to a class.
//***************************************************************************
template <typename T> class successor {
public:
  typedef T successor_type;

  //*************************************************************************
  /// Default constructor
  //*************************************************************************
  successor() : p_successor(GDUT_NULLPTR) {}

  //*************************************************************************
  /// Construct from a successor type
  //*************************************************************************
  successor(successor_type &s) : p_successor(&s) {}

  //*************************************************************************
  /// Set the successor.
  //*************************************************************************
  void set_successor(successor_type &s) { p_successor = &s; }

#if GDUT_CPP11_SUPPORTED
  //*************************************************************************
  /// Set a list of successors to this.
  //*************************************************************************
  template <typename... TSuccessors>
  void set_successor(successor_type &s, TSuccessors &...rest) {
    set_successor(s);
    s.set_successor(rest...);
  }
#endif

  //*************************************************************************
  /// Append a successor.
  //*************************************************************************
  template <typename TSuccessor> void append_successor(TSuccessor &s) {
    if (has_successor()) {
      get_successor().append_successor(s);
    } else {
      set_successor(s);
    }
  }

#if GDUT_CPP11_SUPPORTED
  //*************************************************************************
  /// Append the successor.
  //*************************************************************************
  template <typename TSuccessor, typename... TSuccessors>
  void append_successor(TSuccessor &s, TSuccessors &...rest) {
    if (has_successor()) {
      get_successor().append_successor(s);
      get_successor().append_successor(rest...);
    } else {
      set_successor(s);
      append_successor(rest...);
    }
  }
#endif

  //*************************************************************************
  /// Clear the successor.
  //*************************************************************************
  void clear_successor() { p_successor = GDUT_NULLPTR; }

  //*************************************************************************
  /// Clear the successor chain.
  //*************************************************************************
  void clear_successor_chain() {
    if (has_successor()) {
      get_successor().clear_successor_chain();
      clear_successor();
    }
  }

  //*************************************************************************
  /// Get the successor.
  /// Emits an gdut::successor_invalid if a successor has not been set.
  //*************************************************************************
  successor_type &get_successor() const {
    GDUT_ASSERT(has_successor(), GDUT_ERROR(successor_invalid));

    return *p_successor;
  }

  //*************************************************************************
  /// Does this have a successor?
  //*************************************************************************
  bool has_successor() const { return (p_successor != GDUT_NULLPTR); }

private:
  successor_type *p_successor;
};
} // namespace gdut

#endif
