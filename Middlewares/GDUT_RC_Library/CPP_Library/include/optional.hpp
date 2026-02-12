///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2015 John Wellbelove

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

#ifndef GDUT_OPTIONAL_INCLUDED
#define GDUT_OPTIONAL_INCLUDED

#include "platform.hpp"
#include "alignment.hpp"
#include "memory.hpp"
#include "type_traits.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "utility.hpp"
#include "placement_new.hpp"
#include "initializer_list.hpp"

namespace gdut
{
  //*****************************************************************************
  // Forward declaration of gdut::optional
  //*****************************************************************************
  template <typename T>
  class optional;
  
  //*****************************************************************************
  /// A null option type.
  ///\ingroup utilities
  //*****************************************************************************
  class nullopt_t
  {
  public:

    // Convertible to any type of null non-member pointer.
    template<class T>
    operator T*() const
    {
      return 0;
    }

  private:

    // Can't take address of nullopt.
    void operator&() const GDUT_DELETE;
  };

  //*****************************************************************************
  /// A null option.
  ///\ingroup utilities
  //*****************************************************************************
  const nullopt_t nullopt = {};

  //***************************************************************************
  /// Exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_exception : public exception
  {
  public:

    optional_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Invalid exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_invalid : public optional_exception
  {
  public:

    optional_invalid(string_type file_name_, numeric_type line_number_)
      : optional_exception("optional:invalid", file_name_, line_number_)
    {
    }
  };

  //*****************************************************************************
  // Implementations for fundamental and non fundamental types.
  //*****************************************************************************
  namespace private_optional
  {
    template <typename T, bool UseFundamentalPath = gdut::is_fundamental<T>::value && !gdut::is_const<T>::value>
    class optional_impl;

    //*****************************************************************************
    // Implementation for non fundamental types.
    //*****************************************************************************
    template <typename T>
    class optional_impl<T, false>
    {
    protected:

      typedef T value_type;
      typedef optional_impl<T, false> this_type;

      //***************************************************************************
      /// Constructor.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl()
        : storage()
      {
      }

      //***************************************************************************
      /// Constructor with nullopt.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl(gdut::nullopt_t)
        : storage()
      {
      }

#include "private/diagnostic_uninitialized_push.hpp"
      //***************************************************************************
      /// Copy constructor.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl(const optional_impl<T>& other)
      {
        if (other.has_value())
        {
          storage.construct(other.value());
        }
      }
#include "private/diagnostic_pop.hpp"

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Move constructor.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl(optional_impl<T>&& other)
      {
        if (other.has_value())
        {
          storage.construct(gdut::move(other.value()));
        }
      }

      //***************************************************************************
      /// Constructor from value type.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl(const T& value_)
      {
        storage.construct(value_);
      }

      //***************************************************************************
      /// Constructor from value type.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl(T&& value_)
      {
        storage.construct(gdut::move(value_));
      }

      //***************************************************************************
      /// Constructor from variadic args.
      //***************************************************************************
      template <typename... TArgs>
      GDUT_CONSTEXPR20_STL
        optional_impl(gdut::in_place_t, TArgs&&... args)
      {
        storage.construct(gdut::forward<TArgs>(args)...);
      }

#if GDUT_HAS_INITIALIZER_LIST
      //*******************************************
      /// Construct from initializer_list and arguments.
      //*******************************************
      template <typename U, typename... TArgs >
      GDUT_CONSTEXPR20_STL optional_impl(gdut::in_place_t,
                                        std::initializer_list<U> ilist,
                                        TArgs&&... args)
      {
        storage.construct(ilist, gdut::forward<TArgs>(args)...);
      }
#endif
#endif

      //***************************************************************************
      /// Destructor.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      ~optional_impl()
      {
        storage.destroy();
      }

      //***************************************************************************
      /// Assignment operator from nullopt.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl& operator =(gdut::nullopt_t)
      {
        if (has_value())
        {
          storage.destroy();
        }

        return *this;
      }

      //***************************************************************************
      /// Assignment operator from optional_impl.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl& operator =(const optional_impl<T>& other)
      {
        if (this != &other)
        {
          if (other.has_value())
          {
            storage.construct(other.value());
          }
          else
          {
            storage.destroy();
          }
        }

        return *this;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Assignment operator from optional_impl.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl& operator =(optional_impl&& other)
      {
        if (this != &other)
        {
          if (other.has_value())
          {
            storage.construct(gdut::move(other.value()));
          }
          else
          {
            storage.destroy();
          }
        }

        return *this;
      }
#endif

      //***************************************************************************
      /// Assignment operator from value type.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl& operator =(const T& value_)
      {
        storage.construct(value_);

        return *this;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Assignment operator from value type.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      optional_impl& operator =(T&& value_)
      {
        storage.construct(gdut::move(value_));

        return *this;
      }
#endif

  public:

      //***************************************************************************
      /// Pointer operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T* operator ->()
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return &storage.u.value;
      }

      //***************************************************************************
      /// Pointer operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      const T* operator ->() const
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return &storage.u.value;
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T& operator *() GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.u.value;
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      const T& operator *() const GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.u.value;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T&& operator *()&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.u.value);
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      const T&& operator *() const&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.u.value);
      }
#endif

      //***************************************************************************
      // Check whether optional contains value
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      bool has_value() const GDUT_NOEXCEPT
      {
        return storage.valid;
      }

      //***************************************************************************
      /// Bool conversion operator.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      GDUT_EXPLICIT operator bool() const
      {
        return has_value();
      }

      //***************************************************************************
      /// Get a reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T& value() GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.u.value;
      }

      //***************************************************************************
      /// Get a const reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      const T& value() const GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.u.value;
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T value_or(const T& default_value) const GDUT_LVALUE_REF_QUALIFIER
      {
        return has_value() ? value() : default_value;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Get an rvalue reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      T&& value()&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.u.value);
      }

      //***************************************************************************
      /// Get a const rvalue reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      const T&& value() const&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.u.value);
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      template <typename U>
      GDUT_CONSTEXPR20_STL
      gdut::enable_if_t<gdut::is_convertible<U, T>::value, T>
        value_or(U&& default_value) const&
      {
        return has_value() ? value() : static_cast<T>(gdut::forward<U>(default_value));
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      template <typename U>
      GDUT_CONSTEXPR20_STL
      gdut::enable_if_t<gdut::is_convertible<U, T>::value, T>
        value_or(U&& default_value)&&
      {
        return has_value() ? gdut::move(value()) : static_cast<T>(gdut::forward<U>(default_value));
      }
#endif

      //***************************************************************************
      /// Swaps this value with another.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      void swap(optional_impl& other)
      {
        optional_impl temp(*this);
        *this = other;
        other = temp;
      }

      //***************************************************************************
      /// Reset back to invalid.
      //***************************************************************************
      GDUT_CONSTEXPR20_STL
      void reset()
      {
        storage.destroy();
      }

      //*************************************************************************
      ///
      //*************************************************************************
      GDUT_CONSTEXPR20_STL
      T& emplace(const optional_impl<T>& other)
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(other.has_value(), GDUT_ERROR(optional_invalid));
#endif

        storage.construct(other.value());

        return storage.u.value;
      }

#if GDUT_USING_CPP11  && GDUT_NOT_USING_STLPORT && !defined(GDUT_OPTIONAL_FORCE_CPP03_IMPLEMENTATION)
      //*************************************************************************
      /// Emplaces a value from arbitrary constructor arguments.
      /// Disabled (via SFINAE) if the first argument is an optional_impl (or a
      /// derived type such as gdut::optional<T>) so that the dedicated
      /// emplace(const optional_impl&) overload is selected instead.
      //*************************************************************************
      template <typename U, 
                typename... URest,
                typename gdut::enable_if<!gdut::is_base_of<optional_impl,
                                                         typename gdut::remove_cv<typename gdut::remove_reference<U>::type>::type>::value, int>::type = 0>
      GDUT_CONSTEXPR20_STL
      T& emplace(U&& first, URest&&... rest)
      {
        storage.construct(gdut::forward<U>(first), gdut::forward<URest>(rest)...);

        return storage.u.value;
      }

      //*************************************************************************
      /// Emplaces with zero arguments, i.e. default construct emplace.
      //*************************************************************************
      GDUT_CONSTEXPR20_STL
      T& emplace()
      {
        storage.construct();

        return storage.u.value;
      }
#else
      //*************************************************************************
      /// Emplaces a value.
      /// 0 parameters.
      //*************************************************************************
      T& emplace()
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.u.value) T();
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 1 parameter.
      //*************************************************************************
      template <typename T1>
      typename gdut::enable_if<!gdut::is_base_of<this_type,     typename gdut::remove_cv<typename gdut::remove_reference<T1>::type>::type>::value &&
                              !gdut::is_same<gdut::optional<T>, typename gdut::remove_cv<typename gdut::remove_reference<T1>::type>::type>::value, T&>::type
        emplace(const T1& value1)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.u.value) T(value1);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 2 parameters.
      //*************************************************************************
      template <typename T1, typename T2>
      T& emplace(const T1& value1, const T2& value2)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.u.value) T(value1, value2);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 3 parameters.
      //*************************************************************************
      template <typename T1, typename T2, typename T3>
      T& emplace(const T1& value1, const T2& value2, const T3& value3)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.u.value) T(value1, value2, value3);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 4 parameters.
      //*************************************************************************
      template <typename T1, typename T2, typename T3, typename T4>
      T& emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.u.value) T(value1, value2, value3, value4);
        storage.valid = true;

        return *p;
      }
#endif

    private:

      struct dummy_t {};

      //*************************************
      // The storage for the optional value.
      //*************************************
      struct storage_type
      {
        typedef typename gdut::remove_const<T>::type* pointer_type;

        //*******************************
        GDUT_CONSTEXPR20_STL
        storage_type()
          : u()
          , valid(false)
        {
        }

        //*******************************
        GDUT_CONSTEXPR20_STL
        void construct(const T& value_)
        {
          destroy();

          gdut::construct_at(const_cast<pointer_type>(&u.value), value_);
          valid = true;
        }

#if GDUT_USING_CPP11
        //*******************************
        GDUT_CONSTEXPR20_STL
        void construct(T&& value_)
        {
          destroy();

          gdut::construct_at(const_cast<pointer_type>(&u.value), gdut::move(value_));
          valid = true;
        }

        //*******************************
        template <typename... TArgs>
        GDUT_CONSTEXPR20_STL
        void construct(TArgs&&... args)
        {
          destroy();

          gdut::construct_at(const_cast<pointer_type>(&u.value), gdut::forward<TArgs>(args)...);
          valid = true;
        }
#endif

        //*******************************
        GDUT_CONSTEXPR20_STL
        void destroy()
        {
          if (valid)
          {
            gdut::destroy_at(const_cast<pointer_type>(&u.value));
            valid = false;
          }
        }

        //*******************************
        union union_type
        {
          GDUT_CONSTEXPR20_STL
          union_type()
            : dummy()
          {
          }

          GDUT_CONSTEXPR20_STL
          ~union_type()
          {
          }

          dummy_t dummy;
          T       value;
        } u;

        bool valid;
      };

      storage_type storage;
    };

    //*****************************************************************************
    // Implementation for fundamental types.
    //*****************************************************************************
    template <typename T>
    class optional_impl<T, true>
    {
    protected:

      typedef T value_type;
      typedef optional_impl<T, true> this_type;

      //***************************************************************************
      /// Constructor.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl()
        : storage()
      {
      }

      //***************************************************************************
      /// Constructor with nullopt.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl(gdut::nullopt_t)
        : storage()
      {
      }

#include "private/diagnostic_uninitialized_push.hpp"
      //***************************************************************************
      /// Copy constructor.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl(const optional_impl<T>& other)
      {
        if (other.has_value())
        {
          storage.construct(other.value());
        }
      }
#include "private/diagnostic_pop.hpp"

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Move constructor.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl(optional_impl&& other)
      {
        if (other.has_value())
        {
          storage.construct(gdut::move(other.value()));
        }
      }

      //***************************************************************************
      /// Constructor from value type.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl(const T& value_)
      {
        storage.construct(value_);
      }

      //***************************************************************************
      /// Constructor from value type.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl(T&& value_)
      {
        storage.construct(gdut::move(value_));
      }

      //***************************************************************************
      /// Constructor from variadic args.
      //***************************************************************************
      template <typename... TArgs>
      GDUT_CONSTEXPR14
        optional_impl(gdut::in_place_t, TArgs&&... args)
      {
        storage.construct(gdut::forward<TArgs>(args)...);
      }

#if GDUT_HAS_INITIALIZER_LIST
      //*******************************************
      /// Construct from initializer_list and arguments.
      //*******************************************
      template <typename U, typename... TArgs >
      GDUT_CONSTEXPR14 optional_impl(gdut::in_place_t,
                                    std::initializer_list<U> ilist,
                                    TArgs&&... args)
      {
        storage.construct(ilist, gdut::forward<TArgs>(args)...);
      }
#endif
#endif

      //***************************************************************************
      /// Assignment operator from nullopt.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl& operator =(gdut::nullopt_t)
      {
        if (has_value())
        {
          storage.destroy();
        }

        return *this;
      }

      //***************************************************************************
      /// Assignment operator from optional_impl.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl& operator =(const optional_impl<T>& other)
      {
        if (this != &other)
        {
          if (other.has_value())
          {
            storage.construct(other.value());
          }
          else
          {
            storage.destroy();
          }
        }

        return *this;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Assignment operator from optional_impl.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl& operator =(optional_impl&& other)
      {
        if (this != &other)
        {
          if (other.has_value())
          {
            storage.construct(gdut::move(other.value()));
          }
          else
          {
            storage.destroy();
          }
        }

        return *this;
      }
#endif

      //***************************************************************************
      /// Assignment operator from value type.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl& operator =(const T& value_)
      {
        storage.construct(value_);

        return *this;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Assignment operator from value type.
      //***************************************************************************
      GDUT_CONSTEXPR14
      optional_impl& operator =(T&& value_)
      {
        storage.construct(gdut::move(value_));

        return *this;
      }
#endif

  public:

      //***************************************************************************
      /// Pointer operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T* operator ->()
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return &storage.value;
      }

      //***************************************************************************
      /// Pointer operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      const T* operator ->() const
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return &storage.value;
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T& operator *() GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.value;
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      const T& operator *() const GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.value;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T&& operator *()&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.value);
      }

      //***************************************************************************
      /// Dereference operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      const T&& operator *() const&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.value);
      }
#endif

      //***************************************************************************
      // Check whether optional contains value
      //***************************************************************************
      GDUT_CONSTEXPR14
      bool has_value() const GDUT_NOEXCEPT
      {
        return storage.valid;
      }

      //***************************************************************************
      /// Bool conversion operator.
      //***************************************************************************
      GDUT_CONSTEXPR14
      GDUT_EXPLICIT operator bool() const
      {
        return has_value();
      }

      //***************************************************************************
      /// Get a reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T& value() GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.value;
      }

      //***************************************************************************
      /// Get a const reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR14
      const T& value() const GDUT_LVALUE_REF_QUALIFIER
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return storage.value;
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T value_or(const T& default_value) const GDUT_LVALUE_REF_QUALIFIER
      {
        return has_value() ? value() : default_value;
      }

#if GDUT_USING_CPP11
      //***************************************************************************
      /// Get an rvalue reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR14
      T&& value()&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.value);
      }

      //***************************************************************************
      /// Get a const rvalue reference to the value.
      //***************************************************************************
      GDUT_CONSTEXPR14
      const T&& value() const&&
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(has_value(), GDUT_ERROR(optional_invalid));
#endif

        return gdut::move(storage.value);
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      template <typename U>
      GDUT_CONSTEXPR14
      gdut::enable_if_t<gdut::is_convertible<U, T>::value, T>
        value_or(U&& default_value) const&
      {
        return has_value() ? value() : static_cast<T>(gdut::forward<U>(default_value));
      }

      //***************************************************************************
      /// Gets the value or a default if not valid.
      //***************************************************************************
      template <typename U>
      GDUT_CONSTEXPR14
      gdut::enable_if_t<gdut::is_convertible<U, T>::value, T>
        value_or(U&& default_value)&&
      {
        return has_value() ? gdut::move(value()) : static_cast<T>(gdut::forward<U>(default_value));
      }
#endif

      //***************************************************************************
      /// Swaps this value with another.
      //***************************************************************************
      GDUT_CONSTEXPR14
      void swap(optional_impl& other)
      {
        optional_impl temp(*this);
        *this = other;
        other = temp;
      }

      //***************************************************************************
      /// Reset back to invalid.
      //***************************************************************************
      GDUT_CONSTEXPR14
      void reset()
      {
        storage.destroy();
      }

      //*************************************************************************
      ///
      //*************************************************************************
      GDUT_CONSTEXPR20_STL
      T& emplace(const optional_impl<T>& other)
      {
#if GDUT_IS_DEBUG_BUILD
        GDUT_ASSERT(other.has_value(), GDUT_ERROR(optional_invalid));
#endif

        storage.construct(other.value());

        return storage.u.value;
      }

#if GDUT_USING_CPP11  && GDUT_NOT_USING_STLPORT && !defined(GDUT_OPTIONAL_FORCE_CPP03_IMPLEMENTATION)
      //*************************************************************************
      /// Emplaces a value.
      ///\param args The arguments to construct with.
      //*************************************************************************
      template <typename... TArgs>
      GDUT_CONSTEXPR14
      T& emplace(TArgs&& ... args)
      {
        storage.construct(gdut::forward<TArgs>(args)...);

        return storage.value;
      }
#else
      //*************************************************************************
      /// Emplaces a value.
      /// 0 parameters.
      //*************************************************************************
      T& emplace()
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.value) T();
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 1 parameter.
      //*************************************************************************
      template <typename T1>
      typename gdut::enable_if<!gdut::is_base_of<this_type,     typename gdut::remove_cv<typename gdut::remove_reference<T1>::type>::type>::value &&
                              !gdut::is_same<gdut::optional<T>, typename gdut::remove_cv<typename gdut::remove_reference<T1>::type>::type>::value, T&>::type
        emplace(const T1& value1)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.value) T(value1);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 2 parameters.
      //*************************************************************************
      template <typename T1, typename T2>
      T& emplace(const T1& value1, const T2& value2)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.value) T(value1, value2);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 3 parameters.
      //*************************************************************************
      template <typename T1, typename T2, typename T3>
      T& emplace(const T1& value1, const T2& value2, const T3& value3)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.value) T(value1, value2, value3);
        storage.valid = true;

        return *p;
      }

      //*************************************************************************
      /// Emplaces a value.
      /// 4 parameters.
      //*************************************************************************
      template <typename T1, typename T2, typename T3, typename T4>
      T& emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
      {
        if (has_value())
        {
          // Destroy the old one.
          storage.destroy();
        }

        T* p = ::new (&storage.value) T(value1, value2, value3, value4);
        storage.valid = true;

        return *p;
      }
#endif

    private:

      //*************************************
      // The storage for the optional value.
      //*************************************
      struct storage_type
      {
        //*******************************
        GDUT_CONSTEXPR14
        storage_type()
          : value()
          , valid(false)
        {
        }

        //*******************************
        GDUT_CONSTEXPR14
        void construct(const T& value_)
        {
          value = value_;
          valid   = true;
        }

#if GDUT_USING_CPP11
        //*******************************
        GDUT_CONSTEXPR14
        void construct(T&& value_)
        {
          value = value_;
          valid = true;
        }

        //*******************************
        template <typename... TArgs>
        GDUT_CONSTEXPR14
        void construct(TArgs&&... args)
        {
          value = T(gdut::forward<TArgs>(args)...);
          valid = true;
        }
#endif

        //*******************************
        GDUT_CONSTEXPR14
        void destroy()
        {
          valid = false;
        }

        T    value;
        bool valid;
      };

      storage_type storage;
    };
  }

#define GDUT_OPTIONAL_ENABLE_CPP14     typename gdut::enable_if< gdut::is_pod<typename gdut::remove_cv<U>::type>::value, int>::type = 0
#define GDUT_OPTIONAL_ENABLE_CPP20_STL typename gdut::enable_if<!gdut::is_pod<typename gdut::remove_cv<U>::type>::value, int>::type = 0

#define GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14     GDUT_CONSTEXPR14 typename gdut::enable_if< gdut::is_pod<typename gdut::remove_cv<T>::type>::value, bool>::type
#define GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL GDUT_CONSTEXPR20_STL typename gdut::enable_if<!gdut::is_pod<typename gdut::remove_cv<T>::type>::value, bool>::type

  //*****************************************************************************
  /// An optional type.
  /// If the optional type is not initialised then a type is not constructed.
  /// See http://en.cppreference.com/w/cpp/utility/optional
  ///\tparam T The type to store.
  ///\ingroup utilities
  //*****************************************************************************
  template <typename T>
  class optional : public private_optional::optional_impl<T>
  {
  private:

    typedef private_optional::optional_impl<T> impl_t;
    
  public:

    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional()
      : impl_t()
    {
    }

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional()
      : impl_t()
    {
    }
#else
    optional()
      : impl_t()
    {
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional(gdut::nullopt_t)
      : impl_t(gdut::nullopt)
    {
    }

    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional(gdut::nullopt_t)
      : impl_t(gdut::nullopt)
    {
    }
#else
    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
    optional(gdut::nullopt_t)
      : impl_t(gdut::nullopt)
    {
    }
#endif

#include "private/diagnostic_uninitialized_push.hpp"
#if GDUT_USING_CPP11
    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional(const optional& other)
      : impl_t(other)
    {
    }

    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional(const optional& other)
      : impl_t(other)
    {
    }
#else
    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    optional(const optional& other)
      : impl_t(other)
    {
    }
#endif
#include "private/diagnostic_pop.hpp"

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Move constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional(optional&& other)
      : impl_t(other)
    {
    }

    //***************************************************************************
    /// Move constructor.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional(optional&& other)
      : impl_t(other)
    {
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Construct from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional(const T& value_)
      : impl_t(value_)
    {
    }

    //***************************************************************************
    /// Construct from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional(const T& value_)
      : impl_t(value_)
    {
    }
#else
    //***************************************************************************
    /// Construct from value type.
    //***************************************************************************
    optional(const T& value_)
      : impl_t(value_)
    {
    }
#endif


#if GDUT_USING_CPP11
    //***************************************************************************
    /// Move construct from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional(T&& value_)
      : impl_t(gdut::move(value_))
    {
    }

    //***************************************************************************
    /// Move construct from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional(T&& value_)
      : impl_t(gdut::move(value_))
    {
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Emplace construct from arguments.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14, typename... Args>
    GDUT_CONSTEXPR14
    explicit optional(gdut::in_place_t, Args&&... args)
      : impl_t(gdut::in_place_t{}, gdut::forward<Args>(args)...)
    {
    }

    //***************************************************************************
    /// Emplace construct from arguments.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL, typename... Args>
    GDUT_CONSTEXPR20_STL
    explicit optional(gdut::in_place_t, Args&&... args)
      : impl_t(gdut::in_place_t{}, gdut::forward<Args>(args)...)
    {
    }

#if GDUT_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14, typename... TArgs>
    GDUT_CONSTEXPR14 
    explicit optional(gdut::in_place_t,
                      std::initializer_list<U> ilist,
                      TArgs&&... args)
      : impl_t(gdut::in_place_t{}, ilist, gdut::forward<TArgs>(args)...)
    {
    }

    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL, typename... TArgs>
    GDUT_CONSTEXPR20_STL 
    explicit optional(gdut::in_place_t,
                      std::initializer_list<U> ilist,
                      TArgs&&... args)
      : impl_t(gdut::in_place_t{}, ilist, gdut::forward<TArgs>(args)...)
    {
    }
#endif
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional& operator =(gdut::nullopt_t)
    {
      impl_t::operator=(gdut::nullopt);

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional& operator =(gdut::nullopt_t)
    {
      impl_t::operator=(gdut::nullopt);

      return *this;
    }
#else
    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    optional& operator =(gdut::nullopt_t)
    {
      impl_t::operator=(gdut::nullopt);

      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional& operator =(const optional& other)
    {
      impl_t::operator=(other);

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional& operator =(const optional& other)
    {
      impl_t::operator=(other);

      return *this;
    }
#else
    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    optional& operator =(const optional& other)
    {
      impl_t::operator=(other);

      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Move assignment operator from optional.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
      optional& operator =(optional&& other)
    {
      impl_t::operator=(gdut::move(other));

      return *this;
    }

    //***************************************************************************
    /// Move assignment operator from optional.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
      optional& operator =(optional&& other)
    {
      impl_t::operator=(gdut::move(other));

      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
    optional& operator =(const T& value_)
    {
      impl_t::operator=(value_);

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
    optional& operator =(const T& value_)
    {
      impl_t::operator=(value_);

      return *this;
    }
#else
    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    optional& operator =(const T& value_)
    {
      impl_t::operator=(value_);

      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //***************************************************************************
    /// Move assignment operator from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP14>
    GDUT_CONSTEXPR14
      optional& operator =(T&& value_)
    {
      impl_t::operator=(gdut::move(value_));

      return *this;
    }

    //***************************************************************************
    /// Move assignment operator from value type.
    //***************************************************************************
    template <typename U = T, GDUT_OPTIONAL_ENABLE_CPP20_STL>
    GDUT_CONSTEXPR20_STL
      optional& operator =(T&& value_)
    {
      impl_t::operator=(gdut::move(value_));

      return *this;
    }
#endif

    //***************************************************************************
    /// Returns an iterator to the beginning of the optional.
    //***************************************************************************
    GDUT_CONSTEXPR20_STL
    iterator begin() GDUT_NOEXCEPT
    {
      return this->has_value() ? this->operator->() : GDUT_NULLPTR;
    }

    //***************************************************************************
    /// Returns a const iterator to the beginning of the optional.
    //***************************************************************************
    GDUT_CONSTEXPR20_STL
    const_iterator begin() const GDUT_NOEXCEPT
    {
      return this->has_value() ? this->operator->() : GDUT_NULLPTR;
    }

    //***************************************************************************
    /// Returns an iterator to the end of the optional.
    //***************************************************************************
    GDUT_CONSTEXPR20_STL
    iterator end() GDUT_NOEXCEPT
    {
      return this->has_value() ? this->operator->() + 1 : GDUT_NULLPTR;
    }

    //***************************************************************************
    /// Returns a const iterator to the end of the optional.
    //***************************************************************************
    GDUT_CONSTEXPR20_STL
    const_iterator end() const GDUT_NOEXCEPT
    {
      return this->has_value() ? this->operator->() + 1 : GDUT_NULLPTR;
    }
  };

#include "private/diagnostic_uninitialized_push.hpp"

  //***************************************************************************
  /// Equality operator. cppreference 1
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator ==(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    if (lhs.has_value() != rhs.has_value())
    {
      return false;
    }
    else if (!lhs.has_value() && !rhs.has_value())
    {
      return true;
    }
    else
    {
      return lhs.value() == rhs.value();
    }
  }

  //***************************************************************************
  /// Equality operator. cppreference 1
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator ==(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    if (lhs.has_value() != rhs.has_value())
    {
      return false;
    }
    else if (!lhs.has_value() && !rhs.has_value())
    {
      return true;
    }
    else
    {
      return lhs.value() == rhs.value();
    }
  }

  //***************************************************************************
  /// Inequality operator. cppreference 2
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator !=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 2
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator !=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 3
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    if (!rhs.has_value())
    {
      return false;
    }
    else if (!lhs.has_value())
    {
      return true;
    }
    else
    {
      return lhs.value() < rhs.value();
    }
  }

  //***************************************************************************
  /// Less than operator. cppreference 3
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    if (!rhs.has_value())
    {
      return false;
    }
    else if (!lhs.has_value())
    {
      return true;
    }
    else
    {
      return lhs.value() < rhs.value();
    }
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 4
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(rhs < lhs);
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 4
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(rhs < lhs);
  }

  //***************************************************************************
  /// greater than operator. cppreference 5
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return (rhs < lhs);
  }

  //***************************************************************************
  /// greater than operator. cppreference 5
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return (rhs < lhs);
  }

  //***************************************************************************
  /// greater than equal operator. cppreference 6
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs < rhs);
  }

  //***************************************************************************
  /// greater than equal operator. cppreference 6
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >=(const gdut::optional<T>& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs < rhs);
  }

  //***************************************************************************
  /// Equality operator. cppreference 7
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator ==(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !lhs.has_value();
  }

  //***************************************************************************
  /// Equality operator. cppreference 7
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator ==(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !lhs.has_value();
  }

  //***************************************************************************
  /// Equality operator. cppreference 8
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator ==(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return !rhs.has_value();
  }

  //***************************************************************************
  /// Equality operator. cppreference 8
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator ==(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return !rhs.has_value();
  }

  //***************************************************************************
  /// Inequality operator. cppreference 9
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator !=(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !(lhs == gdut::nullopt);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 9
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator !=(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !(lhs == gdut::nullopt);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 10
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator !=(gdut::nullopt_t , const gdut::optional<T>& rhs)
  {
    return !(gdut::nullopt == rhs);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 10
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator !=(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return !(gdut::nullopt == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 11
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <(const gdut::optional<T>&, gdut::nullopt_t)
  {
    return false;
  }

  //***************************************************************************
  /// Less than operator. cppreference 11
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <(const gdut::optional<T>&, gdut::nullopt_t)
  {
    return false;
  }

  //***************************************************************************
  /// Less than operator. cppreference 12
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return rhs.has_value();
  }

  //***************************************************************************
  /// Less than operator. cppreference 12
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return rhs.has_value();
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 13
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <=(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !lhs.has_value();
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 13
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <=(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return !lhs.has_value();
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 14
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <=(gdut::nullopt_t, const gdut::optional<T>&)
  {
    return true;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 14
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <=(gdut::nullopt_t, const gdut::optional<T>&)
  {
    return true;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 15
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return lhs.has_value();
  }

  //***************************************************************************
  /// Greater than operator. cppreference 15
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >(const gdut::optional<T>& lhs, gdut::nullopt_t)
  {
    return lhs.has_value();
  }

  //***************************************************************************
  /// Greater than operator. cppreference 16
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >(gdut::nullopt_t, const gdut::optional<T>&)
  {
    return false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 16
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >(gdut::nullopt_t, const gdut::optional<T>&)
  {
    return false;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 17
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >=(const gdut::optional<T>&, gdut::nullopt_t)
  {
    return true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 17
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >=(const gdut::optional<T>&, gdut::nullopt_t)
  {
    return true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 18
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >=(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return !rhs.has_value();
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 18
  //***************************************************************************
  template <typename T>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >=(gdut::nullopt_t, const gdut::optional<T>& rhs)
  {
    return !rhs.has_value();
  }

  //***************************************************************************
  /// Equality operator. cppreference 19
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator ==(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() == rhs : false;
  }

  //***************************************************************************
  /// Equality operator. cppreference 19
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator ==(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() == rhs : false;
  }

  //***************************************************************************
  /// Inequality operator. cppreference 21
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator !=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 21
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator !=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Equality operator. cppreference 20
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator ==(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? rhs.value() == lhs : false;
  }

  //***************************************************************************
  /// Equality operator. cppreference 20
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator ==(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? rhs.value() == lhs : false;
  }

  //***************************************************************************
  /// Inequality operator. cppreference 22
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator !=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 22
  //**************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator !=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 23
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() < rhs : true;
  }

  //***************************************************************************
  /// Less than operator. cppreference 23
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() < rhs : true;
  }

  //***************************************************************************
  /// Less than operator. cppreference 24
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs < rhs.value() : false;
  }

  //***************************************************************************
  /// Less than operator. cppreference 24
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs < rhs.value() : false;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 25
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() <= rhs : true;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 25
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() <= rhs : true;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 26
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator <=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs <= rhs.value() : false;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 26
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator <=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs <= rhs.value() : false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 27
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() > rhs  : false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 27
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() > rhs  : false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 28
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs > rhs.value() : true;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 28
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs > rhs.value() : true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 29
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() >= rhs : false;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 29
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >=(const gdut::optional<T>& lhs, const U& rhs)
  {
    return lhs.has_value() ? lhs.value() >= rhs : false;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 30
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14 operator >=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs >= rhs.value() : true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 30
  //***************************************************************************
  template <typename T, typename U>
  GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL operator >=(const U& lhs, const gdut::optional<T>& rhs)
  {
    return rhs.has_value() ? lhs >= rhs.value() : true;
  }

#include "private/diagnostic_pop.hpp"

  //***************************************************************************
  /// Make an optional.
  //***************************************************************************
  template <typename T>
  GDUT_CONSTEXPR20_STL gdut::optional<typename gdut::decay<T>::type> make_optional(T& value)
  {
    return gdut::optional<typename gdut::decay<T>::type>(value);
  }

  //***************************************************************************
  /// Template deduction guides.
  //***************************************************************************
#if GDUT_CPP17_SUPPORTED
  template <typename T>
  optional(T) -> optional<T>;
#endif
}

//*************************************************************************
/// Swaps the values.
//*************************************************************************
template <typename T>
GDUT_CONSTEXPR20_STL void swap(gdut::optional<T>& lhs, gdut::optional<T>& rhs)
{
  lhs.swap(rhs);
}

#undef GDUT_OPTIONAL_ENABLE_CPP14
#undef GDUT_OPTIONAL_ENABLE_CPP20_STL

#undef GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP14
#undef GDUT_OPTIONAL_ENABLE_CONSTEXPR_BOOL_RETURN_CPP20_STL

#endif
