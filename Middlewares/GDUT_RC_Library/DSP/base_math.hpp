#ifndef BASE_MATH_HPP
#define BASE_MATH_HPP

#include <arm_math.h>

namespace gdut::dsp {

template <typename T> inline T cos(T x) { return std::cos(x); }

template <> inline float cos(float x) { return arm_cos_f32(x); }

template <typename T> inline T sin(T x) { return std::sin(x); }

template <> inline float sin(float x) { return arm_sin_f32(x); }

} // namespace gdut::dsp

#endif // BASE_MATH_HPP
