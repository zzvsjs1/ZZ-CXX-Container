#pragma once
#ifndef HELPER
#define HELPER

#include <algorithm>
#include <type_traits>
#include <memory>

#include "Config.h"

JSTD_START

// C++17 version of the GCC "is custom pointer" template.
template <typename Pointer1, typename Pointer2>
using is_custom_pointer = STD conjunction<STD is_same<Pointer1, Pointer2>, STD negation<STD is_pointer<Pointer2>>>;

/*
* 
*/
template <typename Pointer1, typename Pointer2>
inline constexpr bool is_custom_pointer_v = is_custom_pointer<Pointer1, Pointer2>::value;

/*
* 
*/
template <typename... Cond>
using require = STD enable_if_t<STD conjunction_v<Cond...>>;

// A shortcut to get the iterator_category.
template <typename Iter>
using iterator_category_t = typename STD iterator_traits<Iter>::iterator_category;

/*
* Check if the data_type is input iterator. 
* We need to get the iterator_category first, and then
* check if the tag is derived from to the std::input_iterator_tag.
*/
template <typename Iter>
using is_input_iter = STD is_convertible<iterator_category_t<Iter>, STD input_iterator_tag>;

// A shortcut to get the is_input_iter's value.
template <typename Iter>
inline constexpr bool is_input_iter_v = is_input_iter<Iter>::value;

// Determine if the type is an input iterator.
template <typename InputIter>
using RequireInputIter = STD enable_if_t<STD is_convertible_v<iterator_category_t<InputIter>, STD input_iterator_tag>>;

/* 
* Is allocator template.
* Trait to detect Allocator-like types.
*/
template <typename Alloc, typename = void>
struct is_allocator : STD false_type { };

// 
template <typename Alloc>
struct is_allocator 
	<Alloc, STD void_t<typename Alloc::value_type, decltype(STD declval<Alloc&>().allocate(size_t{}))>>
	: STD true_type { };

// A shortcut to get the is_allocator's value.
template <typename Alloc>
inline constexpr bool is_allocator_v = is_allocator<Alloc>::value;

// 
template <typename Alloc>
using RequireAllocator = STD enable_if_t<is_allocator_v<Alloc>, Alloc>;

// 
template <typename Alloc>
using RequireNotAllocator = STD enable_if_t<!is_allocator_v<Alloc>, Alloc>;

// handle false trait or last trait.
template <bool First_value, class First, class... Rest>
struct __Conjunction { 
	using type = First;
};

// the first trait is true, try the next one.
template <class True, class Next, class... Rest>
struct __Conjunction<true, True, Next, Rest...> { 
	using type = typename __Conjunction<Next::value, Next, Rest...>::type;
};

// If Traits is empty, true_type.
template <class... Traits>
struct Conjunction : STD true_type { }; 

// the first false trait in Traits, or the last trait if none are false.
template <class First, class... Rest>
struct Conjunction<First, Rest...> : __Conjunction<First::value, First, Rest...>::type { };

template <class... Traits>
inline constexpr bool Conjunction_v = Conjunction<Traits...>::value;

template <typename T>
constexpr T*
fancyPointerToAddress(T* ptr) noexcept
{
	static_assert(!STD is_function_v<T>, "Please do not passing a function pointer.");
	return ptr;
}

//
template <typename ArrayType>
constexpr typename STD pointer_traits<ArrayType>::element_type*
fancyPointerToAddress(const ArrayType& ptr)
{
	return fancyPointerToAddress(ptr.operator->());
}
 
template <typename Alloc>
constexpr inline void
doAllocSwap(Alloc& left, Alloc& right)
{
	using traits = STD allocator_traits<Alloc>;
	using pocs = typename traits::propagate_on_container_swap;

	if constexpr (pocs::value)
	{
		using STD swap;
		swap(left, right);
	}
}

template <typename Alloc>
constexpr inline void
doAllocCopy(Alloc& left, Alloc& right)
{
	using traits = STD allocator_traits<Alloc>;
	using pocca = typename traits::propagate_on_container_copy_assignment;

	if constexpr (pocca::value)
	{
		left = right;
	}
}

template <typename Alloc>
constexpr inline void
doAllocMove(Alloc& left, Alloc& right)
{
	using traits = STD allocator_traits<Alloc>;
	using pocma = typename traits::propagate_on_container_move_assignment;

	if constexpr (pocma::value)
	{
		left = STD move(right);
	}
}

template <typename T>
struct MoveIfNoexceptCondition
	: public STD conjunction<STD negation<STD is_nothrow_constructible<T>>, STD is_copy_constructible<T>>::type
{ };

template <typename T>
inline constexpr bool MoveIfNoexceptCondition_v = MoveIfNoexceptCondition<T>::value;

template <typename Iterator, typename ReturnT
	= STD conditional_t<
	MoveIfNoexceptCondition_v<
	typename STD iterator_traits<Iterator>::value_type>, Iterator, STD move_iterator<Iterator>>
>
inline constexpr ReturnT makeMoveIfNoexceptIterator(Iterator itr)
{
	return ReturnT(itr);
}

template <typename Alloc, typename = typename Alloc::value_type>
struct MyAlloctTraits : public STD allocator_traits<Alloc> 
{
	using allocator_type = Alloc;
	using Base_type = STD allocator_traits<Alloc>;
	using value_type = typename Base_type::value_type;
	using pointer = typename Base_type::pointer;
	using const_pointer = typename Base_type::const_pointer;
	using size_type = typename Base_type::size_type;
	using difference_type = typename Base_type::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;

	using Base_type::allocate;
	using Base_type::deallocate;
	using Base_type::construct;
	using Base_type::destroy;
	using Base_type::max_size;

	// overload construct for non-standard pointer types
	template <typename ArrayType, typename... Args>
	static constexpr STD enable_if_t<is_custom_pointer_v<pointer, ArrayType>> 
		construct(Alloc& alloc, ArrayType ptr, Args&&... args)
	{
		Base_type::construct(alloc, fancyPointerToAddress(ptr), STD forward<Args>(args)...);
	}

	// overload destroy for non-standard pointer types
	template <typename ArrayType>
	static constexpr STD enable_if_t<is_custom_pointer_v<pointer, ArrayType>> destroy(Alloc& alloc, ArrayType ptr)
	{
		Base_type::destroy(alloc, fancyPointerToAddress(ptr));
	}

	static constexpr Alloc select_on_container_copy_construction(const Alloc& alloc)
	{
		return Base_type::select_on_container_copy_construction(alloc);
	}

	static constexpr void doCopy(Alloc& left, Alloc& right)
	{
		doAllocCopy(left, right);
	}

	static constexpr void doSwap(Alloc& left, Alloc& right)
	{
		doAllocSwap(left, right);
	}

	static constexpr void doMove(Alloc& left, Alloc& right)
	{
		doAllocMove(left, right);
	}

	static constexpr bool propagate_on_container_copy_assignment_v()
	{
		return Base_type::propagate_on_container_copy_assignment::value;
	}

	static constexpr bool propagate_on_move_assign_v()
	{
		return Base_type::propagate_on_container_move_assignment::value;
	}

	static constexpr bool propagate_on_swap_v()
	{
		return Base_type::propagate_on_container_swap::value;
	}

	static constexpr bool always_equal_v()
	{
		return Base_type::is_always_equal::value;
	}

	static constexpr bool nothrow_move()
	{
		return propagate_on_move_assign_v() || always_equal_v();
	}

	template <typename OtherT>
	struct rebind
	{
		using other = typename Base_type:: template rebind_alloc<OtherT>;
	};
};

template <typename Alloc>
struct AllocatedPtrGuard
{
	using pointer = typename STD allocator_traits<Alloc>::pointer;
	using value_type = typename STD allocator_traits<Alloc>::value_type;

	AllocatedPtrGuard(Alloc& alloc, pointer ptr) noexcept
		: mAlloc(STD addressof(alloc)), mPtr(ptr)
	{}

	template <typename ArrayType, typename Req = require<STD is_same<ArrayType, value_type*>>>
	AllocatedPtrGuard(Alloc& alloc, ArrayType ptr)
		: mAlloc(STD addressof(alloc)), mPtr(STD pointer_traits<pointer>::pointer_to(*ptr))
	{}

	AllocatedPtrGuard(AllocatedPtrGuard&& other) noexcept
		: mAlloc(other.mAlloc), mPtr(other.mPtr)
	{
		other.mPtr = nullptr;
	}

	~AllocatedPtrGuard()
	{
		if (mPtr)
		{
			STD allocator_traits<Alloc>::deallocate(*mAlloc, mPtr, 1);
		}
	}

	pointer release()
	{
		pointer temp = mPtr;
		mPtr = nullptr;
		return temp;
	}

	AllocatedPtrGuard& operator=(STD nullptr_t) noexcept
	{
		mPtr = nullptr;
		return *this;
	}

	value_type* get()
	{
		return fancyPointerToAddress(mPtr);
	}

	template <typename Alloc2>
	static AllocatedPtrGuard<Alloc2> guarded(Alloc2& alloc)
	{
		return { alloc, STD allocator_traits<Alloc2>::allocate(alloc, 1) };
	}

private:

	Alloc* mAlloc;
	pointer mPtr;

};

// Tag type for value-initializing first, constructing second from remaining args.
struct ZeroThenVariadicArgsT {
	explicit ZeroThenVariadicArgsT() = default;
}; 

// Tag type for constructing first from one arg, constructing second from remaining args.
struct OneThenVariadicArgsT {
	explicit OneThenVariadicArgsT() = default;
}; 

// Store a pair of values, deriving from empty first.
template <typename T1, typename T2, bool = STD conjunction_v<STD is_empty<T1>, STD negation<STD is_final<T1>>>>
class CompressedPair final : private T1 
{
public:

	T2 second;

	using Base = T1; // for visualization

	template <typename... Other2>
	constexpr explicit CompressedPair(ZeroThenVariadicArgsT, Other2&&... value2) 
		noexcept(
		STD conjunction_v<STD is_nothrow_default_constructible<T1>, STD is_nothrow_constructible<T2, Other2...>>
			)
		: T1(), second(STD forward<Other2>(value2)...) 
	{ }

	template <typename Other1, typename... Other2>
	constexpr CompressedPair(OneThenVariadicArgsT, Other1&& value1, Other2&&... value2) 
		noexcept(
		STD conjunction_v<STD is_nothrow_constructible<T1, Other1>, STD is_nothrow_constructible<T2, Other2...>>
		)
		: T1(STD forward<Other1>(value1)), second(STD forward<Other2>(value2)...) 
	{ }

	constexpr T1& first() noexcept 
	{
		return *this;
	}

	constexpr const T1& first() const noexcept 
	{
		return *this;
	}
};

// Store a pair of values, not deriving from first, because the first one is not empty.
template <typename T1, typename T2>
class CompressedPair<T1, T2, false> final 
{
public:

	T1 value1;

	T2 value2;

	template <typename... Other2>
	constexpr explicit CompressedPair(ZeroThenVariadicArgsT, Other2&&... otherValues) 
		noexcept(
			STD conjunction_v<STD is_nothrow_default_constructible<T1>, STD is_nothrow_constructible<T2, Other2...>>
			)
		: value1(), value2(STD forward<Other2>(otherValues)...) 
	{ }

	template <typename Other1, typename... Other2>
	constexpr CompressedPair(OneThenVariadicArgsT, Other1&& otherValue1, Other2&&... otherValues2) 
		noexcept(
			STD conjunction_v<STD is_nothrow_constructible<T1, Other1>, STD is_nothrow_constructible<T2, Other2...>>
			)
		: value1(STD forward<Other1>(otherValue1)), value2(STD forward<Other2>(otherValues2)...)
	{ }

	constexpr T1& first() noexcept 
	{
		return value1;
	}

	constexpr const T1& first() const noexcept
	{
		return value1;
	}
};

template <typename T>
constexpr inline void myDestroyInPlace(T& object) JLIBCXX_NOEXCEPT
{
	if constexpr (STD is_array_v<T>)
	{
		myDestroyRange(object, object + STD extent_v<T>);
	}
	else
	{
		object.~T();
	}
}

template <typename NoThrowFwdIt>
constexpr inline void myDestroyRange(NoThrowFwdIt first, const NoThrowFwdIt last) JLIBCXX_NOEXCEPT
{
	if constexpr (!STD is_trivially_destructible_v<STD iter_value_t<NoThrowFwdIt>>)
	{
		for (; first != last; ++first)
		{
			myDestroyInPlace(*first);
		}
	}
}

template <typename ForwardIterator, typename Allocator>
void myDestroy(ForwardIterator first, ForwardIterator last, Allocator& alloc)
{
	for (; first != last; ++first)
	{
		STD allocator_traits<Allocator>::destroy(alloc, STD addressof(*first));
	}
}

template <typename ForwardIterator, typename T>
inline void myDestroy(ForwardIterator first, ForwardIterator last, STD allocator<T>&) JLIBCXX_NOEXCEPT
{
	myDestroyRange(first, last);
}

JSTD_END

#endif // HELPER