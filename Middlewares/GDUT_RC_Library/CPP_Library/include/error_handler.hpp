
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

#ifndef GDUT_ERROR_HANDLER_INCLUDED
#define GDUT_ERROR_HANDLER_INCLUDED

///\defgroup error_handler error_handler
/// Error handler for when throwing exceptions is not required.
///\ingroup utilities

#include "exception.hpp"
#include "function.hpp"
#include "nullptr.hpp"
#include "platform.hpp"

#include <assert.h>

#if defined(GDUT_LOG_ERRORS) || defined(GDUT_IN_UNIT_TEST)
namespace gdut {
//***************************************************************************
/// Error handler for when throwing exceptions is not required.
///\ingroup error_handler
//***************************************************************************
class error_handler {
public:
  //*************************************************************************
  /// Callback class for free handler functions. Deprecated.
  //*************************************************************************
  struct free_function : public gdut::function<void, const gdut::exception &> {
    explicit free_function(void (*p_function_)(const gdut::exception &))
        : gdut::function<void, const gdut::exception &>(p_function_) {}
  };

  //*************************************************************************
  /// Callback class for member handler functions. Deprecated.
  //*************************************************************************
  template <typename TObject>
  struct member_function
      : public gdut::function<TObject, const gdut::exception &> {
    member_function(TObject &object_,
                    void (TObject::*p_function_)(const gdut::exception &))
        : gdut::function<TObject, const gdut::exception &>(object_,
                                                           p_function_) {}
  };

  //*****************************************************************************
  /// Sets the error callback function. Deprecated.
  ///\param f A reference to an gdut::function object that will handler errors.
  //*****************************************************************************
  static void set_callback(ifunction<const gdut::exception &> &f) {
    create((void *)(&f), ifunction_stub);
  }

  //*************************************************************************
  /// Create from function (Compile time).
  //*************************************************************************
  template <void (*Method)(const gdut::exception &)>
  static void set_callback() {
    create(GDUT_NULLPTR, function_stub<Method>);
  }

  //*************************************************************************
  /// Create from instance method (Run time).
  //*************************************************************************
  template <typename T, void (T::*Method)(const gdut::exception &)>
  static void set_callback(T &instance) {
    create((void *)(&instance), method_stub<T, Method>);
  }

  //*************************************************************************
  /// Create from const instance method (Run time).
  //*************************************************************************
  template <typename T, void (T::*Method)(const gdut::exception &) const>
  static void set_callback(const T &instance) {
    create((void *)(&instance), const_method_stub<T, Method>);
  }

  //*************************************************************************
  /// Create from instance method (Compile time).
  //*************************************************************************
  template <typename T, T &Instance, void (T::*Method)(const gdut::exception &)>
  static void set_callback() {
    create(method_instance_stub<T, Instance, Method>);
  }

  //*************************************************************************
  /// Create from const instance method (Compile time).
  //*************************************************************************
  template <typename T, T const &Instance,
            void (T::*Method)(const gdut::exception &) const>
  static void set_callback() {
    create(const_method_instance_stub<T, Instance, Method>);
  }

  //*****************************************************************************
  /// Sends the exception error to the user's handler function.
  ///\param e The exception error.
  //*****************************************************************************
  static void error(const gdut::exception &e) {
    invocation_element &invocation = get_invocation_element();

    if (invocation.stub != GDUT_NULLPTR) {
      (*invocation.stub)(invocation.object, e);
    }
  }

private:
  typedef void (*stub_type)(void *object, const gdut::exception &);

  //*************************************************************************
  /// The internal invocation object.
  //*************************************************************************
  struct invocation_element {
    //***********************************************************************
    invocation_element() : object(GDUT_NULLPTR), stub(GDUT_NULLPTR) {}

    //***********************************************************************
    void *object;
    stub_type stub;
  };

  //*************************************************************************
  /// Returns the static invocation element.
  //*************************************************************************
  static invocation_element &get_invocation_element() {
    static invocation_element invocation;

    return invocation;
  }

  //*************************************************************************
  /// Constructs a callback from an object and stub.
  //*************************************************************************
  static void create(void *object, stub_type stub) {
    invocation_element &invocation = get_invocation_element();

    invocation.object = object;
    invocation.stub = stub;
  }

  //*************************************************************************
  /// Constructs a callback from a stub.
  //*************************************************************************
  static void create(stub_type stub) {
    invocation_element &invocation = get_invocation_element();

    invocation.object = GDUT_NULLPTR;
    invocation.stub = stub;
  }

  //*************************************************************************
  /// Stub call for a member function. Run time instance.
  //*************************************************************************
  template <typename T, void (T::*Method)(const gdut::exception &)>
  static void method_stub(void *object, const gdut::exception &e) {
    T *p = static_cast<T *>(object);
    return (p->*Method)(e);
  }

  //*************************************************************************
  /// Stub call for a const member function. Run time instance.
  //*************************************************************************
  template <typename T, void (T::*Method)(const gdut::exception &) const>
  static void const_method_stub(void *object, const gdut::exception &e) {
    T *const p = static_cast<T *>(object);
    return (p->*Method)(e);
  }

  //*************************************************************************
  /// Stub call for a member function. Compile time instance.
  //*************************************************************************
  template <typename T, T &Instance, void (T::*Method)(const gdut::exception &)>
  static void method_instance_stub(void *, const gdut::exception &e) {
    return (Instance.*Method)(e);
  }

  //*************************************************************************
  /// Stub call for a const member function. Compile time instance.
  //*************************************************************************
  template <typename T, const T &Instance,
            void (T::*Method)(const gdut::exception &) const>
  static void const_method_instance_stub(void *, const gdut::exception &e) {
    (Instance.*Method)(e);
  }

  //*************************************************************************
  /// Stub call for a free function.
  //*************************************************************************
  template <void (*Method)(const gdut::exception &)>
  static void function_stub(void *, const gdut::exception &e) {
    (Method)(e);
  }

  //*************************************************************************
  /// Stub call for a ifunction. Run time instance.
  //*************************************************************************
  static void ifunction_stub(void *object, const gdut::exception &e) {
    gdut::ifunction<const gdut::exception &> *p =
        static_cast<gdut::ifunction<const gdut::exception &> *>(object);
    p->operator()(e);
  }
};
} // namespace gdut
#elif defined(GDUT_USE_ASSERT_FUNCTION)
namespace gdut {
namespace private_error_handler {
typedef void (*assert_function_ptr_t)(const gdut::exception &);

// Stores the assert function pointer and default assert function.
template <size_t Index> struct assert_handler {
  static assert_function_ptr_t assert_function_ptr;

  static void default_assert(const gdut::exception &) { assert(false); }
};

template <size_t Index>
assert_function_ptr_t assert_handler<Index>::assert_function_ptr =
    assert_handler<Index>::default_assert;
} // namespace private_error_handler

//***************************************************************************
/// Sets the assert function.
/// The argument function signature is void(*)(const gdut::exception&)
//***************************************************************************
inline void
set_assert_function(gdut::private_error_handler::assert_function_ptr_t afptr) {
  gdut::private_error_handler::assert_handler<0>::assert_function_ptr = afptr;
}
} // namespace gdut
#endif

//***************************************************************************
/// Asserts a condition.
/// Versions of the macro that return a constant value of 'true' will allow the
/// compiler to optimise away any 'if' statements that it is contained within.
/// If GDUT_NO_CHECKS is defined then no runtime checks are executed at all.
/// If asserts or exceptions are enabled then the error is thrown if the assert
/// fails. The return value is always 'true'. If GDUT_LOG_ERRORS is defined then
/// the error is logged if the assert fails. The return value is the value of
/// the boolean test. If GDUT_USE_ASSERT_FUNCTION is defined then the error is
/// sent to the assert function. Otherwise 'assert' is called. The return value
/// is always 'true'.
///\ingroup error_handler
//***************************************************************************
#if defined(GDUT_NO_CHECKS)
#define GDUT_ASSERT(b, e) static_cast<void>(sizeof(b)) // Does nothing.
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  static_cast<void>(sizeof(b)) // Does nothing.
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  static_cast<void>(sizeof(b)) // Does nothing.

#define GDUT_ASSERT_FAIL(e) GDUT_DO_NOTHING                     // Does nothing.
#define GDUT_ASSERT_FAIL_AND_RETURN(e) GDUT_DO_NOTHING          // Does nothing.
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v) GDUT_DO_NOTHING // Does nothing.
#elif defined(GDUT_USE_ASSERT_FUNCTION)
#define GDUT_ASSERT(b, e)                                                      \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::private_error_handler::assert_handler<0>::assert_function_ptr(   \
            (e));                                                              \
      }                                                                        \
  } while (false) // If the condition fails, calls the assert function
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::private_error_handler::assert_handler<0>::assert_function_ptr(   \
            (e));                                                              \
        return;                                                                \
      }                                                                        \
  } while (                                                                    \
      false) // If the condition fails, calls the assert function and return
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::private_error_handler::assert_handler<0>::assert_function_ptr(   \
            (e));                                                              \
        return (v);                                                            \
      }                                                                        \
  } while (false) // If the condition fails, calls the assert function and
                  // return a value

#define GDUT_ASSERT_FAIL(e)                                                    \
  do {                                                                         \
    gdut::private_error_handler::assert_handler<0>::assert_function_ptr((e));  \
  } while (false) // Calls the assert function
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    gdut::private_error_handler::assert_handler<0>::assert_function_ptr((e));  \
    return;                                                                    \
  } while (false) // Calls the assert function and return
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    gdut::private_error_handler::assert_handler<0>::assert_function_ptr((e));  \
    return (v);                                                                \
  } while (false) // Calls the assert function and return a value
#elif GDUT_USING_EXCEPTIONS
#if defined(GDUT_LOG_ERRORS)
#define GDUT_ASSERT(b, e)                                                      \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::error_handler::error((e));                                       \
        throw((e));                                                            \
      }                                                                        \
  } while (false) // If the condition fails, calls the error handler then throws
                  // an exception.
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::error_handler::error((e));                                       \
        throw((e));                                                            \
        return;                                                                \
      }                                                                        \
  } while (false) // If the condition fails, calls the error handler then throws
                  // an exception.
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::error_handler::error((e));                                       \
        throw((e));                                                            \
        return (v);                                                            \
      }                                                                        \
  } while (false) // If the condition fails, calls the error handler then throws
                  // an exception.

#define GDUT_ASSERT_FAIL(e)                                                    \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
    throw((e));                                                                \
  } while (false) // Calls the error handler then throws an exception.
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
    throw((e));                                                                \
    return;                                                                    \
  } while (false) // Calls the error handler then throws an exception.
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
    throw((e));                                                                \
    return (v);                                                                \
  } while (false) // Calls the error handler then throws an exception.
#else
#define GDUT_ASSERT(b, e)                                                      \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY { throw((e)); }                                            \
  } while (false) // If the condition fails, throws an exception.
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY { throw((e)); }                                            \
  } while (false) // If the condition fails, throws an exception.
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY { throw((e)); }                                            \
  } while (false) // If the condition fails, throws an exception.

#define GDUT_ASSERT_FAIL(e)                                                    \
  do {                                                                         \
    throw((e));                                                                \
  } while (false) // Throws an exception.
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    throw((e));                                                                \
  } while (false) // Throws an exception.
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    throw((e));                                                                \
  } while (false) // Throws an exception.
#endif
#else
#if defined(GDUT_LOG_ERRORS)
#define GDUT_ASSERT(b, e)                                                      \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY { gdut::error_handler::error((e)); }                       \
  } while (false) // If the condition fails, calls the error handler
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::error_handler::error((e));                                       \
        return;                                                                \
      }                                                                        \
  } while (false) // If the condition fails, calls the error handler and return
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        gdut::error_handler::error((e));                                       \
        return (v);                                                            \
      }                                                                        \
  } while (false) // If the condition fails, calls the error handler and return
                  // a value

#define GDUT_ASSERT_FAIL(e)                                                    \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
  } while (false) // Calls the error handler
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
    return;                                                                    \
  } while (false) // Calls the error handler and return
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    gdut::error_handler::error((e));                                           \
    return (v);                                                                \
  } while (false) // Calls the error handler and return a value
#else
#if GDUT_IS_DEBUG_BUILD
#define GDUT_ASSERT(b, e) assert((b)) // If the condition fails, asserts.
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        assert(false);                                                         \
        return;                                                                \
      }                                                                        \
  } while (false) // If the condition fails, asserts and return.
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY {                                                          \
        assert(false);                                                         \
        return (v);                                                            \
      }                                                                        \
  } while (false) // If the condition fails, asserts and return a value.

#define GDUT_ASSERT_FAIL(e) assert(false) // Asserts.
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    assert(false);                                                             \
    return;                                                                    \
  } while (false) // Asserts.
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    assert(false);                                                             \
    return (v);                                                                \
  } while (false) // Asserts.
#else
#define GDUT_ASSERT(b, e) static_cast<void>(sizeof(b)) // Does nothing.
#define GDUT_ASSERT_OR_RETURN(b, e)                                            \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY return;                                                    \
  } while (false) // Returns.
#define GDUT_ASSERT_OR_RETURN_VALUE(b, e, v)                                   \
  do {                                                                         \
    if (!(b))                                                                  \
      GDUT_UNLIKELY return (v);                                                \
  } while (false) // Returns a value.

#define GDUT_ASSERT_FAIL(e) GDUT_DO_NOTHING // Does nothing.
#define GDUT_ASSERT_FAIL_AND_RETURN(e)                                         \
  do {                                                                         \
    return;                                                                    \
  } while (false) // Returns.
#define GDUT_ASSERT_FAIL_AND_RETURN_VALUE(e, v)                                \
  do {                                                                         \
    return (v);                                                                \
  } while (false) // Returns a value.
#endif
#endif
#endif

//*************************************
#if defined(GDUT_CHECK_PUSH_POP)
#define GDUT_ASSERT_CHECK_PUSH_POP(b, e) GDUT_ASSERT(b, e)
#define GDUT_ASSERT_CHECK_PUSH_POP_OR_RETURN(b, e) GDUT_ASSERT_OR_RETURN(b, e)
#else
#define GDUT_ASSERT_CHECK_PUSH_POP(b, e)
#define GDUT_ASSERT_CHECK_PUSH_POP_OR_RETURN(b, e)
#endif

//*************************************
#ifdef GDUT_CHECK_INDEX_OPERATOR
#define GDUT_CHECKING_INDEX_OPERATOR 1
#define GDUT_NOT_CHECKING_INDEX_OPERATOR 0
#define GDUT_ASSERT_CHECK_INDEX_OPERATOR(b, e) GDUT_ASSERT(b, e)
#else
#define GDUT_CHECKING_INDEX_OPERATOR 0
#define GDUT_NOT_CHECKING_INDEX_OPERATOR 1
#define GDUT_ASSERT_CHECK_INDEX_OPERATOR(b, e)
#endif

//*************************************
#ifdef GDUT_CHECK_EXTRA
#define GDUT_CHECKING_EXTRA 1
#define GDUT_NOT_CHECKING_EXTRA 0
#define GDUT_ASSERT_CHECK_EXTRA(b, e) GDUT_ASSERT(b, e)
#else
#define GDUT_CHECKING_EXTRA 0
#define GDUT_NOT_CHECKING_EXTRA 1
#define GDUT_ASSERT_CHECK_EXTRA(b, e)
#endif

//*************************************
#if defined(GDUT_VERBOSE_ERRORS)
  // include everything, file name, line number, verbose text
#define GDUT_ERROR(e) (e(__FILE__, __LINE__))
#define GDUT_ERROR_WITH_VALUE(e, v) (e(__FILE__, __LINE__, (v)))
#define GDUT_ERROR_TEXT(verbose_text, terse_text) (verbose_text)
#define GDUT_ERROR_GENERIC(text) (gdut::exception((text), __FILE__, __LINE__))
#elif defined(GDUT_MINIMAL_ERRORS)
  // include nothing, no file name, no line number, no text
#define GDUT_ERROR(e) (e("", -1))
#define GDUT_ERROR_WITH_VALUE(e, v) (e("", -1, (v)))
#define GDUT_ERROR_TEXT(verbose_text, terse_text) ("")
#define GDUT_ERROR_GENERIC(text) (gdut::exception("", "", -1))
#else
// include only terse text, no file name, no line number
#define GDUT_ERROR(e) (e("", -1))
#define GDUT_ERROR_WITH_VALUE(e, v) (e("", -1, (v)))
#define GDUT_ERROR_TEXT(verbose_text, terse_text) (terse_text)
#define GDUT_ERROR_GENERIC(text) (gdut::exception((text), "", -1))
#endif

#endif
