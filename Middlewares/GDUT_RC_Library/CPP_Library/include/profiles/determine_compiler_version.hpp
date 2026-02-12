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

#ifndef GDUT_DETERMINE_COMPILER_VERSION_H_INCLUDED
#define GDUT_DETERMINE_COMPILER_VERSION_H_INCLUDED

#include "determine_compiler.hpp"

#if !defined(GDUT_COMPILER_VERSION) && !defined(GDUT_COMPILER_FULL_VERSION)
#if defined(GDUT_COMPILER_GCC)
#define GDUT_COMPILER_VERSION __GNUC__
#define GDUT_COMPILER_FULL_VERSION                                             \
  ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#elif defined(GDUT_COMPILER_MICROSOFT)
#define GDUT_COMPILER_VERSION _MSC_VER
#define GDUT_COMPILER_FULL_VERSION _MSC_FULL_VER
#elif defined(GDUT_COMPILER_ARM5)
#define GDUT_COMPILER_VERSION __ARMCC_VERSION
#define GDUT_COMPILER_FULL_VERSION __ARMCC_VERSION
#elif defined(GDUT_COMPILER_ARM6)
#define GDUT_COMPILER_VERSION __ARMCOMPILER_VERSION
#define GDUT_COMPILER_FULL_VERSION __ARMCOMPILER_VERSION
#elif defined(GDUT_COMPILER_ARM7)
#define GDUT_COMPILER_VERSION __ARMCOMPILER_VERSION
#define GDUT_COMPILER_FULL_VERSION __ARMCOMPILER_VERSION
#elif defined(GDUT_COMPILER_CLANG)
#define GDUT_COMPILER_VERSION __clang_version__
#define GDUT_COMPILER_FULL_VERSION                                             \
  ((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
#elif defined(GDUT_COMPILER_GREEN_HILLS)
#define GDUT_COMPILER_VERSION __GHS_VERSION_NUMBER__
#define GDUT_COMPILER_FULL_VERSION __GHS_VERSION_NUMBER__
#elif defined(GDUT_COMPILER_IAR)
#define GDUT_COMPILER_VERSION __VER__
#define GDUT_COMPILER_FULL_VERSION __VER__
#elif defined(GDUT_COMPILER_INTEL)
#define GDUT_COMPILER_VERSION __INTEL_COMPILER
#define GDUT_COMPILER_FULL_VERSION __INTEL_COMPILER
#elif defined(GDUT_COMPILER_TEXAS_INSTRUMENTS)
#define GDUT_COMPILER_VERSION __TI_COMPILER_VERSION__
#define GDUT_COMPILER_FULL_VERSION __TI_COMPILER_VERSION__
#elif defined(GDUT_COMPILER_TASKING)
#define GDUT_COMPILER_VERSION __REVISION__
#define GDUT_COMPILER_FULL_VERSION __VERSION__
#else
#define GDUT_COMPILER_VERSION 0
#define GDUT_COMPILER_FULL_VERSION 0
#endif
#endif

#endif
