#pragma once
#ifndef MYLIST
#define MYLIST

#include <algorithm>
#include <cstdlib>
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

/*
*
*
*
*
*
*
*/
class ListNodeBase
{

public:

	ListNodeBase* mPrev;

	ListNodeBase* mNext;

	static void swapNodeBase(ListNodeBase& left, ListNodeBase& right) noexcept
	{
		// If left is not empty.
		if (left.mNext != STD addressof(left))
		{
			// Both are not empty.
			if (right.mNext != STD addressof(right))
			{
				swapWhenBothAreNonEmpty(left, right);
			}
			else
			{
				// left is not empty, right is empty.
				swapWhenOnceIsEmpty(right, left);
			}
		}
		// If right is not empty.
		else if (right.mNext != STD addressof(right))
		{
			swapWhenOnceIsEmpty(left, right);
		}
	}

private:

	static void swapWhenBothAreNonEmpty(ListNodeBase& left, ListNodeBase& right) noexcept
	{
		STD swap(left.mNext, right.mNext);
		STD swap(left.mPrev, right.mPrev);

		auto* const leftAddress = STD addressof(left);

		left.mNext->mPrev = leftAddress;
		left.mPrev->mNext = leftAddress;

		auto* const rightAddress = STD addressof(right);

		right.mNext->mPrev = rightAddress;
		right.mPrev->mNext = rightAddress;
	}

	static void swapWhenOnceIsEmpty(ListNodeBase& empty, ListNodeBase& nonEmpty) noexcept
	{
		empty.mNext = nonEmpty.mNext;
		empty.mPrev = nonEmpty.mPrev;

		auto* const emptyAddress = STD addressof(empty);

		empty.mPrev->mNext = emptyAddress;
		empty.mNext->mPrev = emptyAddress;

		auto* const nonEmptyAddress = STD addressof(nonEmpty);

		nonEmpty.mPrev = nonEmptyAddress;
		nonEmpty.mNext = nonEmptyAddress;
	}

public:

	void linkAnRnageBeforeThisNode(ListNodeBase* const first, ListNodeBase* const last) noexcept
	{
		if (first == last)
		{
			return;
		}

		// Remove [first, last) from its old position.
		first->mPrev->mNext = last;
		last->mPrev->mNext = this;
		mPrev->mNext = first;

		// Splice [first, last) into its new position.
		ListNodeBase* const thisPrev = mPrev;
		mPrev = last->mPrev;
		last->mPrev = first->mPrev;
		first->mPrev = thisPrev;
	}

	void linkBefore(ListNodeBase* const position) noexcept
	{
		mNext = position;
		mPrev = position->mPrev;

		position->mPrev->mNext = this;
		position->mPrev = this;
	}

	void unlinkMyself() noexcept
	{
		mPrev->mNext = mNext;
		mNext->mPrev = mPrev;
	}
};

/*
*
*
*
*
*
*
*
*/
class ListNodeHeader : public ListNodeBase
{

public:

	ListNodeHeader() noexcept
		: ListNodeBase(), mSize()
	{
		resetAllMembers();
	}

	ListNodeHeader(ListNodeHeader&& other) noexcept
		: ListNodeBase{ other.mPrev, other.mNext }, mSize(other.mSize)
	{
		// If other is empty.
		if (STD addressof(other) == other.mNext)
		{
			mPrev = this;
			mNext = this;
		}
		else
		{
			mPrev->mNext = this;
			mNext->mPrev = this;
			other.resetAllMembers();
		}
	}

	void moveNodes(ListNodeHeader&& other)
	{
		// If other is empty.
		if (other.mSize == 0)
		{
			resetAllMembers();
			return;
		}

		mNext = other.mNext;
		mPrev = other.mPrev;

		mNext->mPrev = this;
		mPrev->mNext = this;
		mSize = other.mSize;

		other.resetAllMembers();
	}

	void resetAllMembers() noexcept
	{
		mPrev = this;
		mNext = this;
		mSize = 0;
	}

	void doReverse() noexcept
	{
		STD swap(mNext, mPrev);

		for (ListNodeBase* temp = mPrev; temp != this; temp = temp->mPrev)
		{
			STD swap(temp->mNext, temp->mPrev);
		}
	}

	size_t mSize;
};

// An actual node in the list.
template <typename T>
class ListNode : public ListNodeBase
{

public:

	ListNode() = default;

	template <typename ... Args>
	ListNode(Args&& ... args)
		: ListNodeBase(), actualData(STD forward<Args>(args)...) { }

	~ListNode() = default;

	T* valuePtr()
	{
		return &actualData;
	}

	T const * valuePtr() const
	{
		return &actualData;
	}

	T& valueRef()
	{
		return actualData;
	}

	const T& valueRef() const
	{
		return actualData;
	}

private:

	// The actual data.
	alignas(alignof(T)) T actualData;
};


template <typename T>
class ListIterator
{

public:

	using Node = ListNode<T>;
	using difference_type = STD ptrdiff_t;
	using iterator_category = STD bidirectional_iterator_tag;
	using value_type = T;
	using pointer = T*;
	using reference = T&;

	ListIterator() noexcept
		: mNode()
	{}

	~ListIterator() = default;

	explicit ListIterator(ListNodeBase* node) noexcept
		: mNode(node)
	{}

	ListIterator constCast() const noexcept
	{
		return *this;
	}

	reference operator*() const noexcept
	{
		return static_cast<Node*>(mNode)->valueRef();
	}

	pointer operator->() const noexcept
	{
		return static_cast<Node*>(mNode)->valuePtr();
	}

	ListIterator& operator++() noexcept
	{
		mNode = mNode->mNext;
		return *this;
	}

	ListIterator operator++(int) noexcept
	{
		ListIterator temp = *this;
		mNode = mNode->mNext;
		return temp;
	}

	ListIterator& operator--() noexcept
	{
		mNode = mNode->mPrev;
		return *this;
	}

	ListIterator operator--(int) noexcept
	{
		ListIterator temp = *this;
		mNode = mNode->mPrev;
		return temp;
	}

	friend bool operator==(const ListIterator& left, const ListIterator& right) noexcept
	{
		return left.mNode == right.mNode;
	}

	friend bool operator!=(const ListIterator& left, const ListIterator& right) noexcept
	{
		return left.mNode != right.mNode;
	}

	ListNodeBase* mNode;

};

template <typename T>
class ListConstIterator
{

private:

	using Node = const ListNode<T>;

public:

	using iterator = ListIterator<T>;
	using difference_type = STD ptrdiff_t;
	using iterator_category = STD bidirectional_iterator_tag;
	using value_type = T;
	using pointer = const T*;
	using reference = const T&;

	ListConstIterator() noexcept
		: mNode()
	{}

	explicit ListConstIterator(const ListNodeBase* node) noexcept
		: mNode(node)
	{}

	ListConstIterator(const iterator& other) noexcept
		: mNode(other.mNode)
	{}

	iterator constCast() const noexcept
	{
		return iterator(const_cast<ListNodeBase*>(mNode));
	}

	reference operator*() const noexcept
	{
		return static_cast<Node*>(mNode)->valueRef();
	}

	pointer operator->() const noexcept
	{
		return static_cast<Node*>(mNode)->valuePtr();
	}

	ListConstIterator& operator++() noexcept
	{
		mNode = mNode->mNext;
		return *this;
	}

	ListConstIterator operator++(int) noexcept
	{
		ListConstIterator temp = *this;
		mNode = mNode->mNext;
		return temp;
	}

	ListConstIterator& operator--() noexcept
	{
		mNode = mNode->mPrev;
		return *this;
	}

	ListConstIterator operator--(int) noexcept
	{
		ListConstIterator temp = *this;
		mNode = mNode->mPrev;
		return temp;
	}

	friend bool operator==(const ListConstIterator& left, const ListConstIterator& right) noexcept
	{
		return left.mNode == right.mNode;
	}

	friend bool operator!=(const ListConstIterator& left, const ListConstIterator& right) noexcept
	{
		return left.mNode != right.mNode;
	}

	const ListNodeBase* mNode;

};

// Class template ListBase
template <typename T, typename Alloc>
class ListBase
{

public:

	using T_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<T>::other;
	using T_Alloc_Traits = MyAlloctTraits<Alloc>;
	using Node_Alloc_Type = typename T_Alloc_Traits:: template rebind<ListNode<T>>::other;
	using Node_Alloc_Traits = MyAlloctTraits<Node_Alloc_Type>;

	// The empty base optimization.
	struct ListImpl : public Node_Alloc_Type
	{
		ListNodeHeader mHeader;

		ListImpl() noexcept(STD is_nothrow_default_constructible_v<Node_Alloc_Type>)
			: Node_Alloc_Type(), mHeader()
		{}

		ListImpl(const Node_Alloc_Type& other) noexcept
			: Node_Alloc_Type(other), mHeader()
		{}

		ListImpl(ListImpl&& other) = default;

		ListImpl(Node_Alloc_Type&& otherNode, ListImpl&& otherImpl)
			: Node_Alloc_Type(STD move(otherNode)), mHeader(STD move(otherImpl.mHeader))
		{}

		ListImpl(Node_Alloc_Type&& other) noexcept
			: Node_Alloc_Type(STD move(other)), mHeader()
		{}

	};

	ListImpl mImpl;

	void setSize(const size_t size) noexcept
	{
		mImpl.mHeader.mSize = size;
	}

	void incSize(const size_t n) noexcept
	{
		mImpl.mHeader.mSize += n;
	}

	void decSize(const size_t n) noexcept
	{
		mImpl.mHeader.mSize -= n;
	}

	typename Node_Alloc_Traits::pointer allocateNode()
	{
		// Not good. Similar to return new T; Any good solution?
		return Node_Alloc_Traits::allocate(mImpl, 1);
	}

	void deallocateNode(typename Node_Alloc_Traits::pointer ptr) noexcept
	{
		Node_Alloc_Traits::deallocate(mImpl, ptr, 1);
	}

public:

	using allocator_type = Alloc;

	Node_Alloc_Type& getNodeAllocator() noexcept
	{
		return mImpl;
	}

	const Node_Alloc_Type& getNodeAllocator() const noexcept
	{
		return mImpl;
	}

	// Constructor start from here.
	ListBase() = default;

	ListBase(const Node_Alloc_Type& other) noexcept
		: mImpl(other)
	{}

	ListBase(ListBase&&) = default;

	ListBase(ListBase&& otherBase, Node_Alloc_Type&& otherAllocator)
		: mImpl(STD move(otherAllocator))
	{
		if (getNodeAllocator() == otherBase.getNodeAllocator())
		{
			moveNodes(STD move(otherBase));
		}
	}

	// Used when allocator is_always_equal.
	ListBase(Node_Alloc_Type&& otherAlloc, ListBase&& otherBase)
		: mImpl(STD move(otherAlloc), STD move(otherBase.mImpl))
	{}

	// Used when allocator !is_always_equal.
	ListBase(Node_Alloc_Type&& nodeAlloc)
		: mImpl(STD move(nodeAlloc))
	{}

	void moveNodes(ListBase&& other)
	{
		mImpl.mHeader.moveNodes(STD move(other.mImpl.mHeader));
	}

	~ListBase() noexcept
	{
		// If MyList construcor throw an exception, this destructor will be called.
		clearData();
	}

	void clearDataAndRestHearder() noexcept
	{
		clearData();
		resetHearder();
	}

	void clearData() noexcept
	{
		if (mImpl.mHeader.mSize == 0)
		{
			return;
		}

		using Node = ListNode<T>;
		for (ListNodeBase* next = mImpl.mHeader.mNext; next != &mImpl.mHeader;)
		{
			Node* temp = static_cast<Node*>(next);
			next = next->mNext; // 
			//T* value = temp->valuePtr();
			Node_Alloc_Traits::destroy(getNodeAllocator(), temp);
			deallocateNode(temp);
		}
	}

	void resetHearder() noexcept
	{
		mImpl.mHeader.resetAllMembers();
	}

};

/*
* The List class.
*
*
*/
template <typename T, typename Alloc = STD allocator<T>>
class MyList : protected ListBase<T, Alloc>
{

private:

	// Not allow cv. Following cppreference.
	static_assert(STD is_same_v<typename STD remove_cv_t<T>, T>, "jstd::MyList must have a non-const, non-volatile value_type.");

	using Base = ListBase<T, Alloc>;
	using Node = ListNode<T>;

	using T_Alloc_Type = typename Base::T_Alloc_Type;
	using T_Alloc_Traits = typename Base::T_Alloc_Traits;
	using Node_Alloc_Type = typename Base::Node_Alloc_Type;
	using Node_Alloc_Traits = typename Base::Node_Alloc_Traits;

protected:

	// Short name.
	using Base::mImpl;
	using Base::allocateNode;
	using Base::deallocateNode;
	using Base::getNodeAllocator;

public:

	using value_type = T;
	using allocator_type = Alloc;
	using size_type = typename T_Alloc_Traits::size_type;
	using difference_type = typename T_Alloc_Traits::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename T_Alloc_Traits::pointer;
	using const_pointer = typename T_Alloc_Traits::const_pointer;
	using iterator = ListIterator<T>;
	using const_iterator = ListConstIterator<T>;
	using reverse_iterator = STD reverse_iterator<iterator>;
	using const_reverse_iterator = STD reverse_iterator<const_iterator>;

	// Init the ListBase is enough.
	MyList() = default;

	explicit MyList(const allocator_type& alloc) noexcept
		: Base(Node_Alloc_Type(alloc))
	{}

private:

	template <typename ... Args>
	Node* createNode(Args&& ... args)
	{
		Node* node = allocateNode(); // Throws
		auto& alloc = getNodeAllocator();
		AllocatedPtrGuard<Node_Alloc_Type> guard{ alloc, node };
		Node_Alloc_Traits::construct(alloc, node, STD forward<Args>(args)...); // Throws
		return guard.release();
	}

	void defalutInitializeN(size_type size)
	{
		for (; size; --size)
		{
			emplace_back();
		}
	}

	void initializeValueN(size_type size, const value_type& value)
	{
		for (; size; --size)
		{
			push_back(value);
		}
	}

	template <typename InputIterator>
	void initializeNByInputIterator(InputIterator first, InputIterator last)
	{
		for (; first != last; ++first)
		{
			emplace_back(*first);
		}
	}

	template <typename ... Args>
	void addNodeBefore(iterator position, Args&& ... args)
	{
		Node* temp = createNode(STD forward<Args>(args)...);
		temp->linkBefore(position.mNode);
		this->incSize(1);
	}

public:

	explicit MyList(const size_type size, const allocator_type& alloc = allocator_type())
		: Base(Node_Alloc_Type(alloc))
	{
		defalutInitializeN(size);
	}

	MyList(size_type size, const value_type& value, const allocator_type& alloc = allocator_type())
		: Base(Node_Alloc_Type(alloc))
	{
		initializeValueN(size, value);
	}

	template <typename InputIterator, typename = RequireInputIter<InputIterator>> // true, if the class provide iterator_category. Otherwise is false.
	MyList(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
		:Base(Node_Alloc_Type(alloc))
	{
		initializeNByInputIterator(first, last);
	}

	MyList(const MyList& other)
		: Base(Node_Alloc_Traits::select_on_container_copy_construction(other.getNodeAllocator())) // Folloing the cppreference.
	{
		initializeNByInputIterator(other.begin(), other.end());
	}

	MyList(const MyList& other, const allocator_type& alloc)
		: Base(Node_Alloc_Type(alloc))
	{
		initializeNByInputIterator(other.begin(), other.end());
	}

	MyList(MyList&& other) = default;

private:

	/*
	* If all instances of the allocator type compare are equal.
	* We just do the normal instantiate.
	*
	* This constructor does not throw an exception, because we just move the header node.
	*/
	MyList(MyList&& other, const allocator_type& alloc, STD true_type) noexcept
		: Base(Node_Alloc_Type(alloc), STD move(other))
	{}

	/*
	* If all instances of the allocator type compare are not equal.
	* We need to following the https://en.cppreference.com/w/cpp/container/list/list.
	*
	* If this->getNodeAllocator() != other.getNodeAllocator(), we do the element-wise move.
	* If both are equal, we do the normal instantiate.
	*
	* This constructor may throw an exception, because we do the element-wise move.
	*/
	MyList(MyList&& other, const allocator_type& alloc, STD false_type)
	{
		if (this->getNodeAllocator() == other.getNodeAllocator())
		{
			this->moveNodes(STD move(other));
		}
		// Element-wise move.
		else
		{
			insert(begin(), STD make_move_iterator(begin()), STD make_move_iterator(other.end()));
		}
	}

public:

	/*
	* Allocator-extended move constructor.
	* Using alloc as the allocator for the new container, moving the contents from other;
	* if alloc != other.get_allocator(), this results in an element-wise move.
	*/
	MyList(MyList&& other, const allocator_type& alloc) noexcept(Node_Alloc_Traits::always_equal_v())
		: MyList(STD move(other), alloc, typename Node_Alloc_Traits::is_always_equal{}) // Check whether all instances of the allocator type compare equal.
																						// Note: std::allocator's is_always_equal is eqaul to true.
	{}

	MyList(STD initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
		: Base(Node_Alloc_Type(alloc))
	{
		initializeNByInputIterator(init.begin(), init.end());
	}

	// The base class will release all the resources.
	~MyList() = default;

	MyList& operator=(const MyList& other)
	{
		if (this != STD addressof(other))
		{
			if constexpr (Node_Alloc_Traits::propagate_on_container_copy_assignment_v())
			{
				auto& thisAlloc = this->getNodeAllocator();
				auto& otherAlloc = other.getNodeAllocator();
				if (!Node_Alloc_Traits::always_equal_v() && thisAlloc != otherAlloc)
				{
					clear();
				}

				Node_Alloc_Traits::doCopy(thisAlloc, otherAlloc);
			}

			assign(other.begin(), other.end());
		}

		return *this;
	}

private:

	void doDefaultMove(MyList&& other)
	{
		auto& thisAlloc = this->getNodeAllocator();
		auto& otherAlloc = other.getNodeAllocator();

		clear();
		this->moveNodes(STD move(other));
		Node_Alloc_Traits::doMove(thisAlloc, otherAlloc);
	}

public:

	MyList& operator=(MyList&& other) noexcept(Node_Alloc_Traits::nothrow_move())
	{
		if (this != STD addressof(other))
		{
			if constexpr (Node_Alloc_Traits::nothrow_move())
			{
				doDefaultMove(STD move(other));
			}
			else
			{
				if (getNodeAllocator() == other.getNodeAllocator())
				{
					doDefaultMove(STD move(other));
				}
				else
				{
					assign(STD make_move_iterator(other.begin()), STD make_move_iterator(other.end()));
				}
			}
		}

		return *this;
	}

	MyList& operator=(STD initializer_list<value_type> ilist)
	{
		assign(ilist.begin(), ilist.end());
		return *this;
	}

	void assign(size_type count, const value_type& value)
	{
		auto now = begin();
		for (; now != end(); ++now, --count)
		{
			// Do copy.
			*now = value;
		}

		if (count > 0)
		{
			insert(end(), count, value);
			return;
		}

		earse(now, end());
	}

	template <typename InputIt, typename = STD enable_if_t<is_input_iter_v<InputIt>>>
	void assign(InputIt first, InputIt last)
	{
		auto thisFirst = begin();
		auto thisLast = end();
		for (; first != last && thisFirst != thisLast; ++thisFirst, ++first)
		{
			// Do copy.
			*thisFirst = *first;
		}

		if (first != last)
		{
			insert(thisLast, first, last);
			return;
		}

		erase(thisFirst, thisLast);
	}

	void assign(STD initializer_list<value_type> ilist)
	{
		assign(ilist.begin(), ilist.end());
	}

	[[nodiscard]] allocator_type get_allocator() const noexcept
	{
		return allocator_type(this->getNodeAllocator());
	}

	[[nodiscard]] reference front() noexcept
	{
		return *begin();
	}

	[[nodiscard]] const_reference front() const noexcept
	{
		return *begin();
	}

	[[nodiscard]] reference back() noexcept
	{
		iterator temp = end();
		--temp;
		return *temp;
	}

	[[nodiscard]] const_reference back() const noexcept
	{
		const_iterator temp = end();
		--temp;
		return *temp;
	}

	[[nodiscard]] iterator begin() noexcept
	{
		return iterator(mImpl.mHeader.mNext);
	}

	[[nodiscard]] const_iterator begin() const noexcept
	{
		return const_iterator(mImpl.mHeader.mNext);
	}

	[[nodiscard]] const_iterator cbegin() const noexcept
	{
		return const_iterator(mImpl.mHeader.mNext);
	}

	[[nodiscard]] iterator end() noexcept
	{
		return iterator(STD addressof(mImpl.mHeader));
	}

	[[nodiscard]] const_iterator end() const noexcept
	{
		return const_iterator(STD addressof(mImpl.mHeader));
	}

	[[nodiscard]] const_iterator cend() const noexcept
	{
		return const_iterator(STD addressof(mImpl.mHeader));
	}

	[[nodiscard]] reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	[[nodiscard]] const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	[[nodiscard]] const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	[[nodiscard]] reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	[[nodiscard]] const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	[[nodiscard]] const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return mImpl.mHeader.mSize == 0;
	}

	[[nodiscard]] size_type size() const noexcept
	{
		return mImpl.mHeader.mSize;
	}

	[[nodiscard]] size_type max_size() const noexcept
	{
		return Node_Alloc_Traits::max_size(this->getNodeAllocator());
	}

	void clear() noexcept
	{
		Base::clearDataAndRestHearder();
	}

	iterator insert(const_iterator pos, const value_type& value)
	{
		insert(pos, 1, value);
	}

	iterator insert(const_iterator pos, size_type count, const value_type& value)
	{
		if (!count)
		{
			return pos.constCast();
		}

		MyList temp(count, value, get_allocator());
		iterator beginIt = temp.begin();
		splice(pos, temp);
		return beginIt;
	}

	template <typename InputIt, typename = STD enable_if_t<is_input_iter_v<InputIt>>>
	iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
		if (first == last)
		{
			return pos.constCast();
		}

		MyList temp(first, last, get_allocator());
		iterator inputIterator = temp.begin();
		splice(pos, temp);
		return inputIterator;
	}

	iterator insert(const_iterator pos, STD initializer_list<value_type> ilist)
	{
		insert(pos, ilist.begin(), ilist.end());
	}

	template <typename ... Args>
	iterator emplace(const_iterator pos, Args&&... args)
	{
		Node* temp = createNode(STD forward<Args>(args)...); // Throws
		temp->linkBefore(pos.constCast().mNode);
		this->incSize(1);

		return iterator(temp);
	}

	iterator erase(const_iterator pos) noexcept
	{
		iterator ret = iterator(pos.mNode->mNext);

		Node* needToDelete = static_cast<Node*>(pos.constCast().mNode);
		needToDelete->unlinkMyself();

		Node_Alloc_Traits::destroy(getNodeAllocator(), needToDelete->valuePtr());
		this->deallocateNode(needToDelete);
		this->decSize(1);

		return ret;
	}

	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		for (; first != last; first = erase(first)) {}
		return last.constCast();
	}

	void push_back(const value_type& value)
	{
		addNodeBefore(end(), value);
	}

	void push_back(value_type&& value)
	{
		addNodeBefore(end(), STD move(value));
	}

	// Return a reference to the inserted element. Just call back().
	template <typename ... Args>
	reference emplace_back(Args&& ... args)
	{
		addNodeBefore(end(), STD forward<Args>(args)...);
		return back();
	}

	void pop_back() noexcept
	{
		erase(iterator(mImpl.mHeader.mPrev));
	}

	void push_front(const value_type& value)
	{
		insert(iterator(mImpl.mHeader.mNext), value);
	}

	void push_front(value_type&& value)
	{
		insert(iterator(mImpl.mHeader.mNext), STD move(value));
	}

	template<typename ... Args>
	reference emplace_front(Args&&... args)
	{
		addNodeBefore(begin(), STD forward<Args>(args)...);
		return back();
	}

	void pop_front() noexcept
	{
		erase(iterator(mImpl.mHeader.mNext));
	}

private:

	void defaultAppend(size_type count, const value_type& value)
	{
		size_type i = 0;
		TRY_START	
		const size_type currentSize = size();
		for (; i < count - currentSize; i++)
		{
			emplace_back(value);
		}
		CATCH_ALL

		for (; i; --i)
		{
			pop_back();
		}
		THROW_AGAIN

		END_CATCH
	}

public:

	void resize(size_type count)
	{
		resize(count, value_type());
	}

	void resize(size_type count, const value_type& value)
	{
		if (const auto currentSize = size(); currentSize < count)
		{
			defaultAppend(count, value);
		}
		else if (currentSize > count)
		{
			iterator start;
			if (count < currentSize / 2)
			{
				start = begin();
				STD advance(start, count);
			}
			else
			{
				start = end();
				const difference_type numberOfErase = currentSize - count;
				STD advance(start, -numberOfErase);
			}

			erase(start, end());
		}
	}

	/*
	* If std::allocator_traits<allocator_type>::propagate_on_container_swap::value is true,
	* then the allocators are exchanged using an unqualified call to non-member swap.
	* Otherwise, they are not swapped (and if get_allocator() != other.get_allocator(), the behavior is undefined).
	*
	* noexcept specification:
	* noexcept(std::allocator_traits<Allocator>::is_always_equal::value)
	*/
	void swap(MyList& other) noexcept(Node_Alloc_Traits::always_equal_v())
	{
		if (this == STD addressof(other))
		{
			return;
		}

		// Not necessary.
		if (get_allocator() != other.get_allocator())
		{
			abort();
		}

		ListNodeBase::swapNodeBase(mImpl.mHeader, other.mImpl.mHeader);
		STD swap(mImpl.mHeader.mSize, other.mImpl.mHeader.mSize);

		if constexpr (Node_Alloc_Traits::propagate_on_container_swap::value)
		{
			Node_Alloc_Traits::doSwap(this->getNodeAllocator(), other.getNodeAllocator());
		}
	}

private:

		static void insertedFirstLastBeforePos(iterator pos, iterator first, iterator last) noexcept
		{
			if (first == last)
			{
				return;
			}

			// Inserting [first, last) before pos.
			ListNodeBase* const position = pos.mNode;
			ListNodeBase* const start = first.mNode;
			ListNodeBase* const end = last.mNode;

			// Remove
			start->mPrev->mNext = end;
			end->mPrev->mNext = position;
			position->mPrev->mNext = start;

			// Link
			ListNodeBase* const posPrev = position->mPrev;
			position->mPrev = end->mPrev;
			end->mPrev = start->mPrev;
			start->mPrev = posPrev;
		}

public:

	/*
	* Merges two sorted lists into one. The lists should be sorted into ascending order.
	* No elements are copied. The container other becomes empty after the operation.
	* The function does nothing if other refers to the same object as *this. If get_allocator() != other.get_allocator(),
	* the behavior is undefined. No iterators or references become invalidated,
	* except that the iterators of moved elements now refer into *this, not into other. The default version uses operator < to compare the elements,
	* the second version uses the given comparison function comp.
	* This operation is stable: for equivalent elements in the two lists,
	* the elements from *this shall always precede the elements from other,
	* and the order of equivalent elements of *this and other does not change.
	* 
	* If an exception is thrown, this function has no effect, except if the exception comes from the comparison function.
	*/
	void merge(MyList& other)
	{
		merge(STD move(other), STD less<>{});
	}

	void merge(MyList&& other)
	{
		merge(STD move(other), STD less<>{});
	}

	template <typename Compare>
	void merge(MyList& other, Compare comp)
	{
		merge(STD move(other), comp);
	}

	template <typename Compare>
	void merge(MyList&& other, Compare comp)
	{
		if (this == STD addressof(other))
		{
			return;
		}

		iterator thisFirst = begin();
		iterator thisLast = end();
		iterator otherFirst = other.begin();
		iterator otherLast = other.end();
		size_type oldOtherSize = other.size();

		TRY_START
		while (thisFirst != thisLast && otherFirst != otherLast)
		{
			// If first < otherFirst
			if (comp(*thisFirst, *otherFirst))
			{
				++thisFirst;
				continue;
			}

			iterator next = otherFirst;
			++next;
			insertedFirstLastBeforePos(thisFirst, otherFirst, next);
			otherFirst = next;
		}
	
		if (otherFirst != otherLast)
		{
			insertedFirstLastBeforePos(thisLast, otherFirst, otherLast);
		}

		this->incSize(other.size());
		other.setSize(0);
		CATCH_ALL

		const size_type between = STD distance(otherFirst, otherLast);
		this->incSize(other.size() - between);
		other.setSize(between);
		THROW_AGAIN

		END_CATCH
	}

	/*
	* Transfers all elements from other into *this. The elements are inserted before the element pointed to by pos.
	* The container other becomes empty after the operation. 
	* The behavior is undefined if other refers to the same object as *this.
	*/
	void splice(const_iterator pos, MyList& other) noexcept
	{
		splice(pos, STD move(other));
	}

	void splice(const_iterator pos, MyList&& other) noexcept
	{
		if (other.empty())
		{
			return;
		}

		insertedFirstLastBeforePos(pos.constCast(), other.begin(), other.end());
		this->incSize(other.size());
		other.setSize(0);
	}

	// Transfers the element pointed to by it from other into *this. The element is inserted before the element pointed to by pos.
	void splice(const_iterator pos, MyList& other, const_iterator it) noexcept
	{
		splice(pos, STD move(other), it);
	}

	void splice(const_iterator pos, MyList&& other, const_iterator it) noexcept
	{
		iterator otherEnd = it.constCast();
		++otherEnd;
		// If both are empty.
		if (pos == it || pos == otherEnd)
		{
			return;
		}

		if (this == STD addressof(other))
		{
			return;
		}

		insertedFirstLastBeforePos(pos.constCast(), it.constCast(), otherEnd);
		this->incSize(1);
		other.decSize(1);
	}

	void splice(const_iterator pos, MyList& other, const_iterator first, const_iterator last) noexcept
	{
		splice(pos, STD move(other), first, last);
	}

	/*
	* Transfers the elements in the range [first, last) from other into *this. 
	* The elements are inserted before the element pointed to by pos.
	* The behavior is undefined if pos is an iterator in the range [first,last).
	*/
	void splice(const_iterator pos, MyList&& other, const_iterator first, const_iterator last) noexcept
	{
		if (first != last)
		{
			// Must get size before we modifly the lsit. Because the iterator will failure.
			const size_type numberOfRemove = STD distance(first, last);

			insertedFirstLastBeforePos(pos.constCast(), first.constCast(), last.constCast());

			other.decSize(numberOfRemove);
			this->incSize(numberOfRemove);
		}
	}

private:

	void fastRemove(ListNodeBase* needToRemove) noexcept
	{
		Node* needToDelete = static_cast<Node*>(needToRemove);
		needToDelete->unlinkMyself();

		Node_Alloc_Traits::destroy(getNodeAllocator(), needToDelete);
		this->deallocateNode(needToDelete);
		this->decSize(1);
	}

public:

	size_type remove(const value_type& value)
	{
		return remove_if([&](const value_type& other) { return other == value; });
	}

	template <typename UnaryPredicate>
	size_type remove_if(UnaryPredicate p)
	{
		const size_type oldSize = size();
		MyList needToBeDestroy(get_allocator());

		// The iterator will invalied when we connect the node to needToBeDestroy. 
		// So we first copy and move the next iterator
		// to next element, and then copy back to first.
		for (auto first = begin(), last = end(), next = first; first != last; first = next)
		{
			++next;
			if (p(*first))
			{
				fastRemove(first.mNode);
			}
		}

		return oldSize - size();
	}

	void reverse() noexcept
	{
		mImpl.mHeader.doReverse();
	}

	size_type unique()
	{
		return unique(STD equal_to<>{});
	}

	template <typename BinaryPredicate>
	size_type unique(BinaryPredicate binaryPredicate)
	{		
		const size_type oldSize = size();
		for (auto first = begin(), last = end(), next = first; first != last; first = next)
		{
			++next;
			if (binaryPredicate(*first, *next))
			{
				fastRemove(first.mNode);
			}
		}

		return oldSize - size();
	}

private:

	template <typename Compare>
	void doGccSort(Compare comp)
	{
		MyList carry;
		MyList temp[64];
		MyList *fill = temp;
		MyList *counter;

		TRY_START
		do
		{
			carry.splice(carry.begin(), *this, begin());

			for (counter = temp; counter != fill && !counter->empty(); ++counter)
			{
				counter->merge(carry, comp);
				carry.swap(*counter);
			}

			carry.swap(*counter);

			if (counter == fill)
			{
				++fill;
			}
		} while (!empty());

		for (counter = temp + 1; counter != fill; ++counter)
		{
			counter->merge(*(counter - 1), comp);
		}

		swap(*(fill - 1));

		CATCH_ALL

		splice(end(), carry);

		for (size_type i = 0; i < sizeof temp / sizeof temp[0]; ++i)
		{
			splice(end(), temp[i]);
		}
		THROW_AGAIN

		END_CATCH
	}

public:

	void sort()
	{
		sort(STD less<>{});
	}

	template <typename Compare>
	void sort(Compare comp)
	{
		// Do nothing if the list has length 0 or 1.
		if (const size_type nowSize = size(); nowSize >= 0 && nowSize <= 1)
		{
			return;
		}

		doGccSort(comp);
	}

	template <typename Comsumer>
	MyList filter(Comsumer consumer)
	{
		MyList temp(get_allocator());
		for (auto& data : *this)
		{
			if (consumer(data))
			{
				temp.emplace_back(data);
			}
		}

		return temp;
	}

	template <typename Comsumer>
	Comsumer foreach(Comsumer consumer)
	{
		return STD for_each(begin(), end(), consumer);
	}

	STD list<value_type, Alloc> to_std_list()
	{
		return STD list(begin(), end());
	}

	STD list<value_type, Alloc> to_std_list() const
	{
		return STD list(begin(), end());
	}

	const STD list<value_type, Alloc> to_std_clist() const
	{
		return STD list(cbegin(), cend());
	}

};

template <typename T, typename Alloc>
inline bool operator==(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs)
{
	if (rhs.size() != rhs.size())
	{
		return false;
	}

	for (auto lStart = lhs.begin(), rStart = rhs.begin(), lEnd = lhs.end(), rEnd = rhs.end();
		lStart != lEnd;
		++lStart, ++rStart)
	{
		if (*lStart != *rStart)
		{
			return false;
		}
	}

	return true;
}

template <typename T, typename Alloc>
void swap(MyList<T, Alloc>& lhs, MyList<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}

template <typename T, typename Alloc>
inline bool operator!=(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
inline bool operator<(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs)
{
	return STD lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
inline bool operator<=(const MyList<T, Alloc>& lhs, const std::list<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template <typename T, typename Alloc>
inline bool operator>(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template <typename T, typename Alloc>
inline bool operator>=(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

/*
* This deduction guide is provided for list to allow deduction from an iterator range. 
* This overload participates in overload resolution only if InputIt satisfies 
* LegacyInputIterator and Alloc satisfies Allocator.
*
* Note: the extent to which the library determines that a type does not satisfy LegacyInputIterator 
* is unspecified, except that as a minimum integral types do not qualify as input iterators. 
* Likewise, the extent to which it determines that a type does not satisfy Allocator is unspecified, 
* except that as a minimum the member type Alloc::value_type must exist and the expression 
* std::declval<Alloc&>().allocate(std::size_t{}) must be well-formed when treated as an unevaluated operand.
* 
* refer from: https://en.cppreference.com/w/cpp/container/list/deduction_guides
*/
template 
<
	typename InputIterator, 
	typename ValT = typename STD iterator_traits<InputIterator>::value_type,
	typename Allocator = STD allocator<ValT>,
	typename = RequireInputIter<InputIterator>,
	typename = RequireAllocator<Allocator>
>
MyList(InputIterator, InputIterator, Allocator = Allocator()) -> MyList<ValT, Allocator>;

JSTD_END

#endif // !MYLIST
