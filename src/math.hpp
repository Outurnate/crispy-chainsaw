#ifndef MATH_HPP
#define MATH_HPP

template<typename T>
inline T lerp(T a, T b, T alpha)
{
  return alpha * a + ((1 - alpha) * b);
}

template<typename T>
inline T clamp(T a, T minV, T maxV)
{
  return std::min(std::max(minV, a), maxV);
}

#endif
