///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef GDUT_SEGGER_GCC_INCLUDED
#define GDUT_SEGGER_GCC_INCLUDED

//*****************************************************************************
// GCC
//*****************************************************************************

#define GDUT_TARGET_DEVICE_GENERIC
#define GDUT_TARGET_OS_NONE
#ifdef __cplusplus
#define GDUT_CPP11_SUPPORTED (__cplusplus >= 201103L)
#define GDUT_CPP14_SUPPORTED (__cplusplus >= 201402L)
#define GDUT_CPP17_SUPPORTED (__cplusplus >= 201703L)
#else
#define GDUT_CPP11_SUPPORTED 0
#define GDUT_CPP14_SUPPORTED 0
#define GDUT_CPP17_SUPPORTED 0
#endif
#define GDUT_NO_NULLPTR_SUPPORT 1
#define GDUT_NO_LARGE_CHAR_SUPPORT GDUT_CPP11_NOT_SUPPORTED
#define GDUT_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED 0
#define GDUT_STLPORT 1

#endif
