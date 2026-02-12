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

#ifndef GDUT_INVERT_INCLUDED
#define GDUT_INVERT_INCLUDED

#include "functional.hpp"
#include "limits.hpp"
#include "platform.hpp"

#include <stdint.h>

namespace gdut {
//***************************************************************************
/// Invert.
//***************************************************************************
template <typename TInput>
class invert : public gdut::unary_function<TInput, TInput> {
public:
  //*****************************************************************
  // Constructor.
  //*****************************************************************
  invert()
      : offset(TInput(0)), minuend((gdut::numeric_limits<TInput>::is_signed)
                                       ? TInput(0)
                                       : gdut::numeric_limits<TInput>::max()) {}

  //*****************************************************************
  // Constructor.
  //*****************************************************************
  invert(TInput offset_, TInput minuend_)
      : offset(offset_), minuend(minuend_) {}

  //*****************************************************************
  // operator ()
  //*****************************************************************
  TInput operator()(TInput value) const { return minuend - (value - offset); }

private:
  const TInput offset;
  const TInput minuend;
};
} // namespace gdut

#endif
