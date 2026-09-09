#pragma once
#include <algorithm>
#include <glm/glm.hpp>

namespace Axiom{

template <typename T> requires std::is_arithmetic_v<T>
constexpr T Clamp(T value, T min, T max){
  return std::clamp(value, min, max);
}

template <typename T, glm::qualifier Q>
constexpr glm::vec<2, T, Q> Clamp(const glm::vec<2, T, Q>& value, const glm::vec<2, T, Q>& min, const glm::vec<2, T, Q>& max) {
  return glm::clamp(value, min, max);
}

template <typename T> requires std::is_floating_point_v<T>
constexpr T Lerp(T a, T b, T t) {
  return a + t * (b - a);
}

template <typename T, glm::qualifier Q>
constexpr glm::vec<2, T, Q> Lerp(const glm::vec<2, T, Q>& a, const glm::vec<2, T, Q>& b, T t) {
  return glm::mix(a, b, t);
}

template <typename T, glm::qualifier Q>
constexpr glm::vec<3, T, Q> Lerp(const glm::vec<3, T, Q>& a, const glm::vec<3, T, Q>& b, T t) {
  return glm::mix(a, b, t);
}

template <typename T, glm::qualifier Q>
constexpr glm::vec<4, T, Q> Lerp(const glm::vec<4, T, Q>& a, const glm::vec<4, T, Q>& b, T t) {
  return glm::mix(a, b, t);
}

enum class EaseType {
  Linear,
  InQuad, OutQuad, InOutQuad,
  InCubic, OutCubic, InOutCubic,
  InSine, OutSine, InOutSine,
  InExpo, OutExpo, InOutExpo,
  InBounce, OutBounce, InOutBounce
};

namespace Ease {

template <typename T> requires std::is_floating_point_v<T>
constexpr T InQuad(T t) { return t * t; }

template <typename T> requires std::is_floating_point_v<T>
constexpr T OutQuad(T t) { return t * (2 - t); }

template <typename T> requires std::is_floating_point_v<T>
constexpr T InOutQuad(T t) {
  return t < static_cast<T>(0.5)
  ? static_cast<T>(2) * t * t
  : static_cast<T>(-1) + (static_cast<T>(4) - static_cast<T>(2) * t) * t;
}

template <typename T> requires std::is_floating_point_v<T>
constexpr T InCubic(T t) { return t * t * t; }

template <typename T> requires std::is_floating_point_v<T>
constexpr T OutCubic(T t) { T p = t - static_cast<T>(1); return p * p * p + static_cast<T>(1); }

template <typename T> requires std::is_floating_point_v<T>
constexpr T InOutCubic(T t) {
  return t < static_cast<T>(0.5)
  ? static_cast<T>(4) * t * t * t
  : (t - static_cast<T>(1)) * (static_cast<T>(2) * t - static_cast<T>(2)) * (static_cast<T>(2) * t - static_cast<T>(2)) + static_cast<T>(1);
}

template <typename T> requires std::is_floating_point_v<T>
inline T InSine(T t) {
  constexpr T halfPi = static_cast<T>(1.5707963267948966);
  return static_cast<T>(1) - std::cos(t * halfPi);
}

template <typename T> requires std::is_floating_point_v<T>
inline T OutSine(T t) {
  constexpr T halfPi = static_cast<T>(1.5707963267948966);
  return std::sin(t * halfPi);
}

template <typename T> requires std::is_floating_point_v<T>
inline T InOutSine(T t) {
  constexpr T pi = static_cast<T>(3.1415926535897932);
  return static_cast<T>(-0.5) * (std::cos(pi * t) - static_cast<T>(1));
}

template <typename T> requires std::is_floating_point_v<T>
inline T OutBounce(T t) {
  constexpr T n1 = static_cast<T>(7.5625);
  constexpr T d1 = static_cast<T>(2.75);

  if (t < static_cast<T>(1) / d1) {
    return n1 * t * t;
  } else if (t < static_cast<T>(2) / d1) {
    t -= static_cast<T>(1.5) / d1;
    return n1 * t * t + static_cast<T>(0.75);
  } else if (t < static_cast<T>(2.5) / d1) {
    t -= static_cast<T>(2.25) / d1;
    return n1 * t * t + static_cast<T>(0.9375);
  } else {
    t -= static_cast<T>(2.625) / d1;
    return n1 * t * t + static_cast<T>(0.984375);
  }
}

template <typename T> requires std::is_floating_point_v<T>
inline T InBounce(T t) {
  return static_cast<T>(1) - OutBounce(static_cast<T>(1) - t);
}

// Generic ease resolver
template <typename T> requires std::is_floating_point_v<T>
inline T Evaluate(T t, EaseType ease) {
  switch (ease) {
    case EaseType::InQuad:     return InQuad(t);
    case EaseType::OutQuad:    return OutQuad(t);
    case EaseType::InOutQuad:  return InOutQuad(t);
    case EaseType::InCubic:    return InCubic(t);
    case EaseType::OutCubic:   return OutCubic(t);
    case EaseType::InOutCubic: return InOutCubic(t);
    case EaseType::InSine:     return InSine(t);
    case EaseType::OutSine:    return OutSine(t);
    case EaseType::InOutSine:  return InOutSine(t);
    case EaseType::InBounce:   return InBounce(t);
    case EaseType::OutBounce:  return OutBounce(t);
    case EaseType::Linear:
    default:                   return t;
  }
}

}


template <typename T, typename U>
inline U EaseLerp(const U& a, const U& b, T t, EaseType ease) {
  T easedT = Ease::Evaluate(Clamp(t, static_cast<T>(0), static_cast<T>(1)), ease);
  return Lerp(a, b, easedT);
}

}
