#pragma once
#ifndef FLIST
#define FLIST

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <initializer_list>
#include <stdexcept>
#include <vector>
#include <list>

#include "Healper.h"

JSTD_START

class FListNodeBase
{

public:

	FListNodeBase() = default;

	FListNodeBase(FListNodeBase&& other) noexcept
		: mNext(other.mNext)
	{
		other.mNext = nullptr;
	}

	FListNodeBase(const FListNodeBase&) = delete;

	FListNodeBase& operator=(const FListNodeBase&) = delete;

	FListNodeBase& operator=(FListNodeBase&& ohter) noexcept
	{

		return *this;
	}

	~FListNodeBase() = default;

	FListNodeBase* linkFirstToNext(FListNodeBase* const first, FListNodeBase* const last) noexcept
	{
		auto* const temp = first->mNext;
		if (last)
		{
			first->mNext = last->mNext;
			last->mNext = mNext;
		}
		else
		{
			first->mNext = nullptr;
		}

		mNext = temp;
		return last;
	}

	void reverseAfter() noexcept
	{
		if (!mNext)
		{
			return;
		}


	}

	FListNodeBase* mNext = nullptr;

};


template <typename T>
class FListNode : public FListNodeBase
{

public:

	FListNode() = default;

	~FListNode() = default;

	T* getValPtr() noexcept
	{
		return STD addressof(actualData);
	}

	const T* getValPtr() const noexcept
	{
		return STD addressof(actualData);
	}

	T& getValRef() noexcept
	{
		return actualData;
	}

	const T& getValRef() const noexcept
	{
		return actualData;
	}

private:

	alignas(alignof(T)) T actualData;

};

template <typename T>
class FListIterator
{

private:

	using Self = FListIterator<T>;
	using Node = FListNode<T>;

public:

	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using difference_type = STD ptrdiff_t;
	using iterator_category = STD forward_iterator_tag;

	FListIterator()
		: mNode()
	{}

	explicit FListIterator(FListNodeBase* node) noexcept
		: mNode(node)
	{}

	reference operator*() const noexcept
	{
		return static_cast<Node*>(mNode)->getValRef();
	}

	pointer operator->() const noexcept
	{
		return static_cast<Node*>(mNode)->getValPtr();
	}


	Self next() const noexcept
	{
		return mNode? Self(mNode->mNext) : Self(nullptr);
	}

	FListNodeBase* mNode;

};





JSTD_END

#endif // !FLIST
