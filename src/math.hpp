#ifndef MATH_HPP
#define MATH_HPP

#include <limits>

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

class fastRNG
{
public:
  float operator()()
  {
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return float(z) / float(std::numeric_limits<unsigned long>::max());
  }
private:
  unsigned long x = 123456789, y = 362436069, z = 521288629;
};

static fastRNG rng;

#endif
