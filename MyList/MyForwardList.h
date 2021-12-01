#pragma once
#ifndef F_LIST
#define F_LIST

#include <algorithm>
#include <memory>
#include <iterator>
#include <type_traits>
#include <initializer_list>
#include <string>

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
	FListNodeBase* linkFistPlusOneToLastAfterThis(FListNodeBase* const first, FListNodeBase* const last) noexcept
	{
		auto* const newNext = first->mNext;
		// If last != nullptr.
		if (last)
		{
			first->mNext = last->mNext;
			last->mNext = mNext;
		}
		// If last == nullptr.
		else
		{
			first->mNext = nullptr;
		}

		mNext = newNext;
		return last;
	}

	void reverseAfter() noexcept
	{
		if (!mNext)
		{
			return;
		}

		auto* const newTail = mNext;
		while (auto* nextNext = newTail->mNext)
		{
			auto* const temp = mNext;
			mNext = nextNext;
			newTail->mNext = nextNext->mNext;
			mNext->mNext = temp;
		}
	}

	FListNodeBase* mNext = nullptr;

};

struct DataInitTagT { };

template <typename T>
class FListNode : public FListNodeBase
{
public:

	FListNode() = default;

	template <typename... Args>
	FListNode(DataInitTagT, Args&&... args)
		: actualData(STD forward<Args>(args)...) { }

	~FListNode() = default;

	NODISCARD T* getValPtr() noexcept
	{
		return STD addressof(actualData);
	}

	NODISCARD const T* getValPtr() const noexcept
	{
		return STD addressof(actualData);
	}

	NODISCARD T& getValRef() noexcept
	{
		return actualData;
	}

	NODISCARD const T& getValRef() const noexcept
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

	NODISCARD iterator constCast() const noexcept
	{
		return *this;
	}

	NODISCARD FListNodeBase* constCastPtr() const noexcept
	{
		return mNode;
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

	NODISCARD Self getNextNodeIter() const noexcept
	{
		return mNode ? Self(mNode->mNext) : Self(nullptr);
	}

	FListNodeBase* mNode;

};

template <typename T>
class FListConstIterator
{
private:

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

	NODISCARD iterator constCast() const noexcept
	{
		return iterator(const_cast<FListNodeBase*>(mNode));
	}

	NODISCARD FListNodeBase* constCastPtr() const noexcept
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

	Self getNext() const noexcept
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

	void destroyNode(FListNodeBase* node) noexcept
	{
		Node* originNode = static_cast<Node*>(node);
		Node_Alloc_Traits::destroy(mImpl, originNode);
		deallocateNode(originNode);
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
		for (auto* next = &mImpl.mHead; first != last; ++first, next = next->mNext)
		{
			next->mNext = createNode(*first);
		}
	}

	void constructNCopy(size_type n, const value_type& value)
	{
		for (auto* next = &mImpl.mHead; n; --n)
		{
			next->mNext = createNode(value);
		}
	}

	void defaultConstructN(size_type n)
	{
		for (auto* next = &mImpl.mHead; n; --n)
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
		: Base(Node_Alloc_Type(alloc))
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
		insert_after(cbefore_begin(), makeMoveIfNoexceptIterator(other.begin()), makeMoveIfNoexceptIterator(other.end()));
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

private:

	void moveAndAssign(FList&& other) noexcept
	{
		clear();
		mImpl.mHead.mNext = other.mImpl.mHead.mNext;
		other.mImpl.mHead.mNext = nullptr;
		Node_Alloc_Traits::doMove(this->getNodeAllocator(), other.getNodeAllocator());
	}

public:

	FList& operator=(FList&& other) noexcept(Node_Alloc_Traits::nothrow_move())
	{
		if (this != STD addressof(other))
		{
			if constexpr (Node_Alloc_Traits::nothrow_move())
			{
				moveAndAssign(STD move(other));
			}
			else
			{
				if (this->getNodeAllocator() == other.getNodeAllocator())
				{
					moveAndAssign(STD move(other));
				}
				else
				{
					assign(STD make_move_iterator(other.begin()), STD make_move_iterator(other.end()));
				}
			}
		}

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
		auto prev = before_begin();
		auto start = begin();
		auto endi = end();
		for (; start != endi && count; --count, ++start, ++prev)
		{
			*start = value;
		}

		if (count)
		{
			insert_after(start, count, value);
		}
		else if (start != endi)
		{
			erase_after(prev, endi);
		}
	}

	void doAssign(size_type count, const value_type& value, STD false_type)
	{
		clear();
		insert_after(cbefore_begin(), count, value);
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void doAssign(InputIt first, InputIt last, STD true_type)
	{
		auto prev = before_begin();
		auto start = begin();
		auto endi = end();
		for (; first != last && start != endi; ++first, ++start, ++prev)
		{
			*start = *first;
		}

		if (first != last)
		{
			insert_after(prev, first, last);
		}
		else if (start != endi)
		{
			erase_after(prev, endi);
		}
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	void doAssign(InputIt first, InputIt last, STD false_type)
	{
		clear();
		insert_after(cbefore_begin(), first, last);
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

	NODISCARD allocator_type get_allocator() const noexcept
	{
		return allocator_type();
	}

	reference front()
	{
		return static_cast<Node*>(mImpl.mHead.mNext)->getValRef();
	}

	NODISCARD const_reference front() const
	{
		return static_cast<Node*>(mImpl.mHead.mNext)->getValRef();
	}

	iterator before_begin() noexcept
	{
		return iterator(&mImpl.mHead);
	}

	NODISCARD const_iterator before_begin() const noexcept
	{
		return const_iterator(&mImpl.mHead);
	}

	NODISCARD const_iterator cbefore_begin() const noexcept
	{
		return const_iterator(&mImpl.mHead);
	}

	iterator begin() noexcept
	{
		return iterator(mImpl.mHead.mNext);
	}

	NODISCARD const_iterator begin() const noexcept
	{
		return const_iterator(mImpl.mHead.mNext);
	}

	NODISCARD const_iterator cbegin() const noexcept
	{
		return const_iterator(mImpl.mHead.mNext);
	}

	iterator end() noexcept
	{
		return iterator(nullptr);
	}

	NODISCARD const_iterator end() const noexcept
	{
		return iterator(nullptr);
	}

	NODISCARD const_iterator cend() const noexcept
	{
		return iterator(nullptr);
	}

	NODISCARD bool empty() const noexcept
	{
		return !mImpl.mHead.mNext;
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
		auto* const node = pos.constCastPtr();
		auto* const newOne = createNode(STD forward<Args>(args)...);
		newOne->mNext = node->mNext;
		node->mNext = newOne;
		return iterator(newOne);
	}

	iterator spliceAfterPost(const_iterator pos, const_iterator first, const_iterator last) noexcept
	{
		FListNodeBase* position = pos.constCastPtr();
		FListNodeBase* start = first.constCastPtr();
		FListNodeBase* end = start;

		// Get the new end before the last.
		while (end && end->mNext != last.mNode)
		{
			end = end->mNext;
		}

		return start != end 
			? iterator(position->linkFistPlusOneToLastAfterThis(start, end)) 
			: iterator(position);
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
			return iterator(pos.constCastPtr());
		}

		FList temp(count, value, get_allocator());
		return spliceAfterPost(pos, temp.before_begin(), temp.end());
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	iterator insert_after(const_iterator pos, InputIt first, InputIt last)
	{
		if (first == last)
		{
			return iterator(const_cast<NodeBase*>(pos.mNode));
		}

		FList temp(first, last, get_allocator());
		if (temp.empty())
		{
			return iterator(pos.constCast());
		}

		return spliceAfterPost(pos, temp.before_begin(), temp.end());
	}

	iterator insert_after(const_iterator pos, STD initializer_list<T> ilist)
	{
		return insert_after(pos, ilist.begin(), ilist.end());
	}

	template <typename... Args>
	iterator emplace_after(const_iterator pos, Args&&... args)
	{
		return static_cast<Node*>(insertAfterN(pos, STD forward<Args>(args)...).mNode).getValRef();
	}

	iterator erase_after(const_iterator pos)
	{
		auto* const cur = pos.constCastPtr();
		auto* const toErase = cur->mNext;
		cur->mNext = toErase->mNext;
		this->destroyNode(toErase);
		return iterator(cur->mNext);
	}

	iterator erase_after(const_iterator first, const_iterator last)
	{
		auto* cur = first.constCastPtr()->mNext;
		auto* const lastPtr = last.constCastPtr();
		while (cur != lastPtr)
		{
			auto* const toDelete = cur;
			cur = cur->mNext;
			this->destroyNode(toDelete);
		}

		first.constCastPtr()->mNext = lastPtr;
		return iterator(lastPtr);
	}

	void push_front(const value_type& value)
	{
		insertAfter(cbefore_begin(), value);
	}

	void push_front(value_type&& value)
	{
		insertAfter(cbefore_begin(), STD move(value));
	}

	template <typename... Args>
	reference emplace_front(Args&&... args)
	{
		return static_cast<Node*>(insertAfterN(before_begin(), STD forward<Args>(args)...).mNode)->getValRef();
	}

	void pop_front() noexcept
	{
		erase_after(before_begin());
	}

private:

	template <typename... Args>
	void insertAfterN(const_iterator pos, size_type n, Args&&... args)
	{
		auto backUp = pos;
		TRY_START
		for (; n; --n)
		{
			pos = emplace_after(pos, STD forward<Args>(args)...);
		}
		CATCH_ALL
		erase_after(backUp, ++pos);
		END_CATCH
	}

public:

	void resize(size_type count)
	{
		size_type length = 0;
		auto start = before_begin();
		const auto last = end();

		for (; start.getNextNodeIter() != last && length < count; ++start, ++length) { }

		if (length == count)
		{
			erase_after(start, last);
			return;
		}

		// If length < count.
		insertAfterN(start, count - length);
	}

	void resize(size_type count, const value_type& value)
	{
		size_type length = 0;
		auto start = before_begin();
		const auto last = end();

		for (; start.getNextNodeIter() != last && length < count; ++start, ++length) {}

		if (length == count)
		{
			erase_after(start, last);
			return;
		}

		// If length < count.
		insertAfterN(start, count - length, value);
	}

	void swap(FList& other) noexcept(Node_Alloc_Traits::always_equal_v())
	{
		STD swap(mImpl.mHead.mNext, other.mImpl.mHead.mNext);
		Node_Alloc_Traits::doSwap(this->getNodeAllocator(), other.getNodeAllocator());
	}

	void merge(FList& other)
	{
		merge(STD move(other));
	}

	void merge(FList&& other)
	{
		merge(STD move(other), STD less<value_type>{});
	}

	template <typename Compare>
	void merge(FList& other, Compare comp)
	{
		merge(STD move(other), comp);
	}

	template <typename Compare>
	void merge(FList&& other, Compare comp)
	{
		if (this == STD addressof(other))
		{
			return;
		}

		auto* finalWillBeTail = &mImpl.mHead;
		for (; finalWillBeTail->mNext && other.mImpl.mHead.mNext; finalWillBeTail = finalWillBeTail->mNext)
		{
			// default will be other < this.
			if (!comp(static_cast<Node*>(finalWillBeTail->mNext)->getValRef(),
					static_cast<Node*>(other.mImpl.mHead.mNext)->getValRef()))
			{
				finalWillBeTail->linkFistPlusOneToLastAfterThis(&other.mImpl.mHead, other.mImpl.mHead.mNext);
			}
		}

		if (!other.empty())
		{
			finalWillBeTail->linkFistPlusOneToLastAfterThis(&other.mImpl.mHead, nullptr);
		}
	}

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

	void splice_after(const_iterator pos, FList&&, const_iterator it) noexcept
	{
		auto before = it;
		++it;

		if (pos == before || pos == it)
		{
			return;
		}

		auto* const temp = pos.constCastPtr();
		temp->linkFistPlusOneToLastAfterThis(before.constCastPtr(), it.constCastPtr());
	}

	void splice_after(const_iterator pos, FList& other, const_iterator first, const_iterator last) noexcept
	{
		splice_after(pos, STD move(other), first, last);
	}

	void splice_after(const_iterator pos, FList&&, const_iterator first, const_iterator last) noexcept
	{
		spliceAfterPost(pos, first, last);
	}

	size_type remove(const value_type& value)
	{
		return remove_if([&](const auto& e) { return e == value; });
	}

	template <typename UnaryPredicate>
	size_type remove_if(UnaryPredicate p)
	{
		size_type removed = 0;
		FList toDestroy(get_allocator());
		iterator prevIt = before_begin();
		while (auto* temp = static_cast<Node*>(prevIt.mNode->mNext))
		{
			if (p(temp->getValRef()))
			{
				toDestroy.splice_after(toDestroy.cbefore_begin(), *this, prevIt);
				++removed;
				continue;
			}

			++prevIt;
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
		if (empty())
		{
			return 0;
		}

		size_type removed = 0;
		FList toDestroy(get_allocator());
		auto first = begin();
		auto last = end();

		for (auto next = first; ++next != last; next = first)
		{

			if (p(*first, *next))
			{
				toDestroy.splice_after(toDestroy.cbefore_begin(), *this, first);
				++removed;
			}
			else
			{
				first = next;
			}
		}

		return removed;
	}

private:

	/*
	 * Copy from GCC 11.
	 */
	template <typename Compare>
	void gccSort(Compare& comp)
	{
		// If `next' is nullptr, return immediately.
		Node* list = static_cast<Node*>(this->mImpl.mHead.mNext);
		if (!list)
		{
			return;
		}

		unsigned long inSize = 1;

		while (true)
		{
			Node* p = list;
			list = nullptr;
			Node* tail = nullptr;

			// Count number of merges we do in this pass.
			unsigned long nMerges = 0;

			while (p)
			{
				++nMerges;
				// There exists a merge to be done.
				// Step `insize' places along from p.
				Node* q = p;
				unsigned long psize = 0;
				for (unsigned long i = 0; i < inSize; ++i)
				{
					++psize;
					q = static_cast<Node*>(q->mNext);
					if (!q)
					{
						break;
					}
				}

				// If q hasn't fallen off end, we have two lists to merge.
				unsigned long qSize = inSize;

				// Now we have two lists; merge them.
				while (psize > 0 || (qSize > 0 && q))
				{
					// Decide whether next node of merge comes from p or q.
					Node* e;
					if (psize == 0)
					{
						// p is empty; e must come from q.
						e = q;
						q = static_cast<Node*>(q->mNext);
						--qSize;
					}
					else if (qSize == 0 || !q)
					{
						// q is empty; e must come from p.
						e = p;
						p = static_cast<Node*>(p->mNext);
						--psize;
					}
					else if (!comp(q->getValRef(), p->getValRef()))
					{
						// First node of q is not lower; e must come from p.
						e = p;
						p = static_cast<Node*>(p->mNext);
						--psize;
					}
					else
					{
						// First node of q is lower; e must come from q.
						e = q;
						q = static_cast<Node*>(q->mNext);
						--qSize;
					}

					// Add the next node to the merged list.
					if (tail)
					{
						tail->mNext = e;
					}
					else
					{
						list = e;
					}

					tail = e;
				}

				// Now p has stepped `insize' places along, and q has too.
				p = q;
			}

			tail->mNext = nullptr;

			// If we have done only one merge, we're finished.
			// Allow for nmerges == 0, the empty list case.
			if (nMerges <= 1)
			{
				this->mImpl.mHead.mNext = list;
				return;
			}

			// Otherwise repeat, merging lists twice the size.
			inSize *= 2;
		}
	}

public:

	void sort()
	{
		sort(STD less<value_type>{});
	}

	template <typename Compare>
	void sort(Compare comp)
	{
		gccSort(comp);
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

template< typename T, typename Alloc, typename U>
typename FList<T, Alloc>::size_type
inline erase(FList<T, Alloc>& c, const U& value)
{
	return c.remove_if([&](auto& element) { return element == value; });
}

template< typename T, typename Alloc, typename Pred>
typename FList<T, Alloc>::size_type 
inline erase_if(FList<T, Alloc>& c, Pred pred)
{
	return c.remove_if(pred);
}

JSTD_END

#endif // !F_LIST
