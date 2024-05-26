// Key.hpp

#ifndef KEY_HPP_INCLUDED
#define KEY_HPP_INCLUDED

#include<cstdint>

using Key = uint64_t;

constexpr Key NullKey = 0;

Key NewKey();

#endif