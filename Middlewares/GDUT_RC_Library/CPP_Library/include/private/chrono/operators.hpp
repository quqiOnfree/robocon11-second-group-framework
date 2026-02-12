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

namespace gdut {
namespace chrono {
//*************************************************************************
// month_day
inline GDUT_CONSTEXPR14 gdut::chrono::month_day
operator/(const gdut::chrono::month &m,
          const gdut::chrono::day &d) GDUT_NOEXCEPT {
  return gdut::chrono::month_day(m, d);
}

// month_day
inline GDUT_CONSTEXPR14 gdut::chrono::month_day
operator/(const gdut::chrono::month &m, int d) GDUT_NOEXCEPT {
  return gdut::chrono::month_day(m, gdut::chrono::day(d));
}

// month_day
inline GDUT_CONSTEXPR14 gdut::chrono::month_day
operator/(int m, const gdut::chrono::day &d) GDUT_NOEXCEPT {
  return gdut::chrono::month_day(gdut::chrono::month(m), d);
}

// month_day
inline GDUT_CONSTEXPR14 gdut::chrono::month_day
operator/(const gdut::chrono::day &d,
          const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return gdut::chrono::month_day(m, d);
}

// month_day
inline GDUT_CONSTEXPR14 gdut::chrono::month_day
operator/(const gdut::chrono::day &d, int m) GDUT_NOEXCEPT {
  return gdut::chrono::month_day(gdut::chrono::month(m), d);
}

//*************************************************************************
// month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_day_last
operator/(const gdut::chrono::month &m, gdut::chrono::last_spec) GDUT_NOEXCEPT {
  return gdut::chrono::month_day_last(m);
}

// month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_day_last
operator/(int m, gdut::chrono::last_spec) GDUT_NOEXCEPT {
  return gdut::chrono::month_day_last(gdut::chrono::month(m));
}

// month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_day_last
operator/(gdut::chrono::last_spec, const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return gdut::chrono::month_day_last(m);
}

// month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_day_last
operator/(gdut::chrono::last_spec, int m) GDUT_NOEXCEPT {
  return gdut::chrono::month_day_last(gdut::chrono::month(m));
}

//*************************************************************************
// month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday
operator/(const gdut::chrono::month &m,
          const gdut::chrono::weekday_indexed &wdi) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday(m, wdi);
}

// month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday
operator/(int m, const gdut::chrono::weekday_indexed &wdi) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday(gdut::chrono::month(m), wdi);
}

// month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday
operator/(const gdut::chrono::weekday_indexed &wdi,
          const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday(m, wdi);
}

// month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday
operator/(const gdut::chrono::weekday_indexed &wdi, int m) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday(gdut::chrono::month(m), wdi);
}

//*************************************************************************
// month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday_last
operator/(const gdut::chrono::month &m,
          const gdut::chrono::weekday_last &wdl) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday_last(m, wdl);
}

// month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday_last
operator/(int m, const gdut::chrono::weekday_last &wdl) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday_last(gdut::chrono::month(m), wdl);
}

// month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday_last
operator/(const gdut::chrono::weekday_last &wdl,
          const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday_last(m, wdl);
}

// month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::month_weekday_last
operator/(const gdut::chrono::weekday_last &wdl, int m) GDUT_NOEXCEPT {
  return gdut::chrono::month_weekday_last(gdut::chrono::month(m), wdl);
}

//*************************************************************************
// year_month
inline GDUT_CONSTEXPR14 gdut::chrono::year_month
operator/(const gdut::chrono::year &y,
          const gdut::chrono::month &m) GDUT_NOEXCEPT {
  return gdut::chrono::year_month(y, m);
}

// year_month
inline GDUT_CONSTEXPR14 gdut::chrono::year_month
operator/(const gdut::chrono::year &y, int m) GDUT_NOEXCEPT {
  return gdut::chrono::year_month(y, gdut::chrono::month(m));
}

////*************************************************************************
// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(const gdut::chrono::year_month &ym,
          const gdut::chrono::day &d) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ym.year(), ym.month(), d);
}

// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(const gdut::chrono::year_month &ym, int d) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(ym.year(), ym.month(),
                                      gdut::chrono::day(d));
}

// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(const gdut::chrono::year &y,
          const gdut::chrono::month_day &md) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(y, md.month(), md.day());
}

// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(int y, const gdut::chrono::month_day &md) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(gdut::chrono::year(y), md.month(),
                                      md.day());
}

// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(const gdut::chrono::month_day &md,
          const gdut::chrono::year &y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(y, md.month(), md.day());
}

// year_month_day
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day
operator/(const gdut::chrono::month_day &md, int y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day(gdut::chrono::year(y), md.month(),
                                      md.day());
}

//*************************************************************************
// year_month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator/(const gdut::chrono::year_month &ym,
          gdut::chrono::last_spec) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(
      ym.year(), gdut::chrono::month_day_last(ym.month()));
}

// year_month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator/(const gdut::chrono::year &y,
          const gdut::chrono::month_day_last &mdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(y, mdl);
}

// year_month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator/(int y, const gdut::chrono::month_day_last &mdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(gdut::chrono::year(y), mdl);
}

// year_month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator/(const gdut::chrono::month_day_last &mdl,
          const gdut::chrono::year &y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(y, mdl);
}

// year_month_day_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_day_last
operator/(const gdut::chrono::month_day_last &mdl, int y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_day_last(gdut::chrono::year(y), mdl);
}

//*************************************************************************
// year_month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday
operator/(const gdut::chrono::year_month &ym,
          const gdut::chrono::weekday_indexed &wdi) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday(ym.year(), ym.month(), wdi);
}

// year_month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday
operator/(const gdut::chrono::year &y,
          const gdut::chrono::month_weekday &mwd) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday(y, mwd.month(),
                                          mwd.weekday_indexed());
}

// year_month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday
operator/(int y, const gdut::chrono::month_weekday &mwd) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday(gdut::chrono::year(y), mwd.month(),
                                          mwd.weekday_indexed());
}

// year_month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday
operator/(const gdut::chrono::month_weekday &mwd,
          const gdut::chrono::year &y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday(y, mwd.month(),
                                          mwd.weekday_indexed());
}

// year_month_weekday
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday
operator/(const gdut::chrono::month_weekday &mwd, int y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday(gdut::chrono::year(y), mwd.month(),
                                          mwd.weekday_indexed());
}

//*************************************************************************
// year_month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last
operator/(const gdut::chrono::year_month &ym,
          const gdut::chrono::weekday_last &wdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday_last(ym.year(), ym.month(), wdl);
}

// year_month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last
operator/(const gdut::chrono::year &y,
          const gdut::chrono::month_weekday_last &mwdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday_last(y, mwdl.month(),
                                               mwdl.weekday_last());
}

// year_month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last
operator/(int y, const gdut::chrono::month_weekday_last &mwdl) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday_last(
      gdut::chrono::year(y), mwdl.month(), mwdl.weekday_last());
}

// year_month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last
operator/(const gdut::chrono::month_weekday_last &mwdl,
          const gdut::chrono::year &y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday_last(y, mwdl.month(),
                                               mwdl.weekday_last());
}

// year_month_weekday_last
inline GDUT_CONSTEXPR14 gdut::chrono::year_month_weekday_last
operator/(const gdut::chrono::month_weekday_last &mwdl, int y) GDUT_NOEXCEPT {
  return gdut::chrono::year_month_weekday_last(
      gdut::chrono::year(y), mwdl.month(), mwdl.weekday_last());
}
} // namespace chrono
} // namespace gdut