///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2022 John Wellbelove

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

#ifndef GDUT_EXPECTED_INCLUDED
#define GDUT_EXPECTED_INCLUDED

///\defgroup expected expected
///\ingroup utilities
#include "platform.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "utility.hpp"
#include "variant.hpp"
#include "initializer_list.hpp"
#include "type_traits.hpp"
#include "invoke.hpp"

namespace gdut
{
  // Forward declaration for is_expected
  template <typename TValue, typename TError> class expected;
  
  template <typename T>
  struct is_expected : gdut::false_type {};
  
  template <typename TValue, typename TError>
  struct is_expected<expected<TValue,TError> > : gdut::true_type {};

  //***************************************************************************
  /// Base exception for et::expected
  //***************************************************************************
  class expected_exception : public gdut::exception
  {
  public:

    expected_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// expected_invalid
  //***************************************************************************
  class expected_invalid : public gdut::expected_exception
  {
  public:

    expected_invalid(string_type file_name_, numeric_type line_number_)
      : expected_exception(GDUT_ERROR_TEXT("expected:invalid", GDUT_EXPECTED_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Unexpected type.
  /// gdut::unexpected represents an unexpected value stored in gdut::expected.
  //***************************************************************************
  template <typename TError>
  class unexpected
  {
  public:

    typedef TError error_type;

    //*******************************************
    /// Copy constructor.
    //*******************************************
    GDUT_CONSTEXPR unexpected(const unexpected& other)
      : error_value(other.error_value)
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move constructor.
    //*******************************************
    GDUT_CONSTEXPR unexpected(unexpected&& other)
      : error_value(gdut::move(other.error_value))
    {
    }
#endif

    //*******************************************
    /// Construct from an lvalue.
    //*******************************************
    GDUT_CONSTEXPR explicit unexpected(const TError& e)
      : error_value(e)
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Construct from an rvalue.
    //*******************************************
    GDUT_CONSTEXPR explicit unexpected(TError&& e)
      : error_value(gdut::forward<TError>(e))
    {
    }

    //*******************************************
    /// Construct from arguments.
    //*******************************************
    template <typename... Args >
    GDUT_CONSTEXPR explicit unexpected(gdut::in_place_t, Args&&... args)
      : error_value(gdut::forward<Args>(args)...)
    {
    }
#endif

#if GDUT_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    GDUT_CONSTEXPR explicit unexpected(gdut::in_place_t, std::initializer_list<U> init, Args&&... args)
      : error_value(init, gdut::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Assign from gdut::unexpected.
    //*******************************************
    GDUT_CONSTEXPR14
    gdut::unexpected<TError>& operator =(const gdut::unexpected<TError>& rhs)
    {
#if GDUT_USING_CPP11
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TError>::value, "Error not copy assignable");
#endif

      error_value = rhs.error_value;
      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign from gdut::unexpected.
    //*******************************************
    GDUT_CONSTEXPR14
      gdut::unexpected<TError>& operator =(gdut::unexpected<TError>&& rhs)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TError>::value, "Error not move assignable");

      error_value = gdut::move(rhs.error_value);
      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //*******************************************
    /// Get the error.
    //*******************************************
    GDUT_CONSTEXPR14 TError& error()& GDUT_NOEXCEPT
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    GDUT_CONSTEXPR14 const TError& error() const& GDUT_NOEXCEPT
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    GDUT_CONSTEXPR14 TError&& error()&& GDUT_NOEXCEPT
    {
      return gdut::move(error_value);
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    GDUT_CONSTEXPR14 TError&& error() const&& GDUT_NOEXCEPT
    {
      return gdut::move(error_value);
    }
#else
    //*******************************************
    /// Get the error.
    //*******************************************
    const TError& error() const
    {
      return error_value;
    }
#endif

    //*******************************************
    /// Swap with another gdut::unexpected.
    //*******************************************
    void swap(gdut::unexpected<TError>& other)
    {
      using GDUT_OR_STD::swap;

      swap(error_value, other.error_value);
    }

  private:

    TError error_value;
  };

  //*****************************************************************************
  /// unexpect_t
  //*****************************************************************************
  struct unexpect_t
  {
    GDUT_CONSTEXPR14 explicit unexpect_t()
    {
    }
  };

#if GDUT_USING_CPP17
  inline GDUT_CONSTEXPR unexpect_t unexpect{};
#else
  static const unexpect_t unexpect;
#endif

  //*****************************************************************************
  /// Expected type.
  //*****************************************************************************
  template <typename TValue, typename TError>
  class expected
  {
  public:

    typedef gdut::expected<TValue, TError> this_type;
    typedef TValue                        value_type;
    typedef TError                        error_type;
    typedef gdut::unexpected<TError>       unexpected_type;

#if GDUT_USING_CPP11
    template <typename U>
    using rebind = expected<U, TError>;
#endif

    //*******************************************
    /// Default constructor
    //*******************************************
    GDUT_CONSTEXPR14 expected() GDUT_NOEXCEPT
      : storage(gdut::in_place_index_t<Value_Type>(), value_type())
    {
    }

    //*******************************************
    /// Constructor
    //*******************************************
    GDUT_CONSTEXPR14 expected(const value_type& value_) GDUT_NOEXCEPT
      : storage(gdut::in_place_index_t<Value_Type>(), value_)
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Constructor
    //*******************************************
    GDUT_CONSTEXPR14 expected(value_type&& value_) GDUT_NOEXCEPT
      : storage(gdut::in_place_index_t<Value_Type>(), gdut::move(value_))
    {
    }
#endif

    //*******************************************
    /// Copy constructor
    //*******************************************
    GDUT_CONSTEXPR14 expected(const expected& other) GDUT_NOEXCEPT
      : storage(other.storage)
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move constructor
    //*******************************************
    GDUT_CONSTEXPR14 expected(expected&& other) GDUT_NOEXCEPT
      : storage(gdut::move(other.storage))
    {
    }
#endif

#if GDUT_USING_CPP11
    //*******************************************
    /// Copy construct from unexpected type.
    //*******************************************
    template <typename G, typename gdut::enable_if<!gdut::is_convertible<const G&, TError>::value, bool>::type = false>
    GDUT_CONSTEXPR14 explicit expected(const gdut::unexpected<G>& ue)
      : storage(gdut::in_place_index_t<Error_Type>(), ue.error())
    {
    }

    template <typename G, typename gdut::enable_if<gdut::is_convertible<const G&, TError>::value, bool>::type = false>
    GDUT_CONSTEXPR14 expected(const gdut::unexpected<G>& ue)
      : storage(gdut::in_place_index_t<Error_Type>(), ue.error())
    {
    }
#else
    template <typename G>
    explicit expected(const gdut::unexpected<G>& ue)
      : storage(gdut::in_place_index_t<Error_Type>(), ue.error())
    {
    }
#endif

#if GDUT_USING_CPP11
    //*******************************************
    /// Move construct from unexpected type.
    //*******************************************
    template <typename G, typename gdut::enable_if<!gdut::is_convertible<const G&, TError>::value, bool>::type = false>
    GDUT_CONSTEXPR14 explicit expected(gdut::unexpected<G>&& ue)
      : storage(gdut::in_place_index_t<Error_Type>(), gdut::move(ue.error()))
    {
    }

    template <typename G, typename gdut::enable_if<gdut::is_convertible<const G&, TError>::value, bool>::type = false>
    GDUT_CONSTEXPR14 expected(gdut::unexpected<G>&& ue)
      : storage(gdut::in_place_index_t<Error_Type>(), gdut::move(ue.error()))
    {
    }
#endif

    //*******************************************
    /// Construct with default value type.
    //*******************************************
    GDUT_CONSTEXPR14 explicit expected(gdut::in_place_t) GDUT_NOEXCEPT
      : storage(value_type())
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Construct value type from arguments.
    //*******************************************
    template <typename... Args>
    GDUT_CONSTEXPR14 explicit expected(gdut::in_place_t, Args&&... args)
      : storage(gdut::in_place_index_t<Value_Type>(), gdut::forward<Args>(args)...)
    {
    }

#if GDUT_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct value type from initializser_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    GDUT_CONSTEXPR14 explicit expected(gdut::in_place_t, std::initializer_list<U> il, Args&&... args)
      : storage(gdut::in_place_index_t<Value_Type>(), il, gdut::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Construct error type from arguments.
    //*******************************************
    template <typename... Args>
    GDUT_CONSTEXPR14 explicit expected(gdut::unexpect_t, Args&&... args)
      : storage(error_type(gdut::forward<Args>(args)...))
    {
    }

#if GDUT_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct error type from initializser_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    GDUT_CONSTEXPR14 explicit expected(gdut::unexpect_t, std::initializer_list<U> il, Args&&... args)
      : storage(error_type(il, gdut::forward<Args>(args)...))
    {
    }
#endif
#endif

    //*******************************************
    /// Copy assign from gdut::expected.
    //*******************************************
    this_type& operator =(const this_type& other)
    {
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TValue>::value && gdut::is_copy_constructible<TError>::value, "Not copy assignable");

      storage = other.storage;

      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign from gdut::expected.
    //*******************************************
    this_type& operator =(this_type&& other)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TValue>::value && gdut::is_move_constructible<TError>::value, "Not move assignable");

      storage = gdut::move(other.storage);

      return *this;
    }
#endif

    //*******************************************
    /// Copy assign from value
    //*******************************************
    expected& operator =(const value_type& value)
    {
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TValue>::value, "Value not copy assignable");

      storage.template emplace<Value_Type>(value);

      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign from value
    //*******************************************
    expected& operator =(value_type&& value)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TValue>::value, "Value not move assignable");

      storage.template emplace<Value_Type>(gdut::move(value));

      return *this;
    }
#endif

    //*******************************************
    /// Copy assign from unexpected
    //*******************************************
    expected& operator =(const unexpected_type& ue)
    {
#if GDUT_USING_CPP11
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TError>::value, "Error not copy assignable");
#endif

      storage.template emplace<Error_Type>(ue.error());

      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign from unexpected
    //*******************************************
    expected& operator =(unexpected_type&& ue)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TError>::value, "Error not move assignable");

      storage.template emplace<Error_Type>(gdut::move(ue.error()));

      return *this;
    }
#endif

#if GDUT_USING_CPP11
    //*******************************************
    /// Get the value.
    //*******************************************
    GDUT_CONSTEXPR14 value_type& value()&
    {
      return gdut::get<Value_Type>(storage);
    }

    //*******************************************
    /// Get the value.
    //*******************************************
    GDUT_CONSTEXPR14 const value_type& value() const&
    {
      return gdut::get<Value_Type>(storage);
    }

    //*******************************************
    /// Get the value.
    //*******************************************
    GDUT_CONSTEXPR14 value_type&& value()&&
    {
      return gdut::move(gdut::get<Value_Type>(storage));
    }

    //*******************************************
    /// Get the value.
    //*******************************************
    GDUT_CONSTEXPR14 const value_type&& value() const&&
    {
      return gdut::move(gdut::get<Value_Type>(storage));
    }
#else
    //*******************************************
    /// Get the value.
    //*******************************************
    const value_type& value() const
    {
      return gdut::get<Value_Type>(storage);
    }
#endif

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    bool has_value() const GDUT_NOEXCEPT
    {
      return (storage.index() == Value_Type);
    }

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    GDUT_EXPLICIT
    operator bool() const GDUT_NOEXCEPT
    {
      return has_value();
    }

#if GDUT_USING_CPP11
    //*******************************************
    ///
    //*******************************************
    template <typename U>
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    gdut::enable_if_t<gdut::is_convertible<U, value_type>::value, value_type>
      value_or(U&& default_value) const&
    {
      if (has_value())
      {
        return value();
      }
      else
      {
        return static_cast<value_type>(gdut::forward<U>(default_value));
      }
    }

    //*******************************************
    ///
    //*******************************************
    template <typename U>
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    gdut::enable_if_t<gdut::is_convertible<U, value_type>::value, value_type>
      value_or(U&& default_value)&&
    {
      if (has_value())
      {
        return gdut::move(value());
      }
      else
      {
        return static_cast<value_type>(gdut::forward<U>(default_value));
      }
    }

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    error_type& error()& GDUT_NOEXCEPT
    {
      return gdut::get<Error_Type>(storage);
    }

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    const error_type& error() const& GDUT_NOEXCEPT
    {
      return gdut::get<Error_Type>(storage);
    }

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    error_type&& error()&& GDUT_NOEXCEPT
    {
      return gdut::move(gdut::get<Error_Type>(storage));
    }

    //*******************************************
    ///
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    const error_type&& error() const&& GDUT_NOEXCEPT
    {
      return gdut::move(gdut::get<Error_Type>(storage));
    }


    //*******************************************
    /// Swap with another gdut::expected.
    //*******************************************
    void swap(this_type& other)
    {
      using GDUT_OR_STD::swap;

      swap(storage, other.storage);
    }

    //*******************************************
    ///
    //*******************************************
    template <typename... Args>
    GDUT_CONSTEXPR14 value_type& emplace(Args&&... args) GDUT_NOEXCEPT
    {
      storage.template emplace<value_type>(gdut::forward<Args>(args)...);

      return value();
    }

    //*******************************************
    ///
    //*******************************************
#if GDUT_HAS_INITIALIZER_LIST
    template <typename U, typename... Args>
    GDUT_CONSTEXPR14 value_type& emplace(std::initializer_list<U> il, Args&&... args) GDUT_NOEXCEPT
    {
      storage.template emplace<value_type>(il, gdut::forward<Args>(args)...);

      return value();
    }
#endif
#else
    //*******************************************
    ///
    //*******************************************
    template <typename U>
    value_type value_or(const U& default_value) const
    {
      if (has_value())
      {
        return value();
      }
      else
      {
        return default_value;
      }
    }

    //*******************************************
    ///
    //*******************************************
    const error_type& error() const
    {
      return gdut::get<Error_Type>(storage);
    }
#endif

    //*******************************************
    ///
    //*******************************************
    value_type* operator ->()
    {
      GDUT_ASSERT_OR_RETURN_VALUE(has_value(), GDUT_ERROR(expected_invalid), GDUT_NULLPTR);

      return gdut::addressof(gdut::get<value_type>(storage));
    }

    //*******************************************
    ///
    //*******************************************
    const value_type* operator ->() const
    {
      GDUT_ASSERT_OR_RETURN_VALUE(has_value(), GDUT_ERROR(expected_invalid), GDUT_NULLPTR);

      return gdut::addressof(gdut::get<value_type>(storage));
    }

    //*******************************************
    ///
    //*******************************************
    value_type& operator *() GDUT_LVALUE_REF_QUALIFIER
    {
      GDUT_ASSERT(has_value(), GDUT_ERROR(expected_invalid));

      return gdut::get<value_type>(storage);
    }

    //*******************************************
    ///
    //*******************************************
    const value_type& operator *() const GDUT_LVALUE_REF_QUALIFIER
    {
      GDUT_ASSERT_OR_RETURN_VALUE(has_value(), GDUT_ERROR(expected_invalid), GDUT_NULLPTR);

      return gdut::get<value_type>(storage);
    }

#if GDUT_USING_CPP11
    //*******************************************
    ///
    //*******************************************
    value_type&& operator *()&&
    {
      GDUT_ASSERT_OR_RETURN_VALUE(has_value(), GDUT_ERROR(expected_invalid), GDUT_NULLPTR);

      return gdut::move(gdut::get<value_type>(storage));
    }

    //*******************************************
    ///
    //*******************************************
    const value_type&& operator *() const&&
    {
      GDUT_ASSERT(has_value(), GDUT_ERROR(expected_invalid));

      return gdut::move(gdut::get<value_type>(storage));
    }
#endif

#if GDUT_USING_CPP11
    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TValue&>::type>::type>
    auto transform(F&& f) & -> expected<U, TError>
    {
      return transform_impl<F, this_type&, U, TValue&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TValue&>::type>::type>
    auto transform(F&& f) const& -> expected<U, TError>
    {
      return transform_impl<F, const this_type&, U, const TValue&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TValue&&>::type>::type>
    auto transform(F&& f) && -> expected<U, TError>
    {
      return transform_impl<F, this_type&&, U, TValue&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TValue&&>::type>::type>
    auto transform(F&& f) const&& -> expected<U, TError>
    {
      return transform_impl<F, const this_type&&, U, const TValue&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TValue&>::type>::type>
    auto and_then(F&& f) & -> U
    {
      return and_then_impl<F, this_type&, U, TValue&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TValue&>::type>::type>
    auto and_then(F&& f) const& -> U
    {
      return and_then_impl<F, const this_type&, U, const TValue&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TValue&&>::type>::type>
    auto and_then(F&& f) && -> U
    {
      return and_then_impl<F, this_type&&, U, TValue&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TValue&&>::type>::type>
    auto and_then(F&& f) const&& -> U
    {
      return and_then_impl<F, const this_type&&, U, const TValue&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&>::type>::type>
    auto or_else(F&& f) & -> U
    {
      return or_else_impl<F, this_type&, U, TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&>::type>::type>
    auto or_else(F&& f) const & -> U
    {
      return or_else_impl<F, const this_type&, U, const TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&&>::type>::type>
    auto or_else(F&& f) && -> U
    {
      return or_else_impl<F, this_type&&, U, TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&&>::type>::type>
    auto or_else(F&& f) const && -> U
    {
      return or_else_impl<F, const this_type&&, U, const TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&>::type>::type>
    auto transform_error(F&& f) & -> expected<TValue, U>
    {
      return transform_error_impl<F, this_type&, U, TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&>::type>::type>
    auto transform_error(F&& f) const & -> expected<TValue, U>
    {
      return transform_error_impl<F, const this_type&, U, const TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&&>::type>::type>
    auto transform_error(F&& f) && -> expected<TValue, U>
    {
      return transform_error_impl<F, this_type&&, U, TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&&>::type>::type>
    auto transform_error(F&& f) const&& -> expected<TValue, U>
    {
      return transform_error_impl<F, const this_type&&, U, const TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }
#endif

  private:

    enum
    {
      Uninitialised,
      Value_Type,
      Error_Type
    };

    typedef gdut::variant<gdut::monostate, value_type, error_type> storage_type;
    storage_type storage;

#if GDUT_USING_CPP11
    template <typename F, typename TExp, typename TRet, typename TValueRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value>::type>
    auto transform_impl(F&& f, TExp&& exp) const -> expected<TRet, TError>
    {
      if (exp.has_value())
      {
        return expected<TRet, TError>(gdut::invoke(gdut::forward<F>(f), gdut::forward<TValueRef>(gdut::get<Value_Type>(exp.storage))));
      }
      else
      {
        return expected<TRet, TError>(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TValueRef, typename = typename gdut::enable_if<gdut::is_void<TRet>::value>::type>
    auto transform_impl(F&& f, TExp&& exp) const -> expected<void, TError>
    {
      if (exp.has_value())
      {
        gdut::invoke(gdut::forward<F>(f), gdut::forward<TValueRef>(gdut::get<Value_Type>(exp.storage)));
        return expected<void, TError>();
      }
      else
      {
        return expected<void, TError>(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TValueRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value && gdut::is_expected<TRet>::value && gdut::is_same<typename TRet::error_type, TError>::value>::type>
    auto and_then_impl(F&& f, TExp&& exp) const -> TRet
    {
      if (exp.has_value())
      {
        return gdut::invoke(gdut::forward<F>(f), gdut::forward<TValueRef>(gdut::get<Value_Type>(exp.storage)));
      }
      else
      {
        return TRet(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TErrorRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value && gdut::is_expected<TRet>::value && gdut::is_same<typename TRet::value_type, TValue>::value>::type>
    auto or_else_impl(F&& f, TExp&& exp) const -> TRet
    {
      if (exp.has_value())
      {
        return TRet(gdut::forward<TExp>(exp).value());
      }
      else
      {
        return gdut::invoke(gdut::forward<F>(f), gdut::forward<TErrorRef>(gdut::get<Error_Type>(exp.storage)));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TErrorRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value>::type>
    auto transform_error_impl(F&& f, TExp&& exp) const -> expected<TValue, TRet>
    {
      if (exp.has_value())
      {
        return expected<TValue, TRet>(gdut::forward<TExp>(exp).value());
      }
      else
      {
        return expected<TValue, TRet>(unexpected<TRet>(gdut::invoke(gdut::forward<F>(f), gdut::forward<TErrorRef>(gdut::get<Error_Type>(exp.storage)))));
      }
    }
#endif
  };

  //*****************************************************************************
  /// Specialisation for void value type.
  //*****************************************************************************
  template<typename TError>
  class expected<void, TError>
  {
  public:

    typedef gdut::expected<void, TError> this_type;
    typedef void                        value_type;
    typedef TError                      error_type;
    typedef gdut::unexpected<TError>     unexpected_type;

    //*******************************************
    /// Default constructor
    //*******************************************
    GDUT_CONSTEXPR14
      expected()
    {
    }

    //*******************************************
    /// Copy construct from unexpected
    //*******************************************
    GDUT_CONSTEXPR14
      expected(const unexpected_type& ue_)
      : storage(ue_.error())
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move construct from unexpected
    //*******************************************
    GDUT_CONSTEXPR14
      expected(unexpected_type&& ue_)
      : storage(gdut::move(ue_.error()))
    {
    }
#endif

    //*******************************************
    /// Copy construct
    //*******************************************
    GDUT_CONSTEXPR14
      expected(const this_type& other)
      : storage(other.storage)
    {
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move construct
    //*******************************************
    GDUT_CONSTEXPR14
      expected(this_type&& other)
      : storage(gdut::move(other.storage))
    {
    }
#endif

    //*******************************************
    /// Copy assign
    //*******************************************
    this_type& operator =(const this_type& other)
    {
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TError>::value, "Not copy assignable");

      storage = other.storage;
      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign
    //*******************************************
    this_type& operator =(this_type&& other)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TError>::value, "Not move assignable");

      storage = gdut::move(other.storage);
      return *this;
    }
#endif

    //*******************************************
    /// Copy assign from unexpected
    //*******************************************
    expected& operator =(const unexpected_type& ue)
    {
#if GDUT_USING_CPP11
      GDUT_STATIC_ASSERT(gdut::is_copy_constructible<TError>::value, "Error not copy assignable");
#endif

      storage.template emplace<Error_Type>(ue.error());
      return *this;
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Move assign from unexpected
    //*******************************************
    expected& operator =(unexpected_type&& ue)
    {
      GDUT_STATIC_ASSERT(gdut::is_move_constructible<TError>::value, "Error not move assignable");

      storage.template emplace<Error_Type>(gdut::move(ue.error()));
      return *this;
    }
#endif

    //*******************************************
    /// Returns true if expected has a value
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    bool has_value() const GDUT_NOEXCEPT
    {
      return (storage.index() != Error_Type);
    }

    //*******************************************
    /// Returns true if expected has a value
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    GDUT_EXPLICIT
    operator bool() const GDUT_NOEXCEPT
    {
      return has_value();
    }

#if GDUT_USING_CPP11
    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    error_type& error()& GDUT_NOEXCEPT
    {
      return gdut::get<Error_Type>(storage);
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    const error_type& error() const& GDUT_NOEXCEPT
    {
      return gdut::get<Error_Type>(storage);
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    error_type&& error() && GDUT_NOEXCEPT
    {
      return gdut::move(gdut::get<Error_Type>(storage));
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    GDUT_NODISCARD
    GDUT_CONSTEXPR14
    const error_type&& error() const&& GDUT_NOEXCEPT
    {
      return gdut::move(gdut::get<Error_Type>(storage));
    }
#else
    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    const error_type& error() const
    {
      return gdut::get<Error_Type>(storage);
    }
#endif

    //*******************************************
    /// Swap with another gdut::expected.
    //*******************************************
    void swap(this_type& other)
    {
      using GDUT_OR_STD::swap;

      swap(storage, other.storage);
    }

#if GDUT_USING_CPP11
    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto transform(F&& f) & -> expected<U, TError>
    {
      return transform_impl<F, this_type&, U>(gdut::forward<F>(f), *this);
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto transform(F&& f) const & -> expected<U, TError>
    {
      return transform_impl<F, const this_type&, U>(gdut::forward<F>(f), *this);
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto transform(F&& f) && -> expected<U, TError>
    {
      return transform_impl<F, this_type&&, U>(gdut::forward<F>(f), gdut::move(*this));
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto transform(F&& f) const && -> expected<U, TError>
    {
      return transform_impl<F, const this_type&&, U>(gdut::forward<F>(f), gdut::move(*this));
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto and_then(F&& f) & -> U
    {
      return and_then_impl<F, this_type&, U>(gdut::forward<F>(f), *this);
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto and_then(F&& f) const & -> U
    {
      return and_then_impl<F, const this_type&, U>(gdut::forward<F>(f), *this);
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto and_then(F&& f) && -> U
    {
      return and_then_impl<F, this_type&&, U>(gdut::forward<F>(f), gdut::move(*this));
    }

    template<typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void>::type>::type>
    auto and_then(F&& f) const && -> U
    {
      return and_then_impl<F, const this_type&&, U>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&>::type>::type>
    auto or_else(F&& f) & -> U
    {
      return or_else_impl<F, this_type&, U, TError&>(gdut::forward<F>(f), *this);
    }
    
    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&>::type>::type>
    auto or_else(F&& f) const & -> U
    {
      return or_else_impl<F, const this_type&, U, const TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&&>::type>::type>
    auto or_else(F&& f) && -> U
    {
      return or_else_impl<F, this_type&&, U, TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }
    
    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&&>::type>::type>
    auto or_else(F&& f) const && -> U
    {
      return or_else_impl<F, const this_type&&, U, const TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&>::type>::type>
    auto transform_error(F&& f) & -> expected<void, U>
    {
      return transform_error_impl<F, this_type&, U, TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&>::type>::type>
    auto transform_error(F&& f) const & -> expected<void, U>
    {
      return transform_error_impl<F, const this_type&, U, const TError&>(gdut::forward<F>(f), *this);
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, TError&&>::type>::type>
    auto transform_error(F&& f) && -> expected<void, U>
    {
      return transform_error_impl<F, this_type&&, U, TError&&>(gdut::forward<F>(f), gdut::move(*this));
    }

    template <typename F, typename U = typename gdut::remove_cvref<typename gdut::invoke_result<F, void, const TError&&>::type>::type>
    auto transform_error(F&& f) const && -> expected<void, U>
    {
      return transform_error_impl<F, const this_type&&, U, const TError&&>(gdut::forward<F>(f), *this);
    }
#endif
  
    
  private:

    enum
    {
      Uninitialised,
      Error_Type
    };

    gdut::variant<gdut::monostate, error_type> storage;

#if GDUT_USING_CPP11
    template <typename F, typename TExp, typename TRet, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value>::type>
    auto transform_impl(F&& f, TExp&& exp) const -> expected<TRet, TError>
    {
      if (exp.has_value())
      {
        return expected<TRet, TError>(gdut::invoke(gdut::forward<F>(f)));
      }
      else
      {
        return expected<TRet, TError>(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename = typename gdut::enable_if<gdut::is_void<TRet>::value>::type>
    auto transform_impl(F&& f, TExp&& exp) const -> expected<void, TError>
    {
      if (exp.has_value())
      {
        gdut::invoke(gdut::forward<F>(f));
        return expected<void, TError>();
      }
      else
      {
        return expected<void, TError>(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value && gdut::is_expected<TRet>::value && gdut::is_same<typename TRet::error_type, TError>::value>::type>
    auto and_then_impl(F&& f, TExp&& exp) const -> TRet
    {
      if (exp.has_value())
      {
        return gdut::invoke(gdut::forward<F>(f));
      }
      else
      {
        return TRet(unexpected<TError>(gdut::forward<TExp>(exp).error()));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TErrorRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value && gdut::is_expected<TRet>::value && gdut::is_same<typename TRet::value_type, void>::value>::type>
    auto or_else_impl(F&& f, TExp&& exp) const -> TRet
    {
      if (exp.has_value())
      {
        return TRet();
      }
      else
      {
        return gdut::invoke(gdut::forward<F>(f), gdut::forward<TErrorRef>(gdut::get<Error_Type>(exp.storage)));
      }
    }

    template <typename F, typename TExp, typename TRet, typename TErrorRef, typename = typename gdut::enable_if<!gdut::is_void<TRet>::value>::type>
    auto transform_error_impl(F&& f, TExp&& exp) const -> expected<void, TRet>
    {
      if (exp.has_value())
      {
        return expected<void, TRet>();
      }
      else
      {
        return expected<void, TRet>(unexpected<TRet>(gdut::invoke(gdut::forward<F>(f), gdut::forward<TErrorRef>(gdut::get<Error_Type>(exp.storage)))));
      }
    }
#endif
  };
}

//*******************************************
/// Equivalence operators.
//*******************************************
template <typename TValue, typename TError, typename TValue2, typename TError2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::expected<TValue, TError>& lhs, const gdut::expected<TValue2, TError2>& rhs)
{
  if (lhs.has_value() != rhs.has_value())
  {
    return false;
  }
  if (lhs.has_value())
  {
    return lhs.value() == rhs.value();
  }
  return lhs.error() == rhs.error();
}

//*******************************************
template <typename TValue, typename TError, typename TValue2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::expected<TValue, TError>& lhs, const TValue2& rhs)
{
  if (!lhs.has_value())
  {
    return false;
  }
  return lhs.value() == rhs;
}

//*******************************************
template <typename TValue, typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::expected<TValue, TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  if (lhs.has_value())
  {
    return false;
  }
  return lhs.error() == rhs.error();
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::expected<void, TError>& lhs, const gdut::expected<void, TError2>& rhs)
{
  if (lhs.has_value() != rhs.has_value())
  {
    return false;
  }
  if (lhs.has_value())
  {
    return true;
  }
  return lhs.error() == rhs.error();
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::expected<void, TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  if (lhs.has_value())
  {
    return false;
  }
  return lhs.error() == rhs.error();
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator ==(const gdut::unexpected<TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  return lhs.error() == rhs.error();
}

//*******************************************
template <typename TValue, typename TError, typename TValue2, typename TError2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::expected<TValue, TError>& lhs, const gdut::expected<TValue2, TError2>& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
template <typename TValue, typename TError, typename TValue2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::expected<TValue, TError>& lhs, const TValue2& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
template <typename TValue, typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::expected<TValue, TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::expected<void, TError>& lhs, const gdut::expected<void, TError2>& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::expected<void, TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
template <typename TError, typename TError2>
GDUT_CONSTEXPR14
bool operator !=(const gdut::unexpected<TError>& lhs, const gdut::unexpected<TError2>& rhs)
{
  return !(lhs == rhs);
}

//*******************************************
/// Swap gdut::expected.
//*******************************************
template <typename TValue, typename TError>
GDUT_CONSTEXPR14
void swap(gdut::expected<TValue, TError>& lhs, gdut::expected<TValue, TError>& rhs)
{
  lhs.swap(rhs);
}

//*******************************************
/// Swap gdut::unexpected.
//*******************************************
template <typename TError>
GDUT_CONSTEXPR14
void swap(gdut::unexpected<TError>& lhs, gdut::unexpected<TError>& rhs)
{
  lhs.swap(rhs);
}

#endif
