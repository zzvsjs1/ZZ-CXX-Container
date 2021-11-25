#pragma once
#ifndef F_LIST
#define F_LIST

#include <algorithm>
#include <memory>
#include <iterator>
#include <type_traits>
#include <initializer_list>

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

	FListNodeBase& operator=(FListNodeBase&& other) noexcept
	{
		mNext = other.mNext;
		other.mNext = nullptr;
		return *this;
	}

	~FListNodeBase() = default;

	/*
	* Link (first, last] to this node.
	*/
	FListNodeBase* linkFirstToNext(FListNodeBase* const first, FListNodeBase* const last) noexcept
	{
		auto* const temp = first->mNext;
		// If last
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



		FListNodeBase* __tail = mNext;
		if (!__tail)
			return;
		while (FListNodeBase* __temp = __tail->mNext)
		{
			FListNodeBase* __keep = mNext;
			mNext = __temp;
			__tail->mNext = __temp->mNext;
			mNext->mNext = __keep;
		}
	}

	FListNodeBase* mNext = nullptr;

};

struct DataInitTagT
{
	DataInitTagT() = default;
};

template <typename T>
class FListNode : public FListNodeBase
{

public:

	FListNode() = default;

	template <typename... Args>
	FListNode(DataInitTagT, Args&&... args)
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
	using iterator = Self;

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

	iterator constCast() const noexcept
	{
		return *this;
	}

	FListNodeBase* constCastPtr() const noexcept
	{
		return static_cast<FListNodeBase*>(mNode);
	}

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

public:

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

	iterator constCast() const noexcept
	{
		return iterator(const_cast<FListNodeBase*>(mNode));
	}

	FListNodeBase* constCastPtr() const noexcept
	{
		return const_cast<FListNodeBase*>(mNode);
	}

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

	const FListNodeBase* mNode;

};

template <typename T, typename Alloc>
class FListBase
{
protected:

	using T_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<T>::other;
	using T_Alloc_Traits = MyAlloctTraits<Alloc>;
	using Node_Alloc_Type = typename T_Alloc_Traits:: template rebind<FListNode<T>>::other;
	using Node_Alloc_Traits = MyAlloctTraits<Node_Alloc_Type>;

	struct FListImpl : public Node_Alloc_Type
	{
		FListNodeBase mHead;

		FListImpl() noexcept(STD is_nothrow_default_constructible_v<Node_Alloc_Type>)
			: Node_Alloc_Type(), mHead()
		{ }

		FListImpl(FListImpl&&) = default;

		FListImpl(FListImpl&& other, Node_Alloc_Type&& alloc)
			: Node_Alloc_Type(STD move(alloc)), mHead(STD move(other.mHead))
		{ }

		FListImpl(Node_Alloc_Type&& alloc)
			: Node_Alloc_Type(STD move(alloc)), mHead()
		{ }
	};

	FListImpl mImpl;

public:

	using iterator = FListIterator<T>;
	using const_iterator = FListConstIterator<T>;
	using Node = FListNode<T>;

	FListBase() = default;

	FListBase(Node_Alloc_Type&& other)
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

	NODISCARD Node_Alloc_Type& getNodeAllocator() noexcept
	{
		return mImpl;
	}

	NODISCARD const Node_Alloc_Type& getNodeAllocator() const noexcept
	{
		return mImpl;
	}

protected:

	NODISCARD Node* allocateNode()
	{
		return Node_Alloc_Traits::allocate(mImpl, 1);
	}

	void deallocateNode(Node* node) noexcept
	{
		Node_Alloc_Traits::deallocate(mImpl, node, 1);
	}

	template <typename... Args>
	NODISCARD Node* createNode(Args&&... args)
	{
		Node* newNode = allocateNode();
		AllocatedPtrGuard<Node_Alloc_Type> guard{ mImpl, newNode };
		Node_Alloc_Traits::construct(mImpl, newNode, DataInitTagT{}, STD forward<Args>(args)...);
		return guard.release();
	}

	void clearAll() noexcept
	{
		for (FListNodeBase* next = mImpl.mHead.mNext; next;)
		{
			Node* node = static_cast<Node*>(next);
			next = next->mNext;
			Node_Alloc_Traits::destroy(mImpl, node);
			deallocateNode(node);
		}
	}

	void resetHeader() noexcept
	{
		mImpl.mHead.mNext = nullptr;
	}

	void resetAndClear() noexcept
	{
		clearAll();
		resetHeader();
	}

	void reverseFromHead() noexcept
	{
		mImpl.mHead.reverseAfter();
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
	
	using Base::mImpl;
	using Base::createNode;

public:

	using value_type = T;
	using allocator_type = Alloc;
	using size_type = typename T_Alloc_Traits::size_type;
	using difference_type = typename T_Alloc_Traits::difference_type;
	using pointer = typename T_Alloc_Traits::pointer;
	using const_pointer = typename T_Alloc_Traits::const_pointer;
	using reference = T&;
	using const_reference = const T&;
	using iterator = FListIterator<T>;
	using const_iterator = FListConstIterator<T>;

	FList() = default;

	explicit FList(const Alloc& alloc) noexcept
		: Base(Node_Alloc_Type(alloc))
	{ }

private:

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void rangeConstruct(InputIt first, InputIt last)
	{
		for (FListNodeBase* next = &this->mImpl.mHead; first != last; ++first, next = next->mNext)
		{
			next->mNext = createNode(*first);
		}
	}

	void constructNCopy(size_type n, const value_type& value)
	{
		for (FListNodeBase* next = &this->mImpl.mHead; n; --n)
		{
			next->mNext = createNode(value);
		}
	}

	void defaultConstructN(size_type n)
	{
		for (FListNodeBase* next = &this->mImpl.mHead; n; --n)
		{
			next->mNext = createNode();
		}
	}
	

public:

	FList(size_type count, const value_type& value, const Alloc& alloc = Alloc())
		: Base(Node_Alloc_Type(alloc))
	{
		constructNCopy(count, value);
	}

	explicit FList(size_type count, const Alloc& alloc = Alloc())
		: Base(Node_Alloc_Type(alloc))
	{
		defaultConstructN(count);
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	FList(InputIt first, InputIt last, const Alloc& alloc = Alloc())
	{
		rangeConstruct(first, last);
	}

	FList(const FList& other)
		: Base(Node_Alloc_Traits::select_on_container_copy_construction(other.getNodeAllocator()))
	{
		rangeConstruct(other.begin(), other.end());
	}

	FList(const FList& other, const Alloc& alloc)
		: Base(Node_Alloc_Type(alloc))
	{
		rangeConstruct(other.begin(), other.end());
	}

	FList(FList&& other) = default;

private:

	FList(FList&& other, const Node_Alloc_Type&& alloc, STD true_type) noexcept
		: Base(STD move(other), Node_Alloc_Type(alloc), STD true_type{})
	{ }

	FList(FList&& other, const Node_Alloc_Type&& alloc, STD false_type)
		: Base(STD move(other), Node_Alloc_Type(STD move(alloc)))
	{
		
	}

public:

	FList(FList&& other, const Alloc& alloc) noexcept(Node_Alloc_Traits::always_equal_v())
		: FList(STD move(other), Node_Alloc_Type(alloc), typename Node_Alloc_Traits::is_always_equal{})
	{ }

	FList(STD initializer_list<value_type> init, const Alloc& alloc = Alloc())
		: Base(Node_Alloc_Type(alloc))
	{
		rangeConstruct(init.begin(), init.end());
	}

	~FList() = default;

	FList& operator=(const FList& other)
	{
		if (this != STD addressof(other))
		{
			if constexpr (Node_Alloc_Traits::propagate_on_container_copy_assignment_v())
			{
				auto& thisAlloc = this->getNodeAllocator();
				auto& thatAlloc = other.getNodeAllocator();
				if (Node_Alloc_Traits::always_equal_v() && thisAlloc != thatAlloc)
				{
					clear();
				}

				Node_Alloc_Traits::doCopy(thisAlloc, thatAlloc);
			}

			assign(other.begin(), other.end());
		}

		return *this;
	}

	FList& operator=(FList&& other) noexcept(Node_Alloc_Traits::nothrow_move())
	{

		return *this;
	}

	FList& operator=(STD initializer_list<T> ilist)
	{
		assign(ilist);
		return *this;
	}

private:

	void doAssign(size_type count, const value_type& value, STD true_type)
	{

	}

	void doAssign(size_type count, const value_type& value, STD false_type)
	{
		clear();
		insert_after(cbefore_begin(), count, value);
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void doAssign(InputIt first, InputIt last, STD true_type)
	{

	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void doAssign(InputIt first, InputIt last, STD false_type)
	{

	}

public:

	void assign(size_type count, const value_type& value)
	{
		doAssign(count, value, STD is_copy_assignable<value_type>{});
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void assign(InputIt first, InputIt last)
	{
		doAssign(first, last, STD is_copy_assignable<value_type>{});
	}

	void assign(STD initializer_list<value_type> ilist)
	{
		assign(ilist.begin(), ilist.end());
	}

	allocator_type get_allocator() const noexcept
	{
		return allocator_type();
	}

	reference front()
	{
		return static_cast<Node*>(mImpl.mHead.mNext)->getValRef();
	}

	const_reference front() const
	{
		return static_cast<Node*>(mImpl.mHead.mNext)->getValRef();
	}

	iterator before_begin() noexcept
	{
		return iterator(&mImpl.mHead);
	}

	const_iterator before_begin() const noexcept
	{
		return const_iterator(&mImpl.mHead);
	}

	const_iterator cbefore_begin() const noexcept
	{
		return const_iterator(&mImpl.mHead);
	}

	iterator begin() noexcept
	{
		return iterator(mImpl.mHead.mNext);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(mImpl.mHead.mNext);
	}

	const_iterator cbegin() const noexcept
	{
		return const_iterator(mImpl.mHead.mNext);
	}

	iterator end() noexcept
	{
		return iterator(nullptr);
	}

	const_iterator end() const noexcept
	{
		return iterator(nullptr);
	}

	const_iterator cend() const noexcept
	{
		return iterator(nullptr);
	}

	NODISCARD bool empty() const noexcept
	{
		return mImpl.mHead.mNext;
	}

	NODISCARD size_type max_size() const noexcept
	{
		return Node_Alloc_Traits::max_size(this->getNodeAllocator());
	}

	void clear() noexcept
	{
		this->resetAndClear();
	}

private:

	template <typename... Args>
	iterator insertAfter(const_iterator pos, Args&&... args)
	{
		FListNodeBase* node = const_cast<FListNodeBase*>(pos.mNode);
		Node* newOne = createNode(STD forward<Args>(args)...);
		newOne->mNext = node->mNext;
		node->mNext = newOne;
		return iterator(newOne); // If 
	}


public:

	iterator insert_after(const_iterator pos, const value_type& value)
	{
		return insertAfter(pos, value);
	}

	iterator insert_after(const_iterator pos, value_type&& value)
	{
		return insertAfter(pos, STD move(value));
	}

	iterator insert_after(const_iterator pos, size_type count, const value_type& value)
	{
		if (!count)
		{
			return iterator(const_cast<NodeBase*>(pos.mNode));
		}


	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	iterator insert_after(const_iterator pos, InputIt first, InputIt last)
	{
		if (first == last)
		{
			return iterator(const_cast<NodeBase*>(pos.mNode));
		}


	}

	iterator insert_after(const_iterator pos, STD initializer_list<T> ilist)
	{
		return insert_after(pos, ilist.begin(), ilist.end());
	}

	template <typename... Args>
	iterator emplace_after(const_iterator pos, Args&&... args)
	{
		return static_cast<Node*>(insertAfter(pos, STD forward<Args>(args)...).mNode).getValRef();
	}

	iterator erase_after(const_iterator pos)
	{

	}

	iterator erase_after(const_iterator first, const_iterator last)
	{

	}

	void push_front(const T& value)
	{

	}

	void push_front(T&& value)
	{

	}

	template <typename... Args>
	reference emplace_front(Args&&... args)
	{
		return static_cast<Node*>(insertAfter(before_begin(), STD forward<Args>(args)...).mNode).getValRef();
	}

	void pop_front() noexcept
	{

	}

	void resize(size_type count)
	{
	}

	void resize(size_type count, const value_type& value)
	{

	}

	void swap(FList& other) noexcept(Node_Alloc_Traits::always_equal_v())
	{
		STD swap(mImpl.mHead.mNext, other.mImpl.mHead.mNext);
		Node_Alloc_Traits::doSwap(this->getNodeAllocator(), other.getNodeAllocator());
	}

	void merge(FList& other);

	void merge(FList&& other);

	template <typename Compare>
	void merge(FList& other, Compare comp);

	template <typename Compare>
	void merge(FList&& other, Compare comp);

private:

	iterator spliceAfterPost(const_iterator pos, const_iterator first, const_iterator last) noexcept
	{
		FListNodeBase* position = pos.constCastPtr();
		FListNodeBase* start = first.constCastPtr();
		FListNodeBase* end = start;

		// 
		while (end && end->mNext != last.mNode)
		{
			end = end->mNext;
		}

		return start != end ? iterator(position->linkFirstToNext(start, end)) : iterator(position);
	}

public:

	void splice_after(const_iterator pos, FList& other) noexcept
	{
		splice_after(pos, STD move(other));
	}

	void splice_after(const_iterator pos, FList&& other) noexcept
	{
		if (other.empty())
		{
			return;
		}

		spliceAfterPost(pos, other.before_begin(), other.end());
	}

	void splice_after(const_iterator pos, FList& other, const_iterator it) noexcept
	{
		splice_after(pos, STD move(other), it);
	}

	void splice_after(const_iterator pos, FList&& other, const_iterator it) noexcept
	{
		
	}

	void splice_after(const_iterator pos, FList& other, const_iterator first, const_iterator last) noexcept
	{
		splice_after(pos, STD move(other), first, last);
	}

	void splice_after(const_iterator pos, FList&& other, const_iterator first, const_iterator last) noexcept
	{

	}

	size_type remove(const value_type& value)
	{
		remove_if([&](auto& e) { return e == value; });
	}

	template <typename UnaryPredicate>
	size_type remove_if(UnaryPredicate p)
	{
		size_type removed = 0;
		FList toDestroy(get_allocator());
		iterator prevIt = before_begin();
		while (Node* temp = static_cast<Node*>(prevIt.mNode->mNext))
		{
			if (p(temp->getValRef()))
			{
				toDestroy.splice_after(toDestroy.cbefore_begin(), *this, prevIt);
				++removed;
			}
			else
			{
				++prevIt;
			}
		}

		return removed;
	}

	void reverse() noexcept
	{
		mImpl.mHead.reverseAfter();
	}

	size_type unique()
	{
		return unique(STD equal_to<value_type>{});
	}

	template <typename BinaryPredicate>
	size_type unique(BinaryPredicate p)
	{

	}

	void sort()
	{
		sort(STD less<value_type>{});
	}

	template <typename Compare>
	void sort(Compare comp)
	{
		
	}

};

template <
	typename InputIterator, 
	typename T = typename STD iterator_traits<InputIterator>::value_type,
	typename Alloc = STD allocator<T>,
	typename = RequireInputIter<InputIterator>,
	typename = RequireAllocator<Alloc>
>
	FList(InputIterator, InputIterator, Alloc = Alloc()) -> FList<T, Alloc>;

template <typename T, typename Alloc>
inline bool operator==(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	auto ls = lhs.cbegin();
	auto rs = rhs.begin();
	auto le = lhs.cend();
	auto re = rhs.cend();

	for (; ls != le && rs != re; ++ls, ++rs)
	{
		if (*ls != *rs)
		{
			return false;
		}
	}

	return ls == le && rs == re;
}

template <typename T, typename Alloc>
inline bool operator!=(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
inline bool operator<(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	return STD lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename Alloc>
inline bool operator<=(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template <typename T, typename Alloc>
inline bool operator>(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template <typename T, typename Alloc>
inline bool operator>=(const FList<T, Alloc>& lhs, const FList<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

template <typename T, typename Alloc>
inline void swap(FList<T, Alloc>& lhs, FList<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}

template< typename T, typename Alloc, typename U >
typename FList<T, Alloc>::size_type
inline erase(FList<T, Alloc>& c, const U& value)
{
	return c.remove_if([&](auto& element) { return element == value; });
}

template< typename T, typename Alloc, typename Pred >
typename FList<T, Alloc>::size_type 
inline erase_if(FList<T, Alloc>& c, Pred pred)
{
	return c.remove_if(pred);
}

JSTD_END

#endif // !F_LIST
