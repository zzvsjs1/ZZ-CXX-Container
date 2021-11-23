#pragma once
#ifndef ITERATOR
#define ITERATOR

#include <iterator>

#include "Healper.h"

JSTD_START

template <typename Iterator, typename Container>
class NormalIterator
{
protected:

	Iterator mCurrent;

	using traits = STD iterator_traits<Iterator>

public:

	using iterator_type = Iterator;
	using iterator_category = typename traits::iterator_category;
	using value_type = typename traits::value_type;


	constexpr NormalIterator() noexcept
		: mCurrent(Iterator()) 
	{ }


	~NormalIterator() = default;


};

JSTD_END

#endif // !ITERATOR
