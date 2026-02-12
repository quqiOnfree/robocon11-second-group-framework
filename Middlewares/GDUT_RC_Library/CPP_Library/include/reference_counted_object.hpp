///******************************************************************************
//The MIT License(MIT)
//
//Embedded Template Library.
//https://github.com/ETLCPP/etl
//https://www.etlcpp.com
//
//Copyright(c) 2021 John Wellbelove
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//******************************************************************************/

#ifndef GDUT_REFERENCE_COUNTED_OBJECT_INCLUDED
#define GDUT_REFERENCE_COUNTED_OBJECT_INCLUDED

#include "platform.hpp"
#include "atomic.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "utility.hpp"

#include <stdint.h>

namespace gdut
{

  //***************************************************************************
  /// Exceptions for reference counting
  ///\ingroup reference_counting
  //***************************************************************************
  class reference_counting_exception : public gdut::exception
  {
  public:
    reference_counting_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Reference counter overrun exception
  ///\ingroup reference_counting
  //***************************************************************************
  class reference_count_overrun : public gdut::reference_counting_exception
  {
  public:
    reference_count_overrun(string_type file_name_, numeric_type line_number_)
      : gdut::reference_counting_exception(GDUT_ERROR_TEXT("reference_counting:overrun", GDUT_REFERENCE_COUNTED_OBJECT_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The base of all reference counters.
  //***************************************************************************
  class ireference_counter
  {
  public:

    virtual ~ireference_counter() {}
    virtual void set_reference_count(int32_t value) = 0;
    virtual void increment_reference_count() = 0;
    GDUT_NODISCARD virtual int32_t decrement_reference_count() = 0;
    GDUT_NODISCARD virtual int32_t get_reference_count() const = 0;
  };

  //***************************************************************************
  /// A specific type of reference counter.
  //***************************************************************************
  template <typename TCounter>
  class reference_counter : public ireference_counter
  {
  public:

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    reference_counter()
      : reference_count(0)
    {
    }

    //***************************************************************************
    /// Set the reference count.
    //***************************************************************************
    virtual void set_reference_count(int32_t value) GDUT_OVERRIDE
    {
      reference_count = value;
    }

    //***************************************************************************
    /// Increment the reference count.
    //***************************************************************************
    virtual void increment_reference_count() GDUT_OVERRIDE
    {
      ++reference_count;
    }

    //***************************************************************************
    /// Decrement the reference count.
    //***************************************************************************
    GDUT_NODISCARD virtual int32_t decrement_reference_count() GDUT_OVERRIDE
    {
      GDUT_ASSERT(reference_count > 0, GDUT_ERROR(reference_count_overrun));

      return int32_t(--reference_count);
    }

    //***************************************************************************
    /// Get the current reference count.
    //***************************************************************************
    GDUT_NODISCARD virtual int32_t get_reference_count() const GDUT_OVERRIDE
    {
      return int32_t(reference_count);
    }

  private:

    TCounter reference_count; // The reference count object.
  };

  //***************************************************************************
  /// A specialisation for a counter type of void.
  //***************************************************************************
  template <>
  class reference_counter<void> : public ireference_counter
  {
  public:

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    reference_counter()
    {
      // Do nothing.
    }

    //***************************************************************************
    /// Set the reference count.
    //***************************************************************************
    virtual void set_reference_count(int32_t /*value*/) GDUT_OVERRIDE
    {
      // Do nothing.
    }

    //***************************************************************************
    /// Increment the reference count.
    //***************************************************************************
    virtual void increment_reference_count() GDUT_OVERRIDE
    {
      // Do nothing.
    }

    //***************************************************************************
    /// Decrement the reference count.
    //***************************************************************************
    GDUT_NODISCARD virtual int32_t decrement_reference_count() GDUT_OVERRIDE
    {
      return 1;
    }

    //***************************************************************************
    /// Get the current reference count.
    //***************************************************************************
    GDUT_NODISCARD virtual int32_t get_reference_count() const GDUT_OVERRIDE
    {
      return 1;
    }
  };

  //***************************************************************************
  /// Base for all reference counted objects.
  //***************************************************************************
  class ireference_counted_object
  {
  public:

    virtual ~ireference_counted_object() {}
    GDUT_NODISCARD virtual ireference_counter& get_reference_counter() = 0;
    GDUT_NODISCARD virtual const ireference_counter& get_reference_counter() const = 0;
  };

  //***************************************************************************
  /// Class for creating reference counted objects.
  /// \tparam TObject  The type to be reference counted.
  /// \tparam TCounter The type to use as the counter.
  //***************************************************************************
  template <typename TObject, typename TCounter>
  class reference_counted_object : public gdut::ireference_counted_object
  {
  public:

    typedef TObject  value_type;
    typedef TCounter counter_type;

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    reference_counted_object()
    {
    }

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    reference_counted_object(const TObject& object_)
      : object(object_)
    {
    }

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    template <typename... TArgs>
    reference_counted_object(TArgs&&... args)
      : object(gdut::forward<TArgs>(args)...)
    {
    }
#endif

    //***************************************************************************
    /// Get a reference to the counted object.
    //***************************************************************************
    GDUT_NODISCARD value_type& get_object()
    {
      return object;
    }


    //***************************************************************************
    /// Get a const reference to the counted object.
    //***************************************************************************
    GDUT_NODISCARD const value_type& get_object() const
    {
      return object;
    }

    //***************************************************************************
    /// Get a reference to the reference counter.
    //***************************************************************************
    GDUT_NODISCARD virtual ireference_counter& get_reference_counter() GDUT_OVERRIDE
    {
      return reference_counter;
    }

    //***************************************************************************
    /// Get a const reference to the reference counter.
    //***************************************************************************
    GDUT_NODISCARD virtual const ireference_counter& get_reference_counter() const GDUT_OVERRIDE
    {
      return reference_counter;
    }

  private:

    // This class must not be copy constructed or assigned.
    reference_counted_object(const reference_counted_object&) GDUT_DELETE;
    reference_counted_object& operator =(const reference_counted_object&) GDUT_DELETE;
        
    TObject object;                                     ///< The object being reference counted.
    gdut::reference_counter<TCounter> reference_counter; ///< The reference counter.
  };

#if GDUT_USING_CPP11 && GDUT_HAS_ATOMIC
  //***************************************************************************
  /// Class for creating reference counted objects using an atomic counter.
  /// \tparam TObject  The type to be reference counted.
  //***************************************************************************
  template <typename TObject>
  using atomic_counted_object = gdut::reference_counted_object<TObject, gdut::atomic_int32_t>;
#endif
}

#endif