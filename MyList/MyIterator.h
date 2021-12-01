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

	using traits = STD iterator_traits<Iterator>;

public:

	using iterator_type = Iterator;
	using iterator_category = typename traits::iterator_category;
	using value_type = typename traits::value_type;
	using different_type = typename traits::difference_type;
	using reference = typename traits::reference;
	using pointer = typename traits::pointer;

	constexpr NormalIterator() noexcept
		: mCurrent(Iterator()) 
	{ }

	explicit constexpr NormalIterator(const Iterator& i) noexcept
		: mCurrent(i)
	{ }

	template <typename Iter>
	constexpr NormalIterator(const NormalIterator<Iter, typename STD enable_if_t<>>);


	


};

JSTD_END

#endif // !ITERATOR
