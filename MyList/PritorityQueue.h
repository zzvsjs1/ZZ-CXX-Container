#pragma once
#ifndef PRITORITY_QUEUE
#define PRITORITY_QUEUE

#include <algorithm>
#include <memory>
#include <iterator>
#include <type_traits>
#include <initializer_list>
#include <stdexcept>
#include <vector>

#include "Healper.h"

JSTD_START

template <typename T, typename Container = STD vector<T>, typename Compare = STD less<typename Container::value_type>>
class PriorityQueue
{
	static_assert(STD is_same_v<T, typename Container::value_type>, "value_type must be the same as the underlying container");

public:

	using value_type = typename	Container::value_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;
	using size_type = typename Container::size_type;
	using container_type = Container;
	using value_compare = Compare;

private:

	Container container;
	Compare comp;

	template <typename Alloc>
	using EnableIfUsesAllocator = STD enable_if_t<STD uses_allocator_v<Container, Alloc>>;

public:

	template 
	<
		typename aContainer = Container, 
		typename Requires 
		= STD enable_if_t<STD conjunction_v<STD is_default_constructible<Compare>, STD is_default_constructible<aContainer>>>
	>
	PriorityQueue()
		: container(), comp() { }

	explicit PriorityQueue(const Compare& compare, const Container& otherContainer)
		: container(otherContainer), comp(compare) 
	{
		STD make_heap(otherContainer.begin(), otherContainer.end(), comp);
	}

	explicit PriorityQueue(const Compare& compare, const Container&& otherContainer = Container())
		: container(STD move(otherContainer)), comp(compare)
	{
		STD make_heap(otherContainer.begin(), otherContainer.end(), comp);
	}

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	explicit PriorityQueue(const Alloc& alloc)
		: container(alloc), comp() { }

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	PriorityQueue(const Compare& otherComp, const Alloc& alloc)
		: container(alloc), comp(otherComp) { }

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	PriorityQueue(const Compare& otherComp, const Container& otherContainer, const Alloc& alloc)
		: container(otherContainer, alloc), comp(otherComp)
	{
		STD make_heap(container.begin(), container.end(), comp);
	}

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	PriorityQueue(const Compare& otherComp, Container&& otherContainer, const Alloc& alloc)
		: container(STD move(otherContainer), alloc), comp(otherComp)
	{
		STD make_heap(container.begin(), container.end(), comp);
	}

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	PriorityQueue(const PriorityQueue& other, const Alloc& alloc)
		: container(other.container, alloc), comp(other.comp) { }

	template <typename Alloc, typename Requires = EnableIfUsesAllocator<Alloc>>
	PriorityQueue(PriorityQueue&& other, const Alloc& alloc)
		: container(STD	move(other.container), alloc), comp(STD move(other.comp)) { }

	template <typename InputIterator>
	PriorityQueue(InputIterator first, InputIterator last, const Compare& otherComp, const Container& otherContainer)
		: container(otherContainer), comp(otherComp)
	{
		container.insert(container.end(), first, last);
		STD make_heap(container.begin(), container.end(), comp);
	}

	template <typename InputIterator>
	PriorityQueue(InputIterator first, InputIterator last, const Compare& otherComp = Compare(), Container&& otherContainer = Container())
		: container(STD move(otherContainer)), comp(otherComp)
	{
		container.insert(container.end(), first, last);
		STD make_heap(container.begin(), container.end(), comp);
	}

	~PriorityQueue() = default;

	NODISCARD bool empty() const
	{
		return container.empty();
	}

	NODISCARD size_type size() const
	{
		return container.size();
	}

	NODISCARD const_reference top() const
	{
		return container.front();
	}

	void push(const value_type& value)
	{
		container.push_back(value);
		STD push_heap(container.begin(), container.end(), comp);
	}

	void push(const value_type&& value)
	{
		container.push_back(STD move(value));
		STD push_heap(container.begin(), container.end(), comp);
	}

	template <typename... Args>
	void emplace(Args&&... args)
	{
		container.emplace_back(STD forward<Args>(args)...);
		STD push_heap(container.begin(), container.end(), comp);
	}

	void pop()
	{
		STD pop_heap(container.begin(), container.end(), comp);
		container.pop_back();
	}

	void swap(PriorityQueue& other) noexcept(STD conjunction_v<STD is_nothrow_swappable<Container>, STD is_nothrow_swappable<Compare>>)
	{
		using STD swap;
		swap(container, other.container);
		swap(comp, other.comp);
	}
};

template
<
	typename Compare, typename Container,
	typename = RequireNotAllocator<Compare>,
	typename = RequireNotAllocator<Container>
>
PriorityQueue(Compare, Container) -> PriorityQueue<typename Container::value_type, Container, Compare>;

template
<
	typename InputIterator, 
	typename ValT = typename STD iterator_traits<InputIterator>::value_type,
	typename Compare = STD less<ValT>,
	typename Container = STD vector<ValT>,
	typename = RequireInputIter<InputIterator>,
	typename = RequireNotAllocator<Compare>,
	typename = RequireNotAllocator<Container>
>
PriorityQueue(InputIterator, InputIterator, Compare = Compare(), Container = Container()) 
	-> PriorityQueue<ValT, Container, Compare>;

template
<
	typename Compare, 
	typename Container, 
	typename Alloc,
	typename = RequireNotAllocator<Compare>,
	typename = RequireNotAllocator<Container>,
	typename = RequireAllocator<Alloc>
>
PriorityQueue(Compare, Container, Alloc) -> PriorityQueue<typename Container::value_type, Container, Compare>;

template<typename T, typename Container, typename Compare> 
inline typename STD enable_if_t<STD conjunction_v<STD is_swappable<Container>, STD is_swappable<Compare>>>
swap(PriorityQueue<T, Container, Compare>& left, PriorityQueue<T, Container, Compare>& right) noexcept(noexcept(left.swap(right)))
{
	left.swap(right);
}

JSTD_END

#endif // !PRITORITY_QUEUE
