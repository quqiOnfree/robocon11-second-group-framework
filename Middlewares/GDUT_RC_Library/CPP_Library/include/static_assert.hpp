/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2014 John Wellbelove

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

#ifndef GDUT_GDUT_STATIC_ASSERT_INCLUDED
#define GDUT_GDUT_STATIC_ASSERT_INCLUDED

#include "platform.hpp"

#if (GDUT_USING_CPP11)
  #define GDUT_STATIC_ASSERT(Condition, Message) static_assert(Condition, Message)
#else
  template <bool Condition>
  struct GDUT_GDUT_STATIC_ASSERT_FAILED;

  template <>
  struct GDUT_GDUT_STATIC_ASSERT_FAILED<true> {};

  #define GDUT_SA1(a,b) a##b
  #define GDUT_SA2(a,b) GDUT_SA1(a,b)
  #define GDUT_STATIC_ASSERT(Condition, Message) \
		  enum \
		  { \
        GDUT_SA2(dummy, __LINE__) = sizeof(GDUT_GDUT_STATIC_ASSERT_FAILED<static_cast<bool>(Condition)>) \
	    }
#endif

#endif
