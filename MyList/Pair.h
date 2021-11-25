#pragma once
#ifndef PAIR
#define PAIR

#include "Config.h"

JSTD_START

template <typename T1, typename T2>
class Pair
{
public:

	~Pair();

private:

	T1 first;
	T2 second;

};



JSTD_END

#endif // !PAIR
