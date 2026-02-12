///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2019 John Wellbelove

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

/******************************************************************************

Copyright (C) 2017 by Sergey A Kryukov: derived work
http://www.SAKryukov.org
http://www.codeproject.com/Members/SAKryukov

Based on original work by Sergey Ryazanov:
"The Impossibly Fast C++ Delegates", 18 Jul 2005
https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

MIT license:
http://en.wikipedia.org/wiki/MIT_License

Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed

******************************************************************************/

#ifndef GDUT_DELEGATE_CPP11_INCLUDED
#define GDUT_DELEGATE_CPP11_INCLUDED

#include "../platform.hpp"
#include "../error_handler.hpp"
#include "../exception.hpp"
#include "../type_traits.hpp"
#include "../function_traits.hpp"
#include "../utility.hpp"
#include "../optional.hpp"
#include "../type_list.hpp"

namespace gdut
{
  //***************************************************************************
  /// The base class for delegate exceptions.
  //***************************************************************************
  class delegate_exception : public exception
  {
  public:

    delegate_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the delegate is uninitialised.
  //***************************************************************************
  class delegate_uninitialised : public delegate_exception
  {
  public:

    delegate_uninitialised(string_type file_name_, numeric_type line_number_)
      : delegate_exception(GDUT_ERROR_TEXT("delegate:uninitialised", GDUT_DELEGATE_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //*****************************************************************
  /// The tag to identify an gdut::delegate.
  ///\ingroup delegate
  //*****************************************************************
  struct delegate_tag
  {
  };

  //***************************************************************************
  /// is_delegate
  //***************************************************************************
  template <typename T>
  struct is_delegate : gdut::bool_constant<gdut::is_base_of<delegate_tag, T>::value>
  {
  };

#if GDUT_USING_CPP17
  template <typename T>
  inline constexpr bool is_delegate_v = is_delegate<T>::value;
#endif

  //*************************************************************************
  /// Declaration.
  //*************************************************************************
  template <typename T>
  class delegate;

  //*************************************************************************
  /// Specialisation.
  //*************************************************************************
  template <typename TReturn, typename... TArgs>
  class delegate<TReturn(TArgs...)> final : public delegate_tag
  {
  public:

    using return_type    = TReturn;
    using argument_types = gdut::type_list<TArgs...>;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    GDUT_CONSTEXPR14 delegate() GDUT_NOEXCEPT
    {
    }

    //*************************************************************************
    // Copy constructor.
    //*************************************************************************
    GDUT_CONSTEXPR14 delegate(const delegate& other) = default;

    //*************************************************************************
    // Construct from lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 delegate(TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), lambda_stub<TLambda>);
    }

    //*************************************************************************
    // Construct from const lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 delegate(const TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), const_lambda_stub<TLambda>);
    }

    //*************************************************************************
    // Delete construction from rvalue reference lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !gdut::is_same<gdut::delegate<TReturn(TArgs...)>, TLambda>::value, void>>
    GDUT_CONSTEXPR14 delegate(TLambda&& instance) = delete;

    //*************************************************************************
    /// Create from function (Compile time).
    //*************************************************************************
    template <TReturn(*Method)(TArgs...)>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(GDUT_NULLPTR, function_stub<Method>);
    }

    //*************************************************************************
    /// Create from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create(TLambda& instance) GDUT_NOEXCEPT
    {
      return delegate((void*)(&instance), lambda_stub<TLambda>);
    }

    //*************************************************************************
    /// Create from const Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create(const TLambda& instance) GDUT_NOEXCEPT
    {
      return delegate((void*)(&instance), const_lambda_stub<TLambda>);
    }

    //*************************************************************************
    /// Create from instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...)>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create(T& instance) GDUT_NOEXCEPT
    {
      return delegate((void*)(&instance), method_stub<T, Method>);
    }

    //*************************************************************************
    /// Create from instance method (Run time).
    /// Deleted for rvalue references.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...)>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create(T&& instance) = delete;

    //*************************************************************************
    /// Create from const instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...) const>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create(const T& instance) GDUT_NOEXCEPT
    {
      return delegate((void*)(&instance), const_method_stub<T, Method>);
    }

    //*************************************************************************
    /// Disable create from rvalue instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...) const>
    static GDUT_CONSTEXPR14 delegate create(T&& instance) = delete;

    //*************************************************************************
    /// Create from instance method (Compile time).
    //*************************************************************************
    template <typename T, T& Instance, TReturn(T::*Method)(TArgs...)>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Create from instance method (Compile time).
    /// New API
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...), T& Instance>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Create from const instance method (Compile time).
    //*************************************************************************
    template <typename T, T const& Instance, TReturn(T::*Method)(TArgs...) const>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(const_method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Create from const instance method (Compile time).
    /// New API
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...) const, T const& Instance>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(const_method_instance_stub<T, Method, Instance>);
    }

#if !(defined(GDUT_COMPILER_GCC) && (__GNUC__ <= 8))
    //*************************************************************************
    /// Create from instance function operator (Compile time).
    /// At the time of writing, GCC appears to have trouble with this.
    //*************************************************************************
    template <typename T, T& Instance>
    GDUT_NODISCARD
    static GDUT_CONSTEXPR14 delegate create() GDUT_NOEXCEPT
    {
      return delegate(operator_instance_stub<T, Instance>);
    }
#endif

    //*************************************************************************
    /// Set from function (Compile time).
    //*************************************************************************
    template <TReturn(*Method)(TArgs...)>
    GDUT_CONSTEXPR14 void set() GDUT_NOEXCEPT
    {
      assign(GDUT_NULLPTR, function_stub<Method>);
    }

    //*************************************************************************
    /// Set from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 void set(TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), lambda_stub<TLambda>);
    }

    //*************************************************************************
    /// Set from const Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 void set(const TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), const_lambda_stub<TLambda>);
    }

    //*************************************************************************
    /// Set from instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...)>
    GDUT_CONSTEXPR14 void set(T& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), method_stub<T, Method>);
    }

    //*************************************************************************
    /// Set from const instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...) const>
    GDUT_CONSTEXPR14 void set(T& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), const_method_stub<T, Method>);
    }

    //*************************************************************************
    /// Set from instance method (Compile time).
    //*************************************************************************
    template <typename T, T& Instance, TReturn(T::* Method)(TArgs...)>
    GDUT_CONSTEXPR14 void set() GDUT_NOEXCEPT
    {
      assign(GDUT_NULLPTR, method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Set from instance method (Compile time).
    /// New API
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...), T& Instance>
    GDUT_CONSTEXPR14 void set() GDUT_NOEXCEPT
    {
      assign(GDUT_NULLPTR, method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Set from const instance method (Compile time).
    //*************************************************************************
    template <typename T, T const& Instance, TReturn(T::* Method)(TArgs...) const>
    GDUT_CONSTEXPR14 void set() GDUT_NOEXCEPT
    {
      assign(GDUT_NULLPTR, const_method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Set from const instance method (Compile time).
    /// New API
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TArgs...) const, T const& Instance>
    GDUT_CONSTEXPR14 void set() GDUT_NOEXCEPT
    {
      assign(GDUT_NULLPTR, const_method_instance_stub<T, Method, Instance>);
    }

    //*************************************************************************
    /// Clear the delegate.
    //*************************************************************************
    GDUT_CONSTEXPR14 void clear() GDUT_NOEXCEPT
    {
      invocation.clear();
    }

    //*************************************************************************
    /// Execute the delegate.
    //*************************************************************************
    template <typename... TCallArgs>
    GDUT_CONSTEXPR14
    return_type operator()(TCallArgs&&... args) const
    {
      GDUT_STATIC_ASSERT((sizeof...(TCallArgs) == sizeof...(TArgs)), "Incorrect number of parameters passed to delegate");
      GDUT_STATIC_ASSERT((gdut::type_lists_are_convertible<gdut::type_list<TCallArgs&&...>, argument_types>::value), "Incompatible parameter types passed to delegate");

      GDUT_ASSERT(is_valid(), GDUT_ERROR(delegate_uninitialised));

      return (*invocation.stub)(invocation.object, gdut::forward<TCallArgs>(args)...);
    }

    //*************************************************************************
    /// Execute the delegate if valid.
    /// 'void' return delegate.
    //*************************************************************************
    template <typename TRet = TReturn, typename... TCallArgs>
    GDUT_CONSTEXPR14
    typename gdut::enable_if_t<gdut::is_same<TRet, void>::value, bool>
      call_if(TCallArgs&&... args) const
    {
      GDUT_STATIC_ASSERT((sizeof...(TCallArgs) == sizeof...(TArgs)), "Incorrect number of parameters passed to delegate");
      GDUT_STATIC_ASSERT((gdut::type_lists_are_convertible<gdut::type_list<TCallArgs&&...>, argument_types>::value), "Incompatible parameter types passed to delegate");

      if (is_valid())
      {
        (*invocation.stub)(invocation.object, gdut::forward<TCallArgs>(args)...);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    /// Execute the delegate if valid.
    /// Non 'void' return delegate.
    //*************************************************************************
    template <typename TRet = TReturn, typename... TCallArgs>
    GDUT_CONSTEXPR14
    typename gdut::enable_if_t<!gdut::is_same<TRet, void>::value, gdut::optional<TReturn>>
      call_if(TCallArgs&&... args) const
    {
      GDUT_STATIC_ASSERT((sizeof...(TCallArgs) == sizeof...(TArgs)), "Incorrect number of parameters passed to delegate");
      GDUT_STATIC_ASSERT((gdut::type_lists_are_convertible<gdut::type_list<TCallArgs&&...>, argument_types>::value), "Incompatible parameter types passed to delegate");

      gdut::optional<TReturn> result;

      if (is_valid())
      {
        result = (*invocation.stub)(invocation.object, gdut::forward<TCallArgs>(args)...);
      }

      return result;
    }

    //*************************************************************************
    /// Execute the delegate if valid or call alternative.
    /// Run time alternative.
    //*************************************************************************
    template <typename TAlternative, typename... TCallArgs>
    GDUT_CONSTEXPR14 TReturn call_or(TAlternative&& alternative, TCallArgs&&... args) const
    {
      GDUT_STATIC_ASSERT((sizeof...(TCallArgs) == sizeof...(TArgs)), "Incorrect number of parameters passed to delegate");
      GDUT_STATIC_ASSERT((gdut::type_lists_are_convertible<gdut::type_list<TCallArgs&&...>, argument_types>::value), "Incompatible parameter types passed to delegate");

      if (is_valid())
      {
        return (*invocation.stub)(invocation.object, gdut::forward<TCallArgs>(args)...);
      }
      else
      {
        return gdut::forward<TAlternative>(alternative)(gdut::forward<TCallArgs>(args)...);
      }
    }

    //*************************************************************************
    /// Execute the delegate if valid or call alternative.
    /// Compile time alternative.
    //*************************************************************************
    template <TReturn(*Method)(TArgs...), typename... TCallArgs>
    GDUT_CONSTEXPR14 TReturn call_or(TCallArgs&&... args) const
    {
      GDUT_STATIC_ASSERT((sizeof...(TCallArgs) == sizeof...(TArgs)), "Incorrect number of parameters passed to delegate");
      GDUT_STATIC_ASSERT((gdut::type_lists_are_convertible<gdut::type_list<TCallArgs&&...>, argument_types>::value), "Incompatible parameter types passed to delegate");

      if (is_valid())
      {
        return (*invocation.stub)(invocation.object, gdut::forward<TCallArgs>(args)...);
      }
      else
      {
        return (Method)(gdut::forward<TCallArgs>(args)...);
      }
    }

    //*************************************************************************
    /// Assignment
    //*************************************************************************
    delegate& operator =(const delegate& rhs) = default;

    //*************************************************************************
    /// Create from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 delegate& operator =(TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), lambda_stub<TLambda>);
      return *this;
    }

    //*************************************************************************
    /// Create from const Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value && !is_delegate<TLambda>::value, void>>
    GDUT_CONSTEXPR14 delegate& operator =(const TLambda& instance) GDUT_NOEXCEPT
    {
      assign((void*)(&instance), const_lambda_stub<TLambda>);
      return *this;
    }

    //*************************************************************************
    /// Checks equality.
    //*************************************************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14 bool operator == (const delegate& rhs) const GDUT_NOEXCEPT
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    GDUT_CONSTEXPR14 bool operator != (const delegate& rhs) const GDUT_NOEXCEPT
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14 bool is_valid() const GDUT_NOEXCEPT
    {
      return invocation.stub != GDUT_NULLPTR;
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14 operator bool() const GDUT_NOEXCEPT
    {
      return is_valid();
    }

  private:

    using stub_type = TReturn(*)(void* object, TArgs...);

    //*************************************************************************
    // Callable compatibility: detects if C (or const C) is invocable with (TArgs...) and returns a type
    // convertible to TReturn. Works with generic lambdas and functors.
    template <typename TCallableType, typename = void>
    struct is_invocable_with 
      : gdut::false_type {};

    template <typename TCallableType>
    struct is_invocable_with<TCallableType, gdut::void_t<decltype(gdut::declval<TCallableType&>()(gdut::declval<TArgs>()...))>>
      : gdut::bool_constant<gdut::is_convertible<decltype(gdut::declval<TCallableType&>()(gdut::declval<TArgs>()...)), TReturn>::value>
    {};

    template <typename TCallableType, typename = void>
    struct is_invocable_with_const : gdut::false_type {};

    template <typename TCallableType>
    struct is_invocable_with_const<TCallableType, gdut::void_t<decltype(gdut::declval<const TCallableType&>()(gdut::declval<TArgs>()...))>>
      : gdut::bool_constant<gdut::is_convertible<decltype(gdut::declval<const TCallableType&>()(gdut::declval<TArgs>()...)), TReturn>::value>
    {};

    template <typename TCallableType>
    struct is_compatible_callable
      : gdut::bool_constant<is_invocable_with<TCallableType>::value || is_invocable_with_const<TCallableType>::value>
    {};

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;

      //***********************************************************************
      GDUT_CONSTEXPR14 invocation_element(void* object_, stub_type stub_) GDUT_NOEXCEPT
        : object(object_)
        , stub(stub_)
      {
      }

      //***********************************************************************
      GDUT_CONSTEXPR14 bool operator ==(const invocation_element& rhs) const GDUT_NOEXCEPT
      {
        return (rhs.stub == stub) && (rhs.object == object);
      }

      //***********************************************************************
      GDUT_CONSTEXPR14 bool operator !=(const invocation_element& rhs) const GDUT_NOEXCEPT
      {
        return (rhs.stub != stub) || (rhs.object != object);
      }

      //***********************************************************************
      GDUT_CONSTEXPR14 void clear() GDUT_NOEXCEPT
      {
        object = GDUT_NULLPTR;
        stub   = GDUT_NULLPTR;
      }

      //***********************************************************************
      void*     object = GDUT_NULLPTR;
      stub_type stub   = GDUT_NULLPTR;
    };

    //*************************************************************************
    /// Constructs a delegate from an object and stub.
    //*************************************************************************
    GDUT_CONSTEXPR14 delegate(void* object, stub_type stub) GDUT_NOEXCEPT
      : invocation(object, stub)
    {
    }

    //*************************************************************************
    /// Constructs a delegate from a stub.
    //*************************************************************************
    GDUT_CONSTEXPR14 delegate(stub_type stub) GDUT_NOEXCEPT
      : invocation(GDUT_NULLPTR, stub)
    {
    }

    //*************************************************************************
    /// Assign from an object and stub.
    //*************************************************************************
    GDUT_CONSTEXPR14 void assign(void* object, stub_type stub) GDUT_NOEXCEPT
    {
      invocation.object = object;
      invocation.stub   = stub;
    }

    //*************************************************************************
    /// Stub call for a member function. Run time instance.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...)>
    static GDUT_CONSTEXPR14 TReturn method_stub(void* object, TArgs... args)
    {
      T* p = static_cast<T*>(object);
      return (p->*Method)(gdut::forward<TArgs>(args)...);
    }

    //*************************************************************************
    /// Stub call for a const member function. Run time instance.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...) const>
    static GDUT_CONSTEXPR14 TReturn const_method_stub(void* object, TArgs... args)
    {
      T* const p = static_cast<T*>(object);
      return (p->*Method)(gdut::forward<TArgs>(args)...);
    }

    //*************************************************************************
    /// Stub call for a member function. Compile time instance.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...), T& Instance>
    static GDUT_CONSTEXPR14 TReturn method_instance_stub(void*, TArgs... args)
    {
      return (Instance.*Method)(gdut::forward<TArgs>(args)...);
    }

    //*************************************************************************
    /// Stub call for a const member function. Compile time instance.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TArgs...) const, const T& Instance>
    static GDUT_CONSTEXPR14 TReturn const_method_instance_stub(void*, TArgs... args)
    {
      return (Instance.*Method)(gdut::forward<TArgs>(args)...);
    }

#if !(defined(GDUT_COMPILER_GCC) && (__GNUC__ <= 8))
    //*************************************************************************
    /// Stub call for a function operator. Compile time instance.
    //*************************************************************************
    template <typename T, T& Instance>
    static GDUT_CONSTEXPR14 TReturn operator_instance_stub(void*, TArgs... args)
    {
      return Instance.operator()(gdut::forward<TArgs>(args)...);
    }
#endif

    //*************************************************************************
    /// Stub call for a free function.
    //*************************************************************************
    template <TReturn(*Method)(TArgs...)>
    static GDUT_CONSTEXPR14 TReturn function_stub(void*, TArgs... args)
    {
      return (Method)(gdut::forward<TArgs>(args)...);
    }

    //*************************************************************************
    /// Stub call for a lambda or functor function.
    //*************************************************************************
    template <typename TLambda>
    static GDUT_CONSTEXPR14 TReturn lambda_stub(void* object, TArgs... arg)
    {
      GDUT_STATIC_ASSERT(is_compatible_callable<TLambda>::value, "gdut::delegate: bound lambda/functor is not compatible with the delegate signature");
      
      TLambda* p = static_cast<TLambda*>(object);
      return (p->operator())(gdut::forward<TArgs>(arg)...);
    }

    //*************************************************************************
    /// Stub call for a const lambda or functor function.
    //*************************************************************************
    template <typename TLambda>
    static GDUT_CONSTEXPR14 TReturn const_lambda_stub(void* object, TArgs... arg)
    {
      GDUT_STATIC_ASSERT(is_compatible_callable<TLambda>::value, "gdut::delegate: bound lambda/functor is not compatible with the delegate signature");
      
      const TLambda* p = static_cast<const TLambda*>(object);
      return (p->operator())(gdut::forward<TArgs>(arg)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };

#if GDUT_USING_CPP17
  //*************************************************************************
  /// Make a delegate from a free function.
  //*************************************************************************
  template <auto Function>
  GDUT_NODISCARD
  constexpr auto make_delegate() GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(Function)>::function_type;

    return gdut::delegate<function_type>::template create<Function>();
  }

  //*************************************************************************
  /// Make a delegate from a functor or lambda function.
  //*************************************************************************
  template <typename TLambda, typename = gdut::enable_if_t<gdut::is_class<TLambda>::value, void>>
  GDUT_NODISCARD
  constexpr auto make_delegate(TLambda& instance) GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(&TLambda::operator())>::function_type;

    return gdut::delegate<function_type>(instance);
  }

  //*************************************************************************
  /// Make a delegate from a functor, compile time.
  //*************************************************************************
  template <typename T, T& Instance>
  GDUT_NODISCARD
  constexpr auto make_delegate() GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(&T::operator())>::function_type;

    return gdut::delegate<function_type>::template create<T, Instance>();
  }

  //*************************************************************************
  /// Make a delegate from a member function at compile time.
  //*************************************************************************
  template <typename T, auto Method, T& Instance, typename = gdut::enable_if_t<!gdut::function_traits<decltype(Method)>::is_const>>
  GDUT_NODISCARD
  constexpr auto make_delegate() GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(Method)>::function_type;

    return gdut::delegate<function_type>::template create<T, Method, Instance>();
  }

  //*************************************************************************
  /// Make a delegate from a const member function at compile time.
  //*************************************************************************
  template <typename T, auto Method, const T& Instance, typename = gdut::enable_if_t<gdut::function_traits<decltype(Method)>::is_const>>
  GDUT_NODISCARD
  constexpr auto make_delegate() GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(Method)>::function_type;

    return gdut::delegate<function_type>::template create<T, Method, Instance>();
  }

  //*************************************************************************
  /// Make a delegate from a member function at run time.
  //*************************************************************************
  template <typename T, auto Method>
  GDUT_NODISCARD
  constexpr auto make_delegate(T& instance) GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(Method)>::function_type;

    return gdut::delegate<function_type>::template create<T, Method>(instance);
  }

  //*************************************************************************
  /// Make a delegate from a member function at run time.
  //*************************************************************************
  template <typename T, auto Method>
  GDUT_NODISCARD
  constexpr auto make_delegate(const T& instance) GDUT_NOEXCEPT
  {
    using function_type = typename gdut::function_traits<decltype(Method)>::function_type;

    return gdut::delegate<function_type>::template create<T, Method>(instance);
  }
#endif
}

#endif
