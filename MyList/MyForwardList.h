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

	



	FListNodeBase* mNext = nullptr;

};


class FListNode : public FListNodeBase
{
public:
	FListNode();
	~FListNode();

private:

};





JSTD_END

#endif // !FLIST
