#ifndef BASE_MATH_HPP
#define BASE_MATH_HPP

#include <arm_math.h>
#include <cmath>
#include <type_traits>

namespace gdut::dsp {

struct use_radian_t {};
inline constexpr use_radian_t use_radian{};

struct use_angle_t {};
inline constexpr use_angle_t use_angle{};

template <typename T> struct is_use_angle_type : std::false_type {};

template <> struct is_use_angle_type<use_angle_t> : std::true_type {};

template <> struct is_use_angle_type<use_radian_t> : std::true_type {};

template <typename T>
inline constexpr bool is_use_angle_type_v = is_use_angle_type<T>::value;

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline T cos(T x) {
  return std::cos(x);
}

inline float cos(float x) { return arm_cos_f32(x); }

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline T sin(T x) {
  return std::sin(x);
}
inline float sin(float x) { return arm_sin_f32(x); }

} // namespace gdut::dsp

#endif // BASE_MATH_HPP
