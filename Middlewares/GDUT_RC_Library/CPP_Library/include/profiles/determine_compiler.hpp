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

#ifndef GDUT_DETERMINE_COMPILER_H_INCLUDED
#define GDUT_DETERMINE_COMPILER_H_INCLUDED

//*****************************************************************************
// Macros that are conditionally defined.
//*****************************************************************************
#if !defined(GDUT_COMPILER_GCC) && !defined(GDUT_COMPILER_MICROSOFT) &&        \
    !defined(GDUT_COMPILER_ARM5) && !defined(GDUT_COMPILER_ARM6) &&            \
    !defined(GDUT_COMPILER_ARM7) && !defined(GDUT_COMPILER_CLANG) &&           \
    !defined(GDUT_COMPILER_GREEN_HILLS) && !defined(GDUT_COMPILER_IAR) &&      \
    !defined(GDUT_COMPILER_INTEL) &&                                           \
    !defined(GDUT_COMPILER_TEXAS_INSTRUMENTS) &&                               \
    !defined(GDUT_COMPILER_TASKING) && !defined(GDUT_COMPILER_GENERIC)

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_ARM5)
#if defined(__CC_ARM)
#define GDUT_COMPILER_ARM5
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_ARM6)
#if defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 6000000L) &&   \
    (__ARMCOMPILER_VERSION < 7000000L)
#define GDUT_COMPILER_ARM6
#define GDUT_COMPILER_TYPE_DETECTED 1
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_ARM7)
#if defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 7000000L) &&   \
    (__ARMCOMPILER_VERSION < 8000000L)
#define GDUT_COMPILER_ARM7
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_GCC)
#if defined(__GNUC__) && !defined(__clang__) && !defined(__llvm__)
#define GDUT_COMPILER_GCC
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_CLANG)
#if defined(__clang__) || defined(__llvm__)
#define GDUT_COMPILER_CLANG
#define GDUT_COMPILER_TYPE_DETECTED
#if defined(__AVR__) && (__AVR__ == 1)
#define GDUT_CROSS_COMPILING_TO_AVR
#endif
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_GREEN_HILLS)
#if defined(__ghs__)
#define GDUT_COMPILER_GREEN_HILLS
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_IAR)
#if defined(__IAR_SYSTEMS_ICC__)
#define GDUT_COMPILER_IAR
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_INTEL)
#if defined(__INTEL_COMPILER)
#define GDUT_COMPILER_INTEL
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) &&                                   \
    !defined(GDUT_COMPILER_TEXAS_INSTRUMENTS)
#if defined(__TI_COMPILER_VERSION__)
#define GDUT_COMPILER_TEXAS_INSTRUMENTS
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_MICROSOFT)
#if defined(_MSC_VER)
#define GDUT_COMPILER_MICROSOFT
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED) && !defined(GDUT_COMPILER_TASKING)
#if defined(__TASKING__)
#define GDUT_COMPILER_TASKING
#define GDUT_COMPILER_TYPE_DETECTED
#endif
#endif

#if !defined(GDUT_COMPILER_TYPE_DETECTED)
#define GDUT_COMPILER_GENERIC
#endif
#endif

//*****************************************************************************
// 'Using' macros that are always defined.
//*****************************************************************************
#if defined(GDUT_COMPILER_GCC)
#define GDUT_USING_GCC_COMPILER 1
#else
#define GDUT_USING_GCC_COMPILER 0
#endif

#if defined(GDUT_COMPILER_MICROSOFT)
#define GDUT_USING_MICROSOFT_COMPILER 1
#else
#define GDUT_USING_MICROSOFT_COMPILER 0
#endif

#if defined(GDUT_COMPILER_ARM5)
#define GDUT_USING_ARM5_COMPILER 1
#else
#define GDUT_USING_ARM5_COMPILER 0
#endif

#if defined(GDUT_COMPILER_ARM6)
#define GDUT_USING_ARM6_COMPILER 1
#else
#define GDUT_USING_ARM6_COMPILER 0
#endif

#if defined(GDUT_COMPILER_ARM7)
#define GDUT_USING_ARM7_COMPILER 1
#else
#define GDUT_USING_ARM7_COMPILER 0
#endif

#if defined(GDUT_COMPILER_CLANG)
#define GDUT_USING_CLANG_COMPILER 1
#else
#define GDUT_USING_CLANG_COMPILER 0
#endif

#if defined(GDUT_COMPILER_GREEN_HILLS)
#define GDUT_USING_GREEN_HILLS_COMPILER 1
#else
#define GDUT_USING_GREEN_HILLS_COMPILER 0
#endif

#if defined(GDUT_COMPILER_IAR)
#define GDUT_USING_IAR_COMPILER 1
#else
#define GDUT_USING_IAR_COMPILER 0
#endif

#if defined(GDUT_COMPILER_INTEL)
#define GDUT_USING_INTEL_COMPILER 1
#else
#define GDUT_USING_INTEL_COMPILER 0
#endif

#if defined(GDUT_COMPILER_TEXAS_INSTRUMENTS)
#define GDUT_USING_TEXAS_INSTRUMENTS_COMPILER 1
#else
#define GDUT_USING_TEXAS_INSTRUMENTS_COMPILER 0
#endif

#if defined(GDUT_COMPILER_TASKING)
#define GDUT_USING_TASKING_COMPILER 1
#else
#define GDUT_USING_TASKING_COMPILER 0
#endif

#if defined(GDUT_COMPILER_GENERIC)
#define GDUT_USING_GENERIC_COMPILER 1
#else
#define GDUT_USING_GENERIC_COMPILER 0
#endif

#endif
