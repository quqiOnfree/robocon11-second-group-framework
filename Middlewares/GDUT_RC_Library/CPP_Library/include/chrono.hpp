///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2023 John Wellbelove

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

#ifndef GDUT_CHRONO_INCLUDED
#define GDUT_CHRONO_INCLUDED

#define GDUT_IN_CHRONO_H

#include "platform.hpp"

#if GDUT_NOT_USING_CPP11 && !defined(GDUT_IN_UNIT_TEST)
#error NOT SUPPORTED FOR C++03 OR BELOW
#endif

#if GDUT_USING_CPP11

#include "hash.hpp"
#include "integral_limits.hpp"
#include "type_traits.hpp"

#include <stdint.h>
#include <time.h>

namespace gdut {
namespace chrono {
template <typename TRep>
struct treat_as_floating_point : gdut::is_floating_point<TRep> {};

#if GDUT_USING_CPP17
template <typename TRep>
constexpr bool treat_as_floating_point_v = treat_as_floating_point<TRep>::value;
#endif
} // namespace chrono

// Use the same type as defined in time.h.
using time_t = ::time_t;
} // namespace gdut

// clang-format off
// Keeping the order is important here
#include "private/chrono/last_spec.hpp"
#include "private/chrono/duration.hpp"
#include "private/chrono/time_point.hpp"
#include "private/chrono/clocks.hpp"
#include "private/chrono/day.hpp"
#include "private/chrono/weekday.hpp"
#include "private/chrono/month.hpp"
#include "private/chrono/month_day.hpp"
#include "private/chrono/month_weekday.hpp"
#include "private/chrono/year.hpp"
#include "private/chrono/year_month.hpp"
#include "private/chrono/year_month_day.hpp"
#include "private/chrono/year_month_weekday.hpp"
#include "private/chrono/hh_mm_ss.hpp"
#include "private/chrono/operators.hpp"
#include "private/chrono/time_zone.hpp"
// clang-format on

namespace gdut {
namespace chrono {
using namespace literals::chrono_literals;
}
} // namespace gdut

#endif

#undef GDUT_IN_CHRONO_H

#endif
