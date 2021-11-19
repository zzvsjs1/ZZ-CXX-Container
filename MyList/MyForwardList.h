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

		auto* newTail = mNext;
		while (auto* nextNext = newTail->mNext)
		{

		}
	}

	FListNodeBase* mNext = nullptr;

};


template <typename T>
class FListNode : public FListNodeBase
{

public:

	FListNode() = default;

	template <typename... Args>
	FListNode(Args&&... args)
		: actualData(STD forward<Args>(args)...) { }

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

	Self& operator++() noexcept
	{
		mNode = mNode->mNext;
		return *this;
	}

	Self& operator++(int) noexcept
	{
		Self temp = *this;
		mNode = mNode->mNext;
		return *this;
	}

	friend bool operator==(const Self& left, const Self& right) noexcept
	{
		return left.mNode == right.mNode;
	}
	
	friend bool operator!=(const Self& left, const Self& right) noexcept
	{
		return left.mNode != right.mNode;
	}

	Self next() const noexcept
	{
		return mNode ? Self(mNode->mNext) : Self(nullptr);
	}

	FListNodeBase* mNode;

};

template <typename T>
class FListConstIterator
{
	using Self = FListConstIterator<T>;
	using Node = const FListNode<T>;
	using iterator = FListIterator<T>;

	using value_type = T;
	using pointer = const T*;
	using reference = const T&;
	using difference_type = STD ptrdiff_t;
	using iterator_category = STD forward_iterator_tag;

	FListConstIterator() noexcept
		: mNode() { }

	explicit FListConstIterator(const FListNodeBase* node) noexcept
		: mNode(node) { }

	FListConstIterator(const iterator& iter) noexcept
		: mNode(iter.mNode) { }

	reference operator*() const noexcept
	{
		return static_cast<Node*>(mNode)->getValRef();
	}

	pointer operator->() const noexcept
	{
		return static_cast<Node*>(mNode)->getValPtr();
	}

	Self& operator++() noexcept
	{
		mNode = mNode->mNext;
		return *this;
	}

	Self& operator++(int) noexcept
	{
		Self temp = *this;
		mNode = mNode->mNext;
		return *this;
	}

	friend bool operator==(const Self& left, const Self& right) noexcept
	{
		return left.mNode == right.mNode;
	}

	friend bool operator!=(const Self& left, const Self& right) noexcept
	{
		return left.mNode != right.mNode;
	}

	Self next() const noexcept
	{
		return mNode ? Self(mNode->mNext) : Self(nullptr);
	}

	FListNodeBase* mNode;
};

template <typename T, typename Alloc>
class FListBase
{

protected:

	using T_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<T>::other;
	using T_Alloc_Traits = MyAlloctTraits<Alloc>;
	using Node_Alloc_Type = typename T_Alloc_Traits:: template rebind<ListNode<T>>::other;
	using Node_Alloc_Traits = MyAlloctTraits<Node_Alloc_Type>;

	struct FListImpl : public Node_Alloc_Type
	{
		FListNodeBase* mHead;

		FListImpl() noexcept(STD is_nothrow_default_constructible<Node_Alloc_Type>::value)
			: Node_Alloc_Type(), mHead()
		{ }

		FListImpl(FListImpl&&) = default;

		FListImpl(FListImpl && other, Node_Alloc_Type && alloc)
			: Node_Alloc_Type(std::move(alloc)), mHead(std::move(other.mHead))
		{ }

		FListImpl(Node_Alloc_Type && alloc)
			: Node_Alloc_Type(std::move(alloc)), mHead()
		{ }
	};

public:

	using iterator = FListIterator<T>;
	using const_iterator = FListConstIterator<T>;
	using Node = FListNode<T>;

	FListImpl mImpl;

	FListBase() = default;

	FListBase(const Node_Alloc_Type&& other)
		: mImpl(STD move(other)) { }

	FListBase(FListBase&& other, Node_Alloc_Type&& allocOther, STD true_type)
		: mImpl(STD move(other.mImpl), STD move(allocOther)) { }

	FListBase(FListBase&& other, Node_Alloc_Type&& otherAlloc)
		: mImpl(STD move(otherAlloc))
	{
		if (getNodeAllocator() == other.getNodeAllocator())
		{
			mImpl.mHead = STD move(other.mImpl.mHead);
		}
	}

	FListBase(FListBase&& other) = default;

	~FListBase() noexcept
	{
		clearAll();
	}


	Node_Alloc_Type& getNodeAllocator() noexcept
	{
		return mImpl;
	}

	const Node_Alloc_Type& getNodeAllocator() const noexcept
	{
		return mImpl;
	}

protected:

	Node* allocateNode()
	{
		return Node_Alloc_Traits::allocate(mImpl, 1);
	}

	template <typename... Args>
	Node* createNode(Args&&... args)
	{
		Node* newNode = allocateNode();
		AllocatedPtrGuard guard{ mImpl, newNode };
		Node_Alloc_Traits::construct(mImpl, newNode, STD forward<Args>(args)...);
		return guard.release();
	}

	void clearAll() noexcept
	{
		while (FListNodeBase* next = mImpl.mHead->mNext)
		{
			Node* node = static_cast<Node*>(next);
			Node_Alloc_Traits::destroy(mImpl, node);
			Node_Alloc_Traits::deallocate(mImpl, node, 1);
		}
	}


};

template <typename T, typename Alloc = STD allocator<T>>
class FList : protected FListBase<T, Alloc>
{
	static_assert(STD is_same_v<STD remove_cv_t<T>, T>, "jstd::forward_list must have a non-const, non-volatile value_type");

protected:

	using Base = FListBase<T, Alloc>;
	using NodeBase = FListNodeBase;
	using Node = typename Base::Node;
	using T_Alloc_Type = typename Base::T_Alloc_Type;
	using T_Alloc_Traits = typename Base::T_Alloc_Traits;
	using Node_Alloc_Type = typename Base::Node_Alloc_Type;
	using Node_Alloc_Traits = typename Base::Node_Alloc_Traits;

public:

	using value_type = T;
	using allocator_type = Alloc;
	using size_type = typename T_Alloc_Traits::size_type;
	using difference_type = typename T_Alloc_Traits::difference_type;
	using pointer = typename T_Alloc_Traits::pointer;
	using const_pointer = typename T_Alloc_Traits::const_pointer;
	using reference = T&;
	using const_reference = const T&;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;

	FList() = default;

	explicit FList(const Alloc& alloc) noexcept
		: Base(Node_Alloc_Type(alloc))
	{ }



};

JSTD_END

#endif // !FLIST
