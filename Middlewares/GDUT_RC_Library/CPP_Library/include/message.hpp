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

#ifndef GDUT_MESSAGE_INCLUDED
#define GDUT_MESSAGE_INCLUDED

#include "error_handler.hpp"
#include "exception.hpp"
#include "message_types.hpp"
#include "platform.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"

#include <stdint.h>

namespace gdut {
//***************************************************************************
class message_exception : public gdut::exception {
public:
  message_exception(string_type reason_, string_type file_name_,
                    numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
class unhandled_message_exception : public gdut::message_exception {
public:
  unhandled_message_exception(string_type file_name_, numeric_type line_number_)
      : message_exception(
            GDUT_ERROR_TEXT("message:unknown", GDUT_MESSAGE_FILE_ID "A"),
            file_name_, line_number_) {}
};

class message_tag {};

#if GDUT_HAS_VIRTUAL_MESSAGES
//***************************************************************************
/// Message interface.
/// Virtual.
//***************************************************************************
class imessage {
public:
  //***********************************
  virtual ~imessage() GDUT_NOEXCEPT {}

  //***********************************
  GDUT_NODISCARD virtual gdut::message_id_t
  get_message_id() const GDUT_NOEXCEPT = 0;
};

//***************************************************************************
/// Message type.
/// Virtual.
//***************************************************************************
template <gdut::message_id_t ID_, typename TBase = gdut::imessage>
class message : public TBase, public gdut::message_tag {
public:
  GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TBase>::value),
                     "TBase is not derived from gdut::imessage");

  typedef TBase base_type;

  //***********************************
  GDUT_NODISCARD virtual gdut::message_id_t
  get_message_id() const GDUT_NOEXCEPT GDUT_OVERRIDE {
    return ID;
  }

  //***********************************
  static GDUT_CONSTANT gdut::message_id_t ID = ID_;
};

#else

//***************************************************************************
/// Message interface.
/// Non-virtual.
//***************************************************************************
class imessage {
public:
  //***********************************
  GDUT_NODISCARD gdut::message_id_t get_message_id() const GDUT_NOEXCEPT {
    return id;
  }

protected:
  //***********************************
  imessage(gdut::message_id_t id_) GDUT_NOEXCEPT : id(id_) {}

  //***********************************
  imessage(const imessage &other) GDUT_NOEXCEPT : id(other.id) {}

  //***********************************
  imessage &operator=(const imessage &rhs) GDUT_NOEXCEPT {
    id = rhs.id;
    return *this;
  }

  //***********************************
  gdut::message_id_t id;

private:
  imessage() GDUT_DELETE;
};

//***************************************************************************
/// Message type.
/// Non-virtual.
//***************************************************************************
template <gdut::message_id_t ID_, typename TBase = gdut::imessage>
class message : public TBase, public gdut::message_tag {
public:
  GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TBase>::value),
                     "TBase is not derived from gdut::imessage");

  typedef TBase base_type;

  //***********************************
  message() GDUT_NOEXCEPT : TBase(ID) {}

  //***********************************
  message(const message &) GDUT_NOEXCEPT : TBase(ID) {}

  //***********************************
  message &operator=(const message &) GDUT_NOEXCEPT { return *this; }

  //***********************************
  static GDUT_CONSTANT gdut::message_id_t ID = ID_;
};
#endif

//***************************************************************************
/// The message's static ID.
//***************************************************************************
template <gdut::message_id_t ID_, typename TBase>
GDUT_CONSTANT gdut::message_id_t gdut::message<ID_, TBase>::ID;

//***************************************************************************
/// Is T an gdut::imessage?
//***************************************************************************
template <typename T>
struct is_imessage
    : public gdut::bool_constant<gdut::is_same<
          gdut::imessage, typename gdut::remove_cvref<T>::type>::value> {};

//***************************************************************************
/// Is T ultimately derived from gdut::imessage?
//***************************************************************************
template <typename T>
struct is_message
    : public gdut::bool_constant<gdut::is_base_of<
          gdut::imessage, typename gdut::remove_cvref<T>::type>::value> {};

//***************************************************************************
/// Is T an gdut::message<> or derived from gdut::message<>
//***************************************************************************
template <typename T>
struct is_message_type
    : public gdut::bool_constant<gdut::is_base_of<
          gdut::message_tag, typename gdut::remove_cvref<T>::type>::value> {};

//***************************************************************************
/// Is T a base of gdut::message<T>
//***************************************************************************
template <typename T>
struct is_message_base
    : public gdut::bool_constant<gdut::is_message<T>::value &&
                                 !gdut::is_message_type<T>::value> {};

//***************************************************************************
/// Is T a user defined base of gdut::message<T> and not an gdut::imessage
//***************************************************************************
template <typename T>
struct is_user_message_base
    : public gdut::bool_constant<gdut::is_message_base<T>::value &&
                                 !gdut::is_imessage<T>::value> {};

#if GDUT_USING_CPP17
//***************************************************************************
/// Is T an gdut::imessage?
//***************************************************************************
template <typename T>
inline constexpr bool is_imessage_v = is_imessage<T>::value;

//***************************************************************************
/// Is T ultimately derived from gdut::imessage?
//***************************************************************************
template <typename T> inline constexpr bool is_message_v = is_message<T>::value;

//***************************************************************************
/// Is T derived from gdut::message<>
//***************************************************************************
template <typename T>
inline constexpr bool is_message_type_v = is_message_type<T>::value;

//***************************************************************************
/// Is T a base of gdut::message<T>
//***************************************************************************
template <typename T>
inline constexpr bool is_message_base_v = is_message_base<T>::value;

//***************************************************************************
/// Is T a user defined base of gdut::message<T>
//***************************************************************************
template <typename T>
inline constexpr bool is_user_message_base_v = is_user_message_base<T>::value;
#endif
} // namespace gdut

#endif
