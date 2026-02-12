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

#ifndef GDUT_VERSION_INCLUDED
#define GDUT_VERSION_INCLUDED

#include "platform.hpp"
#include "macros.hpp"

///\defgroup version version
/// Definitions of the ETL version
///\ingroup utilities

#define GDUT_VERSION_MAJOR 20
#define GDUT_VERSION_MINOR 45
#define GDUT_VERSION_PATCH 0

#define GDUT_VERSION       GDUT_STRING(GDUT_VERSION_MAJOR) "." GDUT_STRING(GDUT_VERSION_MINOR) "." GDUT_STRING(GDUT_VERSION_PATCH)
#define GDUT_VERSION_W     GDUT_WIDE_STRING(GDUT_VERSION_MAJOR) L"." GDUT_WIDE_STRING(GDUT_VERSION_MINOR) L"." GDUT_WIDE_STRING(GDUT_VERSION_PATCH)
#if GDUT_HAS_CHAR8_T
  #define GDUT_VERSION_U8  GDUT_U8_STRING(GDUT_VERSION_MAJOR) u8"." GDUT_U8_STRING(GDUT_VERSION_MINOR) u8"." GDUT_U8_STRING(GDUT_VERSION_PATCH)
#endif
  #define GDUT_VERSION_U16 GDUT_U16_STRING(GDUT_VERSION_MAJOR) u"." GDUT_U16_STRING(GDUT_VERSION_MINOR) u"." GDUT_U16_STRING(GDUT_VERSION_PATCH)
  #define GDUT_VERSION_U32 GDUT_U32_STRING(GDUT_VERSION_MAJOR) U"." GDUT_U32_STRING(GDUT_VERSION_MINOR) U"." GDUT_U32_STRING(GDUT_VERSION_PATCH)
#define GDUT_VERSION_VALUE ((GDUT_VERSION_MAJOR * 10000) + (GDUT_VERSION_MINOR * 100) + GDUT_VERSION_PATCH)

namespace gdut
{
  namespace traits
  {
    static GDUT_CONSTANT long version                   = GDUT_VERSION_VALUE;
    static GDUT_CONSTANT long version_major             = GDUT_VERSION_MAJOR;
    static GDUT_CONSTANT long version_minor             = GDUT_VERSION_MINOR;
    static GDUT_CONSTANT long version_patch             = GDUT_VERSION_PATCH;

#if GDUT_USING_CPP11
    static constexpr const char* version_string        = GDUT_VERSION;
    static constexpr const wchar_t*  version_wstring   = GDUT_VERSION_W;
  #if GDUT_HAS_NATIVE_CHAR8_T
    static constexpr const char8_t*  version_u8string  = GDUT_VERSION_U8;
  #endif
    static constexpr const char16_t* version_u16string = GDUT_VERSION_U16;
    static constexpr const char32_t* version_u32string = GDUT_VERSION_U32;
#else
    static const char*               version_string    = GDUT_VERSION;
    static const wchar_t*            version_wstring   = GDUT_VERSION_W;
#endif
  }
}

#endif

