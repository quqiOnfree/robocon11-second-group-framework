///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2025 John Wellbelove

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

#ifndef GDUT_NOT_NULL_INCLUDED
#define GDUT_NOT_NULL_INCLUDED

#include "platform.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "static_assert.hpp"
#include "memory.hpp"
#include "type_traits.hpp"

namespace gdut 
{
  //***************************************************************************
  /// The base class for not_null exceptions.
  //***************************************************************************
  class not_null_exception : public exception
  {
  public:

    not_null_exception(string_type reason_, string_type file_name_, numeric_type line_number_) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception when the not_null contains a null.
  //***************************************************************************
  class not_null_contains_null : public not_null_exception
  {
  public:

    not_null_contains_null(string_type file_name_, numeric_type line_number_) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
      : not_null_exception(GDUT_ERROR_TEXT("not_null:contains null", GDUT_NOT_NULL_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  // not_null
  // Primary template
  //***************************************************************************
  template <typename T>
  class not_null;

  //***************************************************************************
  // Specialisation for T*
  // A container for pointers that are not allowed to be null.
  //***************************************************************************
  template <typename T>
  class not_null<T*> 
  {
  public:

    typedef T        value_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef pointer  underlying_type;

    //*********************************
    /// Constructs a not_null from a pointer.
    /// Asserts if the pointer is null.
    //*********************************
    GDUT_CONSTEXPR14 explicit not_null(underlying_type ptr_) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
      : ptr(ptr_) 
    {
      GDUT_ASSERT(ptr_ != GDUT_NULLPTR, GDUT_ERROR(not_null_contains_null));
    }

    //*********************************
    /// Copy construct from a not_null pointer.
    //*********************************
    GDUT_CONSTEXPR14 not_null(const gdut::not_null<T*>& other) GDUT_NOEXCEPT
      : ptr(other.get()) 
    {
    }

    //*********************************
    /// Assignment from a pointer.
    /// Asserts if the pointer is null.
    //*********************************
    GDUT_CONSTEXPR14 not_null& operator =(underlying_type rhs) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
    {
      GDUT_ASSERT_OR_RETURN_VALUE(rhs != GDUT_NULLPTR, GDUT_ERROR(not_null_contains_null), *this);

      ptr = rhs;

      return *this;
    }

    //*********************************
    /// Assignment from a not_null.
    //*********************************
    GDUT_CONSTEXPR14 not_null& operator =(const gdut::not_null<T*>& rhs) GDUT_NOEXCEPT
    {
      ptr = rhs.get();

      return *this;
    }

    //*********************************
    /// Gets the underlying pointer.
    //*********************************
    GDUT_CONSTEXPR14 pointer get() const GDUT_NOEXCEPT
    { 
      return ptr; 
    }

    //*********************************
    /// Implicit conversion to pointer.
    //*********************************
    GDUT_CONSTEXPR14 operator pointer() const GDUT_NOEXCEPT
    { 
      return ptr; 
    }

    //*********************************
    /// Dereference operator.
    //*********************************
    GDUT_CONSTEXPR14 reference operator*() const GDUT_NOEXCEPT
    { 
      return *ptr; 
    }

    //*********************************
    /// Arrow operator.
    //*********************************
    GDUT_CONSTEXPR14 pointer operator->() const GDUT_NOEXCEPT
    { 
      return ptr; 
    }

  private:

    /// The underlying pointer.
    pointer ptr;
  };

  //***************************************************************************
  // Partial specialisation for gdut::unique_ptr
  // A container for unique_ptr that are not allowed to be null.
  //***************************************************************************
  template <typename T, typename TDeleter>
  class not_null<gdut::unique_ptr<T, TDeleter> > 
  {
  private:

    typedef gdut::not_null<gdut::unique_ptr<T, TDeleter> > this_type;
    typedef gdut::unique_ptr<T, TDeleter> underlying_type;

  public:

    typedef T        value_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;

#if GDUT_USING_CPP11
    //*********************************
    /// Constructs a not_null from a unique_ptr.
    /// Asserts if the unique_ptr contains null.
    /// Moves from the unique_ptr.
    //*********************************
    GDUT_CONSTEXPR14 explicit not_null(underlying_type&& u_ptr_) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
      : u_ptr(gdut::move(u_ptr_)) 
    {
      GDUT_ASSERT(u_ptr.get() != GDUT_NULLPTR, GDUT_ERROR(not_null_contains_null));
    }

    //*********************************
    /// Assign from a unique_ptr.
    /// Asserts if the unique_ptr contains null.
    /// Moves from the unique_ptr.
    //*********************************
    GDUT_CONSTEXPR14 not_null& operator =(underlying_type&& rhs) GDUT_NOEXCEPT_EXPR(GDUT_NOT_USING_EXCEPTIONS)
    {
      GDUT_ASSERT_OR_RETURN_VALUE(rhs.get() != GDUT_NULLPTR, GDUT_ERROR(not_null_contains_null), *this);

      u_ptr = gdut::move(rhs);

      return *this;
    }
#endif

    //*********************************
    /// Gets the underlying ptr.
    //*********************************
    GDUT_CONSTEXPR14 pointer get() const GDUT_NOEXCEPT
    { 
      return u_ptr.get(); 
    }

    //*********************************
    /// Implicit conversion to pointer.
    //*********************************
    GDUT_CONSTEXPR14 operator pointer() const GDUT_NOEXCEPT
    { 
      return u_ptr.get(); 
    }
  
    //*********************************
    /// Dereference operator.
    //*********************************
    GDUT_CONSTEXPR14 reference operator*() const GDUT_NOEXCEPT
    { 
      return *u_ptr; 
    }
  
    //*********************************
    /// Arrow operator.
    //*********************************
    GDUT_CONSTEXPR14 pointer operator->() const GDUT_NOEXCEPT
    { 
      return u_ptr.get(); 
    }

  private:
  
    GDUT_CONSTEXPR14 explicit not_null(const this_type& u_ptr_) GDUT_NOEXCEPT GDUT_DELETE;
    GDUT_CONSTEXPR14 not_null& operator=(const this_type& rhs) GDUT_NOEXCEPT  GDUT_DELETE;

#if GDUT_USING_CPP11
    GDUT_CONSTEXPR14 explicit not_null(this_type&& u_ptr_) GDUT_NOEXCEPT = delete;
    GDUT_CONSTEXPR14 not_null& operator=(this_type&& rhs) GDUT_NOEXCEPT = delete;
#endif

    /// The underlying unique_ptr.
    underlying_type u_ptr;
  };
}

#endif
