// distrib.cpp

#include "distrib.hpp"


static std::vector<int> dist_middle_even( int n, int f )
{
  auto half_n = n / 2;
  std::vector<int> res( n, 0 );

  // distribute an even number of items:
  if( f % 2 == 0 ) {
    
    while( f > 0 ) {

      // enough for everybody?
      if( f >= n ) { // yes!
        for( auto& r : res ) {
          ++r;
          --f;
        }
      }

      // one each to the middle buckets
      else {  
        int half_f = f / 2;
        int idx = half_n - half_f;
        for( int i = idx; i < n - idx; ++i ) {
          res.at( i )++;
          f--;
        }
      }
    }
  }

  // odd number of items:
  else {
    res = dist_middle_even( n, f - 1 );
    if( n == 2 ) {
      res.at( 0 )++;
    }
    else {
      res.at( half_n - 2 )++;
    }
  }
  return res;
}

static std::vector<int> dist_middle_odd( int n, int f )
{
  std::vector<int> res( n, 0 );
  int median_idx = n / 2;
 
  // one at a time
  while( f > 0 ) {

    // enough for everybody?
    if( f >= n ) { // yes!
      for( auto& bucket : res ) {
        ++bucket;
        --f;
      }
      continue;
    }

    // (f is now less than n)
    // f odd:
    if( f % 2 == 1 ) {
      res.at( median_idx )++;
      --f;
      for( int offset = 1; f > 0; ++offset, f -= 2 ) {
        res.at( median_idx - offset )++;
        res.at( median_idx + offset )++;
      }
    }

    // f even
    else {
      auto delta_res = dist_middle_odd( n, f - 1 );
      for( auto i = 0; i < delta_res.size(); ++i ) {
        res.at( i ) += delta_res.at( i );
      }
      f = 1;
      if( n == 1 ) {
        res.at( 0 )++;
      }
      else {
        res.at( median_idx - 1 )++;
      }
      f--;
    }
  }
  return res;
}


// distribute 'f' items (evenly from the middle going outwards)
// into 'n' awaiting buckets
std::vector<int> distrib_middle( int n, int f )
{
  if( n % 2 == 0 ) return dist_middle_even( n, f );
  return dist_middle_odd( n, f );
}
