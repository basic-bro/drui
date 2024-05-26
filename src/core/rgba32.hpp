// rgba32.hpp

#ifndef RGBA32_HPP_INCLUDED
#define RGBA32_HPP_INCLUDED

#include<cstdint>

struct rgba32
{
  uint8_t r = 0, g = 0, b = 0, a = 0;

  bool operator==( rgba32 right ) const
  {
    return r == right.r && g == right.g && b == right.b && a == right.a;
  }
};

constexpr rgba32 White{ 255, 255, 255, 255 };
constexpr rgba32 Red{ 255, 0, 0, 255 };
constexpr rgba32 Green{ 0, 255, 0, 255 };
constexpr rgba32 Blue{ 0, 0, 255, 255 };


#endif