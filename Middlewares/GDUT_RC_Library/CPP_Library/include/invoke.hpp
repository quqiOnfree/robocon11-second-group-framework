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

#ifndef GDUT_INVOKE_INCLUDED
#define GDUT_INVOKE_INCLUDED

#include "platform.hpp"
#include "functional.hpp"
#include "function_traits.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

#if GDUT_USING_CPP11

namespace gdut 
{
  //****************************************************************************
  // invoke implementation
  //****************************************************************************

  //****************************************************************************
  /// Pointer to member function + reference_wrapper
  template <typename TFunction, 
            typename TRefWrapper, 
            typename... TArgs,
            typename = gdut::enable_if_t<gdut::is_member_function_pointer<gdut::decay_t<TFunction>>::value &&
                                        gdut::is_reference_wrapper<gdut::decay_t<TRefWrapper>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TRefWrapper&& ref_wrapper, TArgs&&... args)
    -> decltype((ref_wrapper.get().*f)(gdut::forward<TArgs>(args)...))
  {
    return (ref_wrapper.get().*f)(gdut::forward<TArgs>(args)...);
  }

  //****************************************************************************
  /// Pointer to member function + pointer to object
  template <typename TFunction, 
            typename TPtr, 
            typename... TArgs,
            typename = gdut::enable_if_t<gdut::is_member_function_pointer<gdut::decay_t<TFunction>>::value &&
                                        gdut::is_pointer<gdut::decay_t<TPtr>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TPtr&& ptr, TArgs&&... args)
    -> decltype(((*gdut::forward<TPtr>(ptr)).*f)(gdut::forward<TArgs>(args)...))
  {
    return ((*gdut::forward<TPtr>(ptr)).*f)(gdut::forward<TArgs>(args)...);
  }

  //****************************************************************************
  /// Pointer to member function + object (or derived) reference
  template <typename TFunction, 
            typename TObject,
            typename... TArgs,
            typename = gdut::enable_if_t<gdut::is_member_function_pointer<gdut::decay_t<TFunction>>::value &&
                                        !gdut::is_pointer<gdut::decay_t<TObject>>::value &&
                                        !is_reference_wrapper<gdut::decay_t<TObject>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TObject&& obj, TArgs&&... args)
    -> decltype((gdut::forward<TObject>(obj).*f)(gdut::forward<TArgs>(args)...))
  {
    return (gdut::forward<TObject>(obj).*f)(gdut::forward<TArgs>(args)...);
  }

  //****************************************************************************
  /// Pointer to member object + reference_wrapper
  template <typename TFunction, 
            typename TRefWrapper,
            typename = gdut::enable_if_t<gdut::is_member_object_pointer<gdut::decay_t<TFunction>>::value &&
                                        gdut::is_reference_wrapper<gdut::decay_t<TRefWrapper>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TRefWrapper&& ref_wrapper)
    -> decltype(ref_wrapper.get().*f)
  {
    return ref_wrapper.get().*f;
  }

  //****************************************************************************
  /// Pointer to member object + pointer to object
  template <typename TFunction, typename TPtr,
            typename = gdut::enable_if_t<gdut::is_member_object_pointer<gdut::decay_t<TFunction>>::value &&
                                        gdut::is_pointer<gdut::decay_t<TPtr>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TPtr&& ptr)
    -> decltype(((*gdut::forward<TPtr>(ptr)).*f))
  {
    return ((*gdut::forward<TPtr>(ptr)).*f);
  }

  //****************************************************************************
  /// Pointer to member object + object (or derived) reference
  template <typename TFunction, 
            typename TObject,
            typename = gdut::enable_if_t<gdut::is_member_object_pointer<gdut::decay_t<TFunction>>::value &&
                                        !gdut::is_pointer<gdut::decay_t<TObject>>::value &&
                                        !is_reference_wrapper<gdut::decay_t<TObject>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TObject&& obj)
    -> decltype(gdut::forward<TObject>(obj).*f)
  {
    return gdut::forward<TObject>(obj).*f;
  }

  //****************************************************************************
  /// General callable (function object / lambda / function pointer)
  template <typename TFunction, 
            typename... TArgs,
            typename = gdut::enable_if_t<!gdut::is_member_pointer<gdut::decay_t<TFunction>>::value>>
  GDUT_CONSTEXPR auto invoke(TFunction&& f, TArgs&&... args)
    -> decltype(gdut::forward<TFunction>(f)(gdut::forward<TArgs>(args)...))
  {
    return gdut::forward<TFunction>(f)(gdut::forward<TArgs>(args)...);
  }

  //****************************************************************************
  // is_invocable implementation
  //****************************************************************************
  namespace private_invoke
  {
    //*******************************************
    // Core detection of invocability.
    // Succeeds if the invocation expression is well-formed.
    template <typename TFunction, typename... TArgs>
    struct is_invocable_expr
    {
      template <typename U>
      static auto test(int) -> decltype((void)gdut::invoke(gdut::declval<U>(), gdut::declval<TArgs>()...), gdut::true_type{});
      
      template <typename>
      static gdut::false_type test(...);

      using type = decltype(test<TFunction>(0));

      static GDUT_CONSTANT bool value = type::value;
    };

    //*******************************************
    // Core detection of invocability.
    // Succeeds if the invocation expression is well-formed.
    // Using gdut::type_list for the argument list.
    template <typename TFunction, typename... TArgs>
    struct is_invocable_expr<TFunction, gdut::type_list<TArgs...>>
    {
      template <typename U>
      static auto test(int) -> decltype((void)gdut::invoke(gdut::declval<U>(), gdut::declval<TArgs>()...), gdut::true_type{});

      template <typename>
      static gdut::false_type test(...);

      using type = decltype(test<TFunction>(0));

      static GDUT_CONSTANT bool value = type::value;
    };

    //*******************************************
    // Result type of a valid invocation.
    template <typename TFunction, typename... TArgs>
    struct invoke_result_impl
    {
      template <typename U>
      static auto test(int) -> decltype(gdut::invoke(gdut::declval<U>(), gdut::declval<TArgs>()...));
      
      template <typename>
      static void test(...);

      using type = decltype(test<TFunction>(0));
    };

    //*******************************************
    // Result type of a valid invocation.
    template <typename TFunction, typename... TArgs>
    struct invoke_result_impl<TFunction, gdut::type_list<TArgs...>>
    {
      template <typename U>
      static auto test(int) -> decltype(gdut::invoke(gdut::declval<U>(), gdut::declval<TArgs>()...));

      template <typename>
      static void test(...);

      using type = decltype(test<TFunction>(0));
    };

    template <typename TFunction, typename... TArgs>
    using invoke_result_impl_t = typename invoke_result_impl<TFunction, TArgs...>::type;

    //*******************************************
    // Map raw function type to pointer.
    template <typename TFunction>
    using effective_callable_t = gdut::conditional_t<gdut::is_function<gdut::remove_reference_t<TFunction>>::value,
                                                    gdut::add_pointer_t<gdut::remove_reference_t<TFunction>>,
                                                    TFunction>;
  }

  //****************************************************************************
  /// invoke_result<TFunction, TArgs...>
  template <typename TFunction, typename, typename... TArgs> 
  struct invoke_result
  {
    using type = void;
  };

  //*******************************************
  template <typename TFunction, typename... TArgs>
  struct invoke_result<TFunction,
                       gdut::void_t<decltype(gdut::invoke(gdut::declval<TFunction>(), gdut::declval<TArgs>()...))>,
                       TArgs...> 
  {
  private:

    using FC = private_invoke::effective_callable_t<TFunction>;
  
  public:
  
    using type = gdut::conditional_t<private_invoke::is_invocable_expr<FC, TArgs...>::value,
                                    private_invoke::invoke_result_impl_t<FC, TArgs...>,
                                    void>;
  };

  //****************************************************************************
  /// invoke_result<TFunction, gdut::type_list<TArgs...>>
  template <typename TFunction, typename... TArgs>
  struct invoke_result<TFunction, gdut::type_list<TArgs...>>
  {
  private:

    using FC = private_invoke::effective_callable_t<TFunction>;

  public:

    using type = gdut::conditional_t<private_invoke::is_invocable_expr<FC, TArgs...>::value,
      private_invoke::invoke_result_impl_t<FC, TArgs...>,
      void>;
  };

  //*******************************************
  // Specialization to allow `gdut::type_list<...>` as the second template parameter.
  template <typename TFunction, typename... TArgs>
  struct invoke_result<TFunction,
                       gdut::void_t<decltype(gdut::invoke(gdut::declval<TFunction>(), gdut::declval<TArgs>()...))>,
                       gdut::type_list<TArgs...>> 
  {
    using type = decltype(gdut::invoke(gdut::declval<TFunction>(), gdut::declval<TArgs>()...));
  };

  //*******************************************
  template <typename TFunction, typename... TArgs>
  using invoke_result_t = typename invoke_result<TFunction, void, TArgs...>::type;

  //****************************************************************************
  /// is_invocable<TFunction, TArgs...>
  template <typename TFunction, typename... TArgs>
  struct is_invocable
    : gdut::bool_constant<private_invoke::is_invocable_expr<private_invoke::effective_callable_t<TFunction>, TArgs...>::value>
  {
  };

  //****************************************************************************
  // Specialization to allow `gdut::type_list<...>` as the second template parameter.
  template <typename TFunction, typename... TArgs>
  struct is_invocable<TFunction, gdut::type_list<TArgs...>>
    : is_invocable<TFunction, TArgs...>
  {
  };

  //****************************************************************************
  // is_invocable_r<TReturn, TFunction, TArgs...>
  template <typename TReturn, typename TFunction, typename... TArgs>
  struct is_invocable_r
    : gdut::conditional_t<gdut::is_same<TReturn, void>::value,
                         gdut::is_invocable<TFunction, TArgs...>,
                         gdut::conditional_t<is_invocable<TFunction, TArgs...>::value,
                                            gdut::bool_constant<gdut::is_convertible<invoke_result_t<TFunction, TArgs...>, TReturn>::value>,
                                            gdut::false_type>>
  {
  };

  //****************************************************************************
  // Specialization to allow `gdut::type_list<...>` as the second template parameter.
  template <typename TReturn, typename TFunction, typename... TArgs>
  struct is_invocable_r<TReturn, TFunction, gdut::type_list<TArgs...>>
    : is_invocable_r<TReturn, TFunction, TArgs...>
  {
  };

  //****************************************************************************
  // Specialization to allow `gdut::type_list<...>` when there is a preceding object argument.
  template <typename TFunction, typename TObject, typename... TArgs>
  struct is_invocable<TFunction, TObject, gdut::type_list<TArgs...>>
    : is_invocable<TFunction, TObject, TArgs...>
  {
  };

  //****************************************************************************
  // Specialization for is_invocable_r with a preceding object argument.
  template <typename TReturn, typename TFunction, typename TObject, typename... TArgs>
  struct is_invocable_r<TReturn, TFunction, TObject, gdut::type_list<TArgs...>>
    : is_invocable_r<TReturn, TFunction, TObject, TArgs...>
  {
  };

#if GDUT_USING_CPP17
  //****************************************************************************
  /// is_nothrow_invocable<TFunction, TArgs...>
  template <typename TFunction, typename... TArgs>
  struct is_nothrow_invocable
    : gdut::bool_constant<gdut::is_invocable<TFunction, TArgs...>::value &&
                         gdut::function_traits<private_invoke::effective_callable_t<TFunction>>::is_noexcept>
  {
  };

  //****************************************************************************
  /// is_nothrow_invocable_r<TReturn, TFunction, TArgs...>
  template <typename TReturn, typename TFunction, typename... TArgs>
  struct is_nothrow_invocable_r 
    : gdut::bool_constant<gdut::is_invocable_r<TReturn, TFunction, TArgs...>::value &&
                         gdut::function_traits<private_invoke::effective_callable_t<TFunction>>::is_noexcept &&
                         (gdut::is_same<TReturn, void>::value ||
                          gdut::is_nothrow_convertible<invoke_result_t<TFunction, TArgs...>, TReturn>::value)>
  {
  };

  //****************************************************************************
  /// is_nothrow_invocable_r<TReturn, TFunction, gdut::type_list<TArgs...>>
  template <typename TReturn, typename TFunction, typename... TArgs>
  struct is_nothrow_invocable_r<TReturn, TFunction, gdut::type_list<TArgs...>>
    : gdut::bool_constant<gdut::is_invocable_r<TReturn, TFunction, TArgs...>::value &&
                         gdut::function_traits<private_invoke::effective_callable_t<TFunction>>::is_noexcept &&
                         (gdut::is_same<TReturn, void>::value ||
                          gdut::is_nothrow_convertible<invoke_result_t<TFunction, TArgs...>, TReturn>::value)>
  {
  };

  //****************************************************************************
  // Specialization to allow `gdut::type_list<...>` when there is a preceding object argument
  // for the nothrow-with-return trait.
  template <typename TReturn, typename TFunction, typename TObject, typename... TArgs>
  struct is_nothrow_invocable_r<TReturn, TFunction, TObject, gdut::type_list<TArgs...>>
    : is_nothrow_invocable_r<TReturn, TFunction, TObject, TArgs...>
  {};
#endif

#if GDUT_USING_CPP17
  template <typename TFunction, typename... TArgs>
  inline constexpr bool is_invocable_v = is_invocable<TFunction, TArgs...>::value;

  template <typename TReturn, typename TFunction, typename... TArgs>
  inline constexpr bool is_invocable_r_v = is_invocable_r<TReturn, TFunction, TArgs...>::value;
  
  template <typename TFunction, typename... TArgs>
  inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<TFunction, TArgs...>::value;

  template <typename TFunction, typename... TArgs>
  inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<TFunction, TArgs...>::value;
#endif
}

#endif  // GDUT_USING_CPP11
#endif  // GDUT_INVOKE_INCLUDED
