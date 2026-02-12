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

#ifndef GDUT_RESULT_INCLUDED
#define GDUT_RESULT_INCLUDED

///\defgroup result result
///\ingroup utilities

#include "optional.hpp"
#include "platform.hpp"
#include "variant.hpp"

#if GDUT_CPP11_NOT_SUPPORTED
#if !defined(GDUT_IN_UNIT_TEST)
#error NOT SUPPORTED FOR C++03 OR BELOW
#endif
#else

namespace gdut {
//*****************************************************************************
/// Result type.
//*****************************************************************************
template <typename TValue, typename TError> class result {
public:
  typedef TValue value_type;
  typedef TError error_type;

  //*******************************************
  /// Cannot be default constructed
  //*******************************************
  result() = delete;

  //*******************************************
  /// Copy constructor
  //*******************************************
  result(const result &other) : data(other.data) {}

#if GDUT_CPP11_SUPPORTED
  //*******************************************
  /// Move constructor
  //*******************************************
  result(result &&other) : data(gdut::move(other.data)) {}
#endif

  //*******************************************
  // Construct from a value
  //*******************************************
  result(const TValue &value) : data(value) {}

  //*******************************************
  // Move construct from a value
  //*******************************************
  result(TValue &&value) : data(gdut::move(value)) {}

  //*******************************************
  /// Construct from error
  //*******************************************
  result(const TError &error) : data(error) {}

  //*******************************************
  /// Move construct from error
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  result(TError &&error) : data(gdut::move(error)) {}
#endif

  //*******************************************
  /// Copy assign
  //*******************************************
  result &operator=(const result &other) {
    data = other.data;
    return *this;
  }

  //*******************************************
  /// Move assign
  //*******************************************
  result &operator=(result &&other) {
    data = gdut::move(other.data);
    return *this;
  }

  //*******************************************
  /// Copy assign from value
  //*******************************************
  result &operator=(const TValue &value) {
    data = value;
    return *this;
  }

  //*******************************************
  /// Move assign from value
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  result &operator=(TValue &&value) {
    data = gdut::move(value);
    return *this;
  }
#endif

  //*******************************************
  /// Copy assign from error
  //*******************************************
  result &operator=(const TError &error) {
    data = error;
    return *this;
  }

  //*******************************************
  /// Move assign from error
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  result &operator=(TError &&error) {
    data = gdut::move(error);
    return *this;
  }
#endif

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool has_value() const { return (data.index() == 0U); }

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool is_value() const { return has_value(); }

  //*******************************************
  /// <b>true</b> if result contains an error
  //*******************************************
  bool is_error() const { return !has_value(); }

  //*******************************************
  /// Returns a const reference to the value.
  /// Undefined if the result does not contain an value.
  //*******************************************
  const TValue &value() const { return gdut::get<TValue>(data); }

  //*******************************************
  /// Returns an rvalue reference to the value.
  /// Undefined if the result does not contain an value.
  //*******************************************
  TValue &&value() { return gdut::move(gdut::get<TValue>(data)); }

  //*******************************************
  /// Returns a const reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
  const TError &error() const { return gdut::get<TError>(data); }

  //*******************************************
  /// Returns an rvalue reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  TError &&error() { return gdut::move(gdut::get<TError>(data)); }
#endif

private:
  gdut::variant<TValue, TError> data;
};

//*****************************************************************************
/// Result type.
/// Specialisation for void value type.
//*****************************************************************************
template <typename TError> class result<void, TError> {
public:
  typedef void value_type;
  typedef TError error_type;

  //*******************************************
  /// Default Constructor
  //*******************************************
  result() {}

  //*******************************************
  /// Copy constructor
  //*******************************************
  result(const result &other) : data(other.data) {}

  //*******************************************
  /// Move constructor
  //*******************************************
  result(result &&other) : data(gdut::move(other.data)) {}

  //*******************************************
  /// Construct from error
  //*******************************************
  result(const TError &error) : data(error) {}

  //*******************************************
  /// Move construct from error
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  result(TError &&error) : data(gdut::move(error)) {}
#endif

  //*******************************************
  /// Copy assign from error
  //*******************************************
  result &operator=(const TError &error) {
    data = error;
    return *this;
  }

  //*******************************************
  /// Move assign from error
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  result &operator=(TError &&error) {
    data = gdut::move(error);
    return *this;
  }
#endif

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool has_value() const { return !data.has_value(); }

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool is_value() const { return has_value(); }

  //*******************************************
  /// <b>true</b> if result contains an error
  //*******************************************
  bool is_error() const { return !has_value(); }

  //*******************************************
  /// Returns a const reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
  const TError &error() const { return data.value(); }

  //*******************************************
  /// Returns an rvalue reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
#if GDUT_CPP11_SUPPORTED
  TError &&error() { return gdut::move(data.value()); }
#endif

private:
  gdut::optional<TError> data;
};

//*****************************************************************************
/// Result type.
/// Specialisation for void error type.
//*****************************************************************************
template <typename TValue> class result<TValue, void> {
public:
  //*******************************************
  /// Default Constructor
  //*******************************************
  result() {}

  //*******************************************
  /// Copy constructor
  //*******************************************
  result(const result &other) : data(other.data) {}

  //*******************************************
  /// Move constructor
  //*******************************************
  result(result &&other) : data(gdut::move(other.data)) {}

  //*******************************************
  /// Construct from error
  //*******************************************
  result(const TValue &value) : data(value) {}

  //*******************************************
  /// Move construct from error
  //*******************************************
  result(TValue &&value) : data(gdut::move(value)) {}

  //*******************************************
  /// Copy assign from error
  //*******************************************
  result &operator=(const TValue &value) {
    data = value;
    return *this;
  }

  //*******************************************
  /// Move assign from error
  //*******************************************
  result &operator=(TValue &&value) {
    data = gdut::move(value);
    return *this;
  }

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool has_value() const { return data.has_value(); }

  //*******************************************
  /// <b>true</b> if result contains a value
  //*******************************************
  bool is_value() const { return has_value(); }

  //*******************************************
  /// <b>true</b> if result contains an error
  //*******************************************
  bool is_error() const { return !has_value(); }

  //*******************************************
  /// Returns a const reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
  const TValue &value() const { return data.value(); }

  //*******************************************
  /// Returns an rvalue reference to the error.
  /// Undefined if the result does not contain an error.
  //*******************************************
  TValue &&value() { return gdut::move(data.value()); }

private:
  gdut::optional<TValue> data;
};
} // namespace gdut
#endif

#endif
