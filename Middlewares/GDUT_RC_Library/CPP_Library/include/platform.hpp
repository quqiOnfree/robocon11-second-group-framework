///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2016 John Wellbelove

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

#ifndef GDUT_PLATFORM_INCLUDED
#define GDUT_PLATFORM_INCLUDED

//*************************************
// Enable all limit macros
// Note: This macro must be defined before the first include of stdint.h
#if !defined(__STDC_LIMIT_MACROS)
  #define __STDC_LIMIT_MACROS
#endif

//*************************************
// Enable all constant macros
// Note: This macro must be defined before the first include of stdint.h
#if !defined(__STDC_CONSTANT_MACROS)
  #define __STDC_CONSTANT_MACROS
#endif

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "file_error_numbers.hpp"

//*************************************
// Include the user's profile definition.
#if !defined(GDUT_NO_PROFILE_HEADER) && defined(__has_include)
  #if !__has_include("etl_profile.hpp")
    #define GDUT_NO_PROFILE_HEADER
  #endif
#endif

#if !defined(GDUT_NO_PROFILE_HEADER)
  #include "etl_profile.hpp"
#endif

// Null statement
#define GDUT_DO_NOTHING static_cast<void>(0)

// Determine the bit width of the platform.
#define GDUT_PLATFORM_16BIT (UINT16_MAX == UINTPTR_MAX)
#define GDUT_PLATFORM_32BIT (UINT32_MAX == UINTPTR_MAX)
#define GDUT_PLATFORM_64BIT (UINT64_MAX == UINTPTR_MAX)

//*************************************
// Define debug macros.
#if (defined(_DEBUG) || defined(DEBUG)) && !defined(GDUT_DEBUG)
  #define GDUT_DEBUG
#endif

#if defined(GDUT_DEBUG)
  #define GDUT_IS_DEBUG_BUILD 1
#else
  #define GDUT_IS_DEBUG_BUILD 0
#endif

//*************************************
// Do a validity check for error settings, only one is allowed.
#if defined(GDUT_VERBOSE_ERRORS) && defined(GDUT_MINIMAL_ERRORS)
  #error "GDUT_VERBOSE_ERRORS and GDUT_MINIMAL_ERRORS are mutually exclusive"
#endif

//*************************************
// Helper macros, so we don't have to use double negatives.
// The ETL will use the STL, unless GDUT_NO_STL is defined.
// With this macro we can use '#if GDUT_USING_STL' instead of '#if !GDUT_NO_STL' in the code.
#if defined(GDUT_NO_STL)
  #define GDUT_USING_STL     0
  #define GDUT_NOT_USING_STL 1
#else
  #define GDUT_USING_STL     1
  #define GDUT_NOT_USING_STL 0
#endif

//*************************************
// Helper macros for GDUT_STLPORT.
#if defined(GDUT_STLPORT)
  #define GDUT_USING_STLPORT     1
  #define GDUT_NOT_USING_STLPORT 0
#else
  #define GDUT_USING_STLPORT     0
  #define GDUT_NOT_USING_STLPORT 1
#endif

//*************************************
// Some targets do not support 8bit types.
#if (CHAR_BIT == 8)
  #define GDUT_USING_8BIT_TYPES     1
  #define GDUT_NOT_USING_8BIT_TYPES 0
#else
  #define GDUT_USING_8BIT_TYPES     0
  #define GDUT_NOT_USING_8BIT_TYPES 1
#endif

#define GDUT_8BIT_SUPPORT (CHAR_BIT == 8) // Deprecated

//*************************************
// Some targets support 20bit types.
#if defined(GDUT_USE_20BIT_TYPES)
  #define GDUT_USING_20BIT_TYPES     1
  #define GDUT_NOT_USING_20BIT_TYPES 0
#else
  #define GDUT_USING_20BIT_TYPES     0
  #define GDUT_NOT_USING_20BIT_TYPES 1
#endif


//*************************************
// Helper macro for GDUT_NO_64BIT_TYPES.
#if defined(GDUT_NO_64BIT_TYPES)
  #define GDUT_USING_64BIT_TYPES     0
  #define GDUT_NOT_USING_64BIT_TYPES 1
#else
  #define GDUT_USING_64BIT_TYPES     1
  #define GDUT_NOT_USING_64BIT_TYPES 0
#endif

//*************************************
// For when the runtime library is compiled without wchar_t support.
#if defined(GDUT_NO_WIDE_CHARACTERS)
  #define GDUT_USING_WIDE_CHARACTERS     0
  #define GDUT_NOT_USING_WIDE_CHARACTERS 1
#else
  #define GDUT_USING_WIDE_CHARACTERS     1
  #define GDUT_NOT_USING_WIDE_CHARACTERS 0
#endif

//*************************************
// Figure out things about the compiler, if haven't already done so in etl_profile.h
#include "profiles/determine_compiler_version.hpp"
#include "profiles/determine_compiler_language_support.hpp"

//*************************************
// See if we can determine the OS we're compiling on, if haven't already done so in etl_profile.h
#include "profiles/determine_development_os.hpp"

//*************************************
// Helper macro for choosing the variant type.
#if !GDUT_USING_CPP11 || defined(GDUT_USE_LEGACY_VARIANT)
  #define GDUT_USING_LEGACY_VARIANT     1
  #define GDUT_NOT_USING_LEGACY_VARIANT 0
#else
  #define GDUT_USING_LEGACY_VARIANT     0
  #define GDUT_NOT_USING_LEGACY_VARIANT 1
#endif

//*************************************
// Check WCHAR_MIN and WCHAR_MAX
#if !defined(WCHAR_MIN)
  #define WCHAR_MIN 0x0000
#endif

#if !defined(WCHAR_MAX)
  #define WCHAR_MAX 0xFFFF
#endif

//*************************************
// Option to force string construction from a character pointer to be explicit.
#if defined(GDUT_FORCE_EXPLICIT_STRING_CONVERSION_FROM_CHAR)
  #define GDUT_EXPLICIT_STRING_FROM_CHAR explicit
#else
  #define GDUT_EXPLICIT_STRING_FROM_CHAR
#endif

//*************************************
// Option to disable truncation checks for strings.
#if defined(GDUT_DISABLE_STRING_TRUNCATION_CHECKS)
  #define GDUT_HAS_STRING_TRUNCATION_CHECKS 0
#else
  #define GDUT_HAS_STRING_TRUNCATION_CHECKS 1
#endif

//*************************************
// Option to disable clear-after-use functionality for strings.
#if defined(GDUT_DISABLE_STRING_CLEAR_AFTER_USE)
  #define GDUT_HAS_STRING_CLEAR_AFTER_USE 0
#else
  #define GDUT_HAS_STRING_CLEAR_AFTER_USE 1
#endif

//*************************************
// Option to make string truncation an error.
#if defined(GDUT_ENABLE_ERROR_ON_STRING_TRUNCATION)
  #define GDUT_HAS_ERROR_ON_STRING_TRUNCATION 1
#else
  #define GDUT_HAS_ERROR_ON_STRING_TRUNCATION 0
#endif

//*************************************
// Option to enable repair-after-memcpy for istrings.
#if defined(GDUT_ISTRING_REPAIR_ENABLE)
  #define GDUT_HAS_ISTRING_REPAIR 1
#else
  #define GDUT_HAS_ISTRING_REPAIR 0
#endif

//*************************************
// Option to enable repair-after-memcpy for ivector.
#if defined(GDUT_IVECTOR_REPAIR_ENABLE)
  #define GDUT_HAS_IVECTOR_REPAIR 1
#else
  #define GDUT_HAS_IVECTOR_REPAIR 0
#endif

//*************************************
// Option to enable repair-after-memcpy for ideque.
#if defined(GDUT_IDEQUE_REPAIR_ENABLE)
  #define GDUT_HAS_IDEQUE_REPAIR 1
#else
  #define GDUT_HAS_IDEQUE_REPAIR 0
#endif

//*************************************
// Option to enable repair-after-memcpy for icircular_buffer.
#if defined(GDUT_ICIRCULAR_BUFFER_REPAIR_ENABLE)
#define GDUT_HAS_ICIRCULAR_BUFFER_REPAIR 1
#else
#define GDUT_HAS_ICIRCULAR_BUFFER_REPAIR 0
#endif

//*************************************
// Indicate if C++ exceptions within the ETL are enabled.
#if defined(GDUT_THROW_EXCEPTIONS)
  #define GDUT_USING_EXCEPTIONS     1
  #define GDUT_NOT_USING_EXCEPTIONS 0
#else
  #define GDUT_USING_EXCEPTIONS     0
  #define GDUT_NOT_USING_EXCEPTIONS 1
#endif

//*************************************
// Indicate if C++ exceptions are enabled for debug asserts.
#if GDUT_IS_DEBUG_BUILD && defined(GDUT_DEBUG_THROW_EXCEPTIONS)
  #define GDUT_DEBUG_USING_EXCEPTIONS     1
  #define GDUT_DEBUG_NOT_USING_EXCEPTIONS 0
#else
  #define GDUT_DEBUG_USING_EXCEPTIONS     0
  #define GDUT_DEBUG_NOT_USING_EXCEPTIONS 1
#endif

//*************************************
// Indicate if nullptr is used.
#if GDUT_NO_NULLPTR_SUPPORT
  #define GDUT_HAS_NULLPTR 0
#else
  #define GDUT_HAS_NULLPTR 1
#endif

//*************************************
// Indicate if legacy bitset is used.
#if defined(GDUT_USE_LEGACY_BITSET)
  #define GDUT_USING_LEGACY_BITSET 1
#else
  #define GDUT_USING_LEGACY_BITSET 0
#endif

//*************************************
// Indicate if array_view is mutable.
#if defined(GDUT_ARRAY_VIEW_IS_MUTABLE)
  #define GDUT_HAS_MUTABLE_ARRAY_VIEW 1
#else
  #define GDUT_HAS_MUTABLE_ARRAY_VIEW 0
#endif

//*************************************
// Indicate if gdut::imassage is to be non-virtual.
#if defined(GDUT_MESSAGES_ARE_NOT_VIRTUAL)
  #define GDUT_HAS_VIRTUAL_MESSAGES 0
#else
  #define GDUT_HAS_VIRTUAL_MESSAGES 1
#endif

//*************************************
// Indicate if gdut::exception is to be derived from std::exception.
#if defined(GDUT_USE_STD_EXCEPTION)
#if GDUT_NOT_USING_STL
  #error "Requested std base for gdut::exception, but STL is not used"
#endif
  #define GDUT_USING_STD_EXCEPTION 1
#else
  #define GDUT_USING_STD_EXCEPTION 0
#endif

//*************************************
// Indicate if gdut::literals::chrono_literals uses ETL verbose style.
#if defined(GDUT_USE_VERBOSE_CHRONO_LITERALS) && GDUT_USING_CPP11
#define GDUT_USING_VERBOSE_CHRONO_LITERALS 1
#else
#define GDUT_USING_VERBOSE_CHRONO_LITERALS 0
#endif

//*************************************
// Indicate if gdut::literals::chrono_literals has days (_days)
#if defined(GDUT_DISABLE_CHRONO_LITERALS_DAY) && GDUT_USING_CPP11
  #define GDUT_HAS_CHRONO_LITERALS_DAY 0
#else
  #define GDUT_HAS_CHRONO_LITERALS_DAY 1
#endif

//*************************************
// Indicate if gdut::literals::chrono_literals has year (_years)
#if defined(GDUT_DISABLE_CHRONO_LITERALS_YEAR) && GDUT_USING_CPP11
  #define GDUT_HAS_CHRONO_LITERALS_YEAR 0
#else
  #define GDUT_HAS_CHRONO_LITERALS_YEAR 1
#endif

//*************************************
// Indicate if gdut::literals::chrono_literals has year (_hours, _minutes, _seconds, _milliseconds, _microseconds, _nanoseconds)
#if defined(GDUT_DISABLE_CHRONO_LITERALS_DURATION) && GDUT_USING_CPP11
#define GDUT_HAS_CHRONO_LITERALS_DURATION 0
#else
#define GDUT_HAS_CHRONO_LITERALS_DURATION 1
#endif

//*************************************
// Indicate if noexcept is part of the function type.
#if !defined(GDUT_HAS_NOEXCEPT_FUNCTION_TYPE)
  #if defined(__cpp_noexcept_function_type) && (__cpp_noexcept_function_type >= 201510)
    #define GDUT_HAS_NOEXCEPT_FUNCTION_TYPE 1
  #else
    #define GDUT_HAS_NOEXCEPT_FUNCTION_TYPE 0
  #endif
#endif

//*************************************
// The macros below are dependent on the profile.
// C++11
#if GDUT_USING_CPP11
  #define GDUT_CONSTEXPR                   constexpr
  #define GDUT_CONSTEXPR11                 constexpr // Synonym for GDUT_CONSTEXPR
  #define GDUT_CONSTANT                    constexpr
  #define GDUT_DELETE                      = delete
  #define GDUT_EXPLICIT                    explicit
  #define GDUT_OVERRIDE                    override
  #define GDUT_FINAL                       final
  #define GDUT_NORETURN                    [[noreturn]]
  #define GDUT_MOVE(x)                     gdut::move(x)
  #define GDUT_ENUM_CLASS(name)            enum class name
  #define GDUT_ENUM_CLASS_TYPE(name, type) enum class name : type
  #define GDUT_LVALUE_REF_QUALIFIER        &
  #if GDUT_USING_EXCEPTIONS
    #define GDUT_NOEXCEPT                  noexcept
    #define GDUT_NOEXCEPT_EXPR(...)        noexcept(__VA_ARGS__)
    #define GDUT_NOEXCEPT_FROM(x)          noexcept(noexcept(x))
  #else
    #define GDUT_NOEXCEPT
    #define GDUT_NOEXCEPT_EXPR(...)
    #define GDUT_NOEXCEPT_FROM(x) 
  #endif
#else
  #define GDUT_CONSTEXPR
  #define GDUT_CONSTEXPR11
  #define GDUT_CONSTANT                    const
  #define GDUT_DELETE
  #define GDUT_EXPLICIT
  #define GDUT_OVERRIDE
  #define GDUT_FINAL
  #define GDUT_NORETURN
  #define GDUT_NOEXCEPT
  #define GDUT_NOEXCEPT_EXPR(...)
  #define GDUT_NOEXCEPT_FROM(x) 
  #define GDUT_MOVE(x) x
  #define GDUT_ENUM_CLASS(name)            enum name
  #define GDUT_ENUM_CLASS_TYPE(name, type) enum name
  #define GDUT_LVALUE_REF_QUALIFIER
#endif

//*************************************
// C++14
#if GDUT_USING_CPP14
  #define GDUT_CONSTEXPR14  constexpr

  #if !defined(GDUT_IN_UNIT_TEST)   
    #define GDUT_DEPRECATED                [[deprecated]]
    #define GDUT_DEPRECATED_REASON(reason) [[deprecated(reason)]]
  #else
    #define GDUT_DEPRECATED
    #define GDUT_DEPRECATED_REASON(reason)
  #endif
#else
  #define GDUT_CONSTEXPR14
  #define GDUT_DEPRECATED
  #define GDUT_DEPRECATED_REASON(reason)
#endif

//*************************************
// C++17
#if GDUT_USING_CPP17
  #define GDUT_CONSTEXPR17  constexpr
  #define GDUT_IF_CONSTEXPR constexpr
  #define GDUT_NODISCARD    [[nodiscard]]
  #define GDUT_MAYBE_UNUSED [[maybe_unused]]
  #define GDUT_FALLTHROUGH  [[fallthrough]]
  #define GDUT_INLINE_VAR   inline
#else
  #define GDUT_CONSTEXPR17
  #define GDUT_IF_CONSTEXPR
  #define GDUT_NODISCARD
  #define GDUT_MAYBE_UNUSED
  #define GDUT_FALLTHROUGH
  #define GDUT_INLINE_VAR
#endif

//*************************************
// C++20
#if GDUT_USING_CPP20
  #define GDUT_LIKELY             [[likely]]
  #define GDUT_UNLIKELY           [[unlikely]]
  #define GDUT_CONSTEXPR20        constexpr
  #define GDUT_CONSTEVAL          consteval
  #define GDUT_CONSTINIT          constinit
  #define GDUT_NO_UNIQUE_ADDRESS  [[no_unique_address]]
  #define GDUT_EXPLICIT_EXPR(...) explicit(__VA_ARGS__)
#else
  #define GDUT_LIKELY
  #define GDUT_UNLIKELY
  #define GDUT_CONSTEXPR20
  #define GDUT_CONSTEVAL
  #define GDUT_CONSTINIT
  #define GDUT_NO_UNIQUE_ADDRESS
  #define GDUT_EXPLICIT_EXPR(...) explicit
#endif

#if GDUT_USING_CPP20 && GDUT_USING_STL
  #define GDUT_CONSTEXPR20_STL constexpr
#else
  #define GDUT_CONSTEXPR20_STL
#endif

//*************************************
// C++23
#if GDUT_USING_CPP23
  #define GDUT_ASSUME(expression) [[assume(expression)]]
#else
  #define GDUT_ASSUME GDUT_DO_NOTHING
#endif

//*************************************
// Determine if the ETL can use char8_t type.
#if GDUT_NO_SMALL_CHAR_SUPPORT
#include "private/diagnostic_cxx_20_compat_push.hpp"
  typedef uint_least8_t char8_t;
  #define GDUT_HAS_CHAR8_T 1
  #define GDUT_HAS_NATIVE_CHAR8_T 0
#include "private/diagnostic_pop.hpp"
#else
  #define GDUT_HAS_CHAR8_T 1
  #define GDUT_HAS_NATIVE_CHAR8_T 1
#endif

//*************************************
// Define the large character types if necessary.
#if GDUT_NO_LARGE_CHAR_SUPPORT
  typedef uint_least16_t char16_t;
  typedef uint_least32_t char32_t;
  #define GDUT_HAS_NATIVE_CHAR16_T 0
  #define GDUT_HAS_NATIVE_CHAR32_T 0
#else
  #define GDUT_HAS_NATIVE_CHAR16_T 1
  #define GDUT_HAS_NATIVE_CHAR32_T 1
#endif

//*************************************
// Determine if the ETL can use std::array
#if !defined(GDUT_HAS_STD_ARRAY)
  #if GDUT_USING_STL && GDUT_USING_CPP11
    #define GDUT_HAS_STD_ARRAY 1
  #else
    #define GDUT_HAS_STD_ARRAY 0
  #endif
#endif

//*************************************
// Determine if the ETL can use libc's wchar.h
#if !defined(GDUT_NO_LIBC_WCHAR_H)
  #if defined(__has_include)
    #if !__has_include(<wchar.h>)
        #define GDUT_NO_LIBC_WCHAR_H
    #endif
  #endif
#endif

#if defined(GDUT_NO_LIBC_WCHAR_H)
  #define GDUT_USING_LIBC_WCHAR_H     0
  #define GDUT_NOT_USING_LIBC_WCHAR_H 1
#else
  #define GDUT_USING_LIBC_WCHAR_H     1
  #define GDUT_NOT_USING_LIBC_WCHAR_H 0
#endif

//*************************************
// Determine if the ETL can use STL ostream
#if !defined(GDUT_NO_STD_OSTREAM) && GDUT_USING_STL
  #if defined(__has_include)
    #if !__has_include(<ostream>)
      #define GDUT_NO_STD_OSTREAM
    #endif
  #endif
#endif

#if defined(GDUT_NO_STD_OSTREAM) || (GDUT_NOT_USING_STL && !defined(GDUT_IN_UNIT_TEST))
  #define GDUT_USING_STD_OSTREAM     0
  #define GDUT_NOT_USING_STD_OSTREAM 1
#else
  #define GDUT_USING_STD_OSTREAM     1
  #define GDUT_NOT_USING_STD_OSTREAM 0
#endif

//*************************************
// Determine if the ETL should support atomics.
#if defined(GDUT_NO_ATOMICS) || \
    defined(GDUT_TARGET_DEVICE_ARM_CORTEX_M0) || \
    defined(GDUT_TARGET_DEVICE_ARM_CORTEX_M0_PLUS) || \
    defined(__STDC_NO_ATOMICS__)
  #define GDUT_HAS_ATOMIC 0
  #define GDUT_HAS_ATOMIC_ALWAYS_LOCK_FREE 0
#else
  #if ((GDUT_USING_CPP11 && (GDUT_USING_STL || defined(GDUT_IN_UNIT_TEST))) || \
        defined(GDUT_COMPILER_ARM5)  || \
        defined(GDUT_COMPILER_ARM6)  || \
        defined(GDUT_COMPILER_GCC)   || \
        defined(GDUT_COMPILER_CLANG))
    #define GDUT_HAS_ATOMIC 1
  #else
    #define GDUT_HAS_ATOMIC 0
  #endif
  #if ((GDUT_USING_CPP17 && (GDUT_USING_STL || defined(GDUT_IN_UNIT_TEST))) || \
        defined(GDUT_COMPILER_ARM5)  || \
        defined(GDUT_COMPILER_ARM6)  || \
        defined(GDUT_COMPILER_GCC)   || \
        defined(GDUT_COMPILER_CLANG))
    #define GDUT_HAS_ATOMIC_ALWAYS_LOCK_FREE 1
  #else
    #define GDUT_HAS_ATOMIC_ALWAYS_LOCK_FREE 0
  #endif
#endif

//*************************************
// Determine if the ETL should use std::initializer_list.
#if (defined(GDUT_FORCE_GDUT_INITIALIZER_LIST) && defined(GDUT_FORCE_STD_INITIALIZER_LIST))
  #error GDUT_FORCE_GDUT_INITIALIZER_LIST and GDUT_FORCE_STD_INITIALIZER_LIST both been defined. Choose one or neither.
#endif

#if (GDUT_USING_CPP11 && !defined(GDUT_NO_INITIALIZER_LIST))
  // Use the compiler's std::initializer_list?
  #if (GDUT_USING_STL && GDUT_NOT_USING_STLPORT && !defined(GDUT_FORCE_GDUT_INITIALIZER_LIST)) || defined(GDUT_IN_UNIT_TEST) || defined(GDUT_FORCE_STD_INITIALIZER_LIST)
    #define GDUT_HAS_INITIALIZER_LIST 1
  #else
    // Use the ETL's compatible version?
    #if defined(GDUT_COMPILER_MICROSOFT) || defined(GDUT_COMPILER_GCC)  || defined(GDUT_COMPILER_CLANG) || \
        defined(GDUT_COMPILER_ARM6) || defined(GDUT_COMPILER_ARM7) || defined(GDUT_COMPILER_IAR)   || \
        defined(GDUT_COMPILER_TEXAS_INSTRUMENTS) || defined(GDUT_COMPILER_INTEL)
      #define GDUT_HAS_INITIALIZER_LIST 1
    #else
      #define GDUT_HAS_INITIALIZER_LIST 0
    #endif
  #endif
#else
  #define GDUT_HAS_INITIALIZER_LIST 0
#endif

//*************************************
// Determine if the ETL should use __attribute__((packed).
#if defined(GDUT_COMPILER_CLANG) || defined(GDUT_COMPILER_GCC) || defined(GDUT_COMPILER_INTEL) || defined(GDUT_COMPILER_ARM6)
  #define GDUT_PACKED_CLASS(class_type)   class  __attribute__((packed)) class_type
  #define GDUT_PACKED_STRUCT(struct_type) struct __attribute__((packed)) struct_type
  #define GDUT_END_PACKED
  #define GDUT_HAS_PACKED 1
#elif defined(GDUT_COMPILER_MICROSOFT)
  #define GDUT_PACKED_CLASS(class_type)   __pragma(pack(push, 1)) class  class_type
  #define GDUT_PACKED_STRUCT(struct_type) __pragma(pack(push, 1)) struct struct_type
  #define GDUT_PACKED     
  #define GDUT_END_PACKED __pragma(pack(pop))
  #define GDUT_HAS_PACKED 1
#else
  #define GDUT_PACKED_CLASS(class_type)   class  class_type
  #define GDUT_PACKED_STRUCT(struct_type) struct struct_type
  #define GDUT_END_PACKED
  #define GDUT_HAS_PACKED 0
#endif

//*************************************
// Check for availability of certain builtins
#include "profiles/determine_builtin_support.hpp"

//*************************************
// Sort out namespaces for STL/No STL options.
#include "private/choose_namespace.hpp"

namespace gdut
{
  namespace traits
  {
    // Documentation: https://www.etlcpp.com/etl_traits.html
    // General
    static GDUT_CONSTANT long cplusplus                        = __cplusplus;
    static GDUT_CONSTANT int  language_standard                = GDUT_LANGUAGE_STANDARD;

    // Using...
    static GDUT_CONSTANT bool using_stl                        = (GDUT_USING_STL == 1);
    static GDUT_CONSTANT bool using_stlport                    = (GDUT_USING_STLPORT == 1);
    static GDUT_CONSTANT bool using_cpp11                      = (GDUT_USING_CPP11 == 1);
    static GDUT_CONSTANT bool using_cpp14                      = (GDUT_USING_CPP14 == 1);
    static GDUT_CONSTANT bool using_cpp17                      = (GDUT_USING_CPP17 == 1);
    static GDUT_CONSTANT bool using_cpp20                      = (GDUT_USING_CPP20 == 1);
    static GDUT_CONSTANT bool using_cpp23                      = (GDUT_USING_CPP23 == 1);
    static GDUT_CONSTANT bool using_gcc_compiler               = (GDUT_USING_GCC_COMPILER == 1);
    static GDUT_CONSTANT bool using_microsoft_compiler         = (GDUT_USING_MICROSOFT_COMPILER == 1);
    static GDUT_CONSTANT bool using_arm5_compiler              = (GDUT_USING_ARM5_COMPILER == 1);
    static GDUT_CONSTANT bool using_arm6_compiler              = (GDUT_USING_ARM6_COMPILER == 1);
    static GDUT_CONSTANT bool using_arm7_compiler              = (GDUT_USING_ARM7_COMPILER == 1);
    static GDUT_CONSTANT bool using_clang_compiler             = (GDUT_USING_CLANG_COMPILER == 1);
    static GDUT_CONSTANT bool using_green_hills_compiler       = (GDUT_USING_GREEN_HILLS_COMPILER == 1);
    static GDUT_CONSTANT bool using_iar_compiler               = (GDUT_USING_IAR_COMPILER == 1);
    static GDUT_CONSTANT bool using_intel_compiler             = (GDUT_USING_INTEL_COMPILER == 1);
    static GDUT_CONSTANT bool using_texas_instruments_compiler = (GDUT_USING_TEXAS_INSTRUMENTS_COMPILER == 1);
    static GDUT_CONSTANT bool using_generic_compiler           = (GDUT_USING_GENERIC_COMPILER == 1);
    static GDUT_CONSTANT bool using_legacy_bitset              = (GDUT_USING_LEGACY_BITSET == 1);
    static GDUT_CONSTANT bool using_exceptions                 = (GDUT_USING_EXCEPTIONS == 1);
    static GDUT_CONSTANT bool using_libc_wchar_h               = (GDUT_USING_LIBC_WCHAR_H == 1);
    static GDUT_CONSTANT bool using_std_exception              = (GDUT_USING_STD_EXCEPTION == 1);
    
    // Has...
    static GDUT_CONSTANT bool has_initializer_list             = (GDUT_HAS_INITIALIZER_LIST == 1);
    static GDUT_CONSTANT bool has_8bit_types                   = (GDUT_USING_8BIT_TYPES == 1);
    static GDUT_CONSTANT bool has_64bit_types                  = (GDUT_USING_64BIT_TYPES == 1);
    static GDUT_CONSTANT bool has_atomic                       = (GDUT_HAS_ATOMIC == 1);
    static GDUT_CONSTANT bool has_atomic_always_lock_free      = (GDUT_HAS_ATOMIC_ALWAYS_LOCK_FREE == 1);
    static GDUT_CONSTANT bool has_nullptr                      = (GDUT_HAS_NULLPTR == 1);
    static GDUT_CONSTANT bool has_char8_t                      = (GDUT_HAS_CHAR8_T == 1);
    static GDUT_CONSTANT bool has_native_char8_t               = (GDUT_HAS_NATIVE_CHAR8_T == 1);
    static GDUT_CONSTANT bool has_native_char16_t              = (GDUT_HAS_NATIVE_CHAR16_T == 1);
    static GDUT_CONSTANT bool has_native_char32_t              = (GDUT_HAS_NATIVE_CHAR32_T == 1);
    static GDUT_CONSTANT bool has_string_truncation_checks     = (GDUT_HAS_STRING_TRUNCATION_CHECKS == 1);
    static GDUT_CONSTANT bool has_error_on_string_truncation   = (GDUT_HAS_ERROR_ON_STRING_TRUNCATION == 1);
    static GDUT_CONSTANT bool has_string_clear_after_use       = (GDUT_HAS_STRING_CLEAR_AFTER_USE == 1);
    static GDUT_CONSTANT bool has_istring_repair               = (GDUT_HAS_ISTRING_REPAIR == 1);
    static GDUT_CONSTANT bool has_ivector_repair               = (GDUT_HAS_IVECTOR_REPAIR == 1);
    static GDUT_CONSTANT bool has_icircular_buffer_repair      = (GDUT_HAS_ICIRCULAR_BUFFER_REPAIR == 1);
    static GDUT_CONSTANT bool has_mutable_array_view           = (GDUT_HAS_MUTABLE_ARRAY_VIEW == 1);
    static GDUT_CONSTANT bool has_ideque_repair                = (GDUT_HAS_IDEQUE_REPAIR == 1);
    static GDUT_CONSTANT bool has_virtual_messages             = (GDUT_HAS_VIRTUAL_MESSAGES == 1);
    static GDUT_CONSTANT bool has_packed                       = (GDUT_HAS_PACKED == 1);
    static GDUT_CONSTANT bool has_chrono_literals_day          = (GDUT_HAS_CHRONO_LITERALS_DAY == 1);
    static GDUT_CONSTANT bool has_chrono_literals_year         = (GDUT_HAS_CHRONO_LITERALS_YEAR == 1);
    static GDUT_CONSTANT bool has_chrono_literals_hours        = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_chrono_literals_minutes      = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_chrono_literals_seconds      = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_chrono_literals_milliseconds = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_chrono_literals_microseconds = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_chrono_literals_nanoseconds  = (GDUT_HAS_CHRONO_LITERALS_DURATION == 1);
    static GDUT_CONSTANT bool has_std_byteswap                 = (GDUT_HAS_STD_BYTESWAP == 1);
    static GDUT_CONSTANT bool has_noexcept_function_type       = (GDUT_HAS_NOEXCEPT_FUNCTION_TYPE == 1);

    // Is...
    static GDUT_CONSTANT bool is_debug_build                   = (GDUT_IS_DEBUG_BUILD == 1);
  }
}

#endif
