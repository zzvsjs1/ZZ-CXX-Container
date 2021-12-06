#pragma once
#ifndef ITERATOR
#define ITERATOR

#include <iterator>
#include <type_traits>

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
	using difference_type = typename traits::difference_type;
	using reference = typename traits::reference;
	using pointer = typename traits::pointer;

	constexpr NormalIterator() noexcept
		: mCurrent(Iterator()) 
	{ }

	explicit constexpr NormalIterator(const Iterator& i) noexcept
		: mCurrent(i)
	{ }

	template <typename Iter>
	constexpr NormalIterator(const NormalIterator<Iter, 
		STD enable_if_t<STD is_same_v<Iter, typename Container::pointer>, Container>>& i) noexcept
		: mCurrent(i.base())
	{ }

	constexpr reference operator*() const noexcept
	{
		return *mCurrent;
	}

	constexpr pointer operator->() const noexcept
	{
		return mCurrent;
	}

	constexpr NormalIterator& operator++() noexcept
	{
		++mCurrent;
		return *this;
	}

	constexpr NormalIterator operator++(int) noexcept
	{
		return NormalIterator(mCurrent++);
	}
	
	constexpr NormalIterator& operator--() noexcept
	{
		--mCurrent;
		return *this;
	}

	constexpr NormalIterator operator--(int) noexcept
	{
		return NormalIterator(mCurrent--);
	}

	constexpr reference operator[](difference_type n) const noexcept
	{
		return mCurrent[n];
	}

	constexpr NormalIterator& operator+=(difference_type n) noexcept
	{
		mCurrent += n;
		return *this;
	}

	constexpr NormalIterator operator+(difference_type n) const noexcept
	{
		return NormalIterator(mCurrent + n);
	}

	constexpr NormalIterator& operator-=(difference_type n) noexcept
	{
		mCurrent -= n;
		return *this;
	}

	constexpr NormalIterator operator-(difference_type n) const noexcept
	{
		return NormalIterator(mCurrent - n);
	}

	constexpr const Iterator& base() const noexcept
	{
		return mCurrent;
	}
};

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator==(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() == rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator==(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() == rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator!=(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() != rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator!=(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() != rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator<(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() < rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator<(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() < rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator>(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() > rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator>(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() > rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator<=(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() <= rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator<=(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() <= rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline bool operator>=(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs) noexcept
{
	return lhs.base() >= rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline bool operator>=(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() >= rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
constexpr inline auto operator-(const NormalIterator<IteratorL, Container>& lhs, const NormalIterator<IteratorR, Container>& rhs)
	noexcept -> decltype(lhs.base() - rhs.base())
{
	return lhs.base() - rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline typename NormalIterator<Iterator, Container>::difference_type
operator-(const NormalIterator<Iterator, Container>& lhs, const NormalIterator<Iterator, Container>& rhs) noexcept
{
	return lhs.base() - rhs.base();
}

template <typename Iterator, typename Container>
constexpr inline NormalIterator<Iterator, Container>
operator+(typename  NormalIterator<Iterator, Container>::difference_type n, const NormalIterator<Iterator, Container>& itr) noexcept
{
	return NormalIterator<Iterator, Container>(itr.base() + n);
}

template <typename Iterator, typename Container>
constexpr inline Iterator
getBaseFromIter(NormalIterator<Iterator, Container>& itr) noexcept(STD is_nothrow_copy_constructible_v<Iterator>)
{
	return itr.base();
}

JSTD_END

#endif // !ITERATOR
