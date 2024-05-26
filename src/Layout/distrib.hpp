// distrib.h

#ifndef DISTRIB_H_INCLUDED
#define DISTRIB_H_INCLUDED

#include<vector>

// distribute 'f' items (evenly from the middle going outwards)
// into 'n' awaiting buckets
std::vector<int> distrib_middle( int n, int f );

#endif