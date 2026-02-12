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

#ifndef GDUT_IN_CHRONO_H
#error DO NOT DIRECTLY INCLUDE THIS FILE. USE CHRONO.H
#endif

#if !defined(GDUT_CHRONO_SYSTEM_CLOCK_DURATION)
#define GDUT_CHRONO_SYSTEM_CLOCK_DURATION gdut::chrono::nanoseconds
#endif

#if !defined(GDUT_CHRONO_SYSTEM_CLOCK_IS_STEADY)
#define GDUT_CHRONO_SYSTEM_CLOCK_IS_STEADY true
#endif

#if !defined(GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_DURATION)
#define GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_DURATION gdut::chrono::nanoseconds
#endif

#if !defined(GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_IS_STEADY)
#define GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_IS_STEADY true
#endif

#if !defined(GDUT_CHRONO_STEADY_CLOCK_DURATION)
#define GDUT_CHRONO_STEADY_CLOCK_DURATION gdut::chrono::nanoseconds
#endif

extern "C" GDUT_CHRONO_SYSTEM_CLOCK_DURATION::rep etl_get_system_clock();
extern "C" GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_DURATION::rep
etl_get_high_resolution_clock();
extern "C" GDUT_CHRONO_STEADY_CLOCK_DURATION::rep etl_get_steady_clock();

namespace gdut {
namespace chrono {
namespace private_chrono {
template <bool b> struct is_steady_trait {
  static GDUT_CONSTANT bool is_steady = b;
};

template <bool b> GDUT_CONSTANT bool is_steady_trait<b>::is_steady;
} // namespace private_chrono

//*************************************************************************
/// The system clock time
//*************************************************************************
class system_clock : public private_chrono::is_steady_trait<
                         GDUT_CHRONO_SYSTEM_CLOCK_IS_STEADY> {
public:
  using duration = GDUT_CHRONO_SYSTEM_CLOCK_DURATION;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = gdut::chrono::time_point<system_clock, duration>;

  //*************************************************************************
  static time_point now() GDUT_NOEXCEPT {
    return time_point(duration(etl_get_system_clock()));
  }

  //*************************************************************************
  static gdut::time_t to_time_t(const time_point &t) GDUT_NOEXCEPT {
    // Get the duration since the epoch
    duration dur = t.time_since_epoch();

    // Convert the duration to seconds
    return dur.count() / duration::period::den;
  }

  //*************************************************************************
  static time_point from_time_t(gdut::time_t t) GDUT_NOEXCEPT {
    // Convert seconds to the appropriate duration
    duration dur(t * duration::period::den);

    // Construct and return the time_point
    return time_point(dur);
  }
};

//*************************************************************************
/// The high resolution clock time
//*************************************************************************
class high_resolution_clock : public private_chrono::is_steady_trait<
                                  GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_IS_STEADY> {
public:
  using duration = GDUT_CHRONO_HIGH_RESOLUTION_CLOCK_DURATION;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = gdut::chrono::time_point<high_resolution_clock, duration>;

  //*************************************************************************
  static time_point now() GDUT_NOEXCEPT {
    return time_point(duration(etl_get_high_resolution_clock()));
  }
};

//*************************************************************************
/// The steady clock time
//*************************************************************************
class steady_clock : public private_chrono::is_steady_trait<true> {
public:
  using duration = GDUT_CHRONO_STEADY_CLOCK_DURATION;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = gdut::chrono::time_point<steady_clock, duration>;

  //*************************************************************************
  static time_point now() GDUT_NOEXCEPT {
    return time_point(duration(etl_get_steady_clock()));
  }
};

//***************************************************************************
/// System time
//***************************************************************************
template <typename Duration>
using sys_time = gdut::chrono::time_point<gdut::chrono::system_clock, Duration>;

using sys_seconds = sys_time<gdut::chrono::seconds>;
using sys_days = sys_time<gdut::chrono::days>;

//***************************************************************************
/// Local time
//***************************************************************************
struct local_t {};

template <typename TDuration>
using local_time = gdut::chrono::time_point<gdut::chrono::local_t, TDuration>;

using local_seconds = local_time<gdut::chrono::seconds>;
using local_days = local_time<gdut::chrono::days>;

//*************************************************************************
/// Cast a time point from one clock to another.
/// This implementation assumes all clock epochs are the same.
//*************************************************************************
template <typename TToClock, typename TFromClock, typename TFromDuration>
GDUT_CONSTEXPR14 gdut::chrono::time_point<TToClock, typename TToClock::duration>
clock_cast(const gdut::chrono::time_point<TFromClock, TFromDuration>
               &from_time_point) GDUT_NOEXCEPT {
  // Get the duration since the epoch of the FromClock
  auto from_duration = from_time_point.time_since_epoch();

  // Convert the duration to the ToClock's duration type
  auto to_duration =
      gdut::chrono::duration_cast<typename TToClock::duration>(from_duration);

  // Construct and return the time_point for the ToClock
  return gdut::chrono::time_point<TToClock, typename TToClock::duration>(
      to_duration);
}
} // namespace chrono
} // namespace gdut