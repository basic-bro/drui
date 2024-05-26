// Key.cpp

#include "Key.hpp"

Key NewKey()
{
	static Key key = 1;
	return key++;
}