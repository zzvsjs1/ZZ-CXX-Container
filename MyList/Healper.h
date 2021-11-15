#pragma once
#ifndef UTILITY
#define UTILITY

#include <type_traits>
#include <memory>

#ifndef JSTD_START

#define JSTD_START namespace jstd {
#define JSTD_END }

#endif // !JSTD_START

JSTD_START

#define STD ::std::
#define NODISCARD [[nodiscard]]
#define TRY_START try {
#define CATCH_ALL } catch(...) {
#define THROW_AGAIN throw;
#define END_CATCH }

// C++17 version of the GCC "is custom pointer" template.
template <typename Pointer1, typename Pointer2>
using is_custom_pointer = STD conjunction<STD is_same<Pointer1, Pointer2>, STD negation<STD is_pointer<Pointer2>>>;

//
template <typename Pointer1, typename Pointer2>
inline constexpr bool is_custom_pointer_v = is_custom_pointer<Pointer1, Pointer2>::value;

template <typename... Cond>
using require = STD enable_if_t<STD conjunction_v<Cond...>>;

// A shortcut to get the iterator_category.
template <typename Iter>
using iterator_category_t = typename STD iterator_traits<Iter>::iterator_category;

// Check if the data_type is input iterator. We need to get the iterator_category first, and then
// check if the tag is inhiread to the std::input_iterator_tag.
template <typename Iter>
using is_input_iter = STD is_convertible<iterator_category_t<Iter>, STD input_iterator_tag>;

// A shortcut to get the is_input_iter's value.
template <typename Iter>
inline constexpr bool is_input_iter_v = is_input_iter<Iter>::value;

// 
template <typename T>
constexpr T*
fancyPointerToAddress(T* ptr) noexcept
{
	static_assert(!STD is_function_v<T>, "Please do not passing a function pointer.");
	return ptr;
}

//
template <typename Ptr>
constexpr typename STD pointer_traits<Ptr>::element_type*
fancyPointerToAddress(const Ptr& ptr)
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
	template <typename Ptr, typename... Args>
	static constexpr STD enable_if_t<is_custom_pointer_v<pointer, Ptr>> 
		construct(Alloc& alloc, Ptr ptr, Args&&... args)
	{
		Base_type::construct(alloc, fancyPointerToAddress(ptr), STD forward<Args>(args)...);
	}

	// overload destroy for non-standard pointer types
	template <typename Ptr>
	static constexpr STD enable_if_t<is_custom_pointer_v<pointer, Ptr>> destroy(Alloc& alloc, Ptr ptr)
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
		: m_alloc(STD addressof(alloc)), m_ptr(ptr)
	{}

	template <typename Ptr, typename Req = require<STD is_same<Ptr, value_type*>>>
	AllocatedPtrGuard(Alloc& alloc, Ptr ptr)
		: m_alloc(STD addressof(alloc)), m_ptr(STD pointer_traits<pointer>::pointer_to(*ptr))
	{}

	AllocatedPtrGuard(AllocatedPtrGuard&& other) noexcept
		: m_alloc(other.m_alloc), m_ptr(other.m_ptr)
	{
		other.m_ptr = nullptr;
	}

	~AllocatedPtrGuard()
	{
		if (m_ptr)
		{
			STD allocator_traits<Alloc>::deallocate(*m_alloc, m_ptr, 1);
		}
	}

	pointer release()
	{
		pointer temp = m_ptr;
		m_ptr = nullptr;
		return temp;
	}

	AllocatedPtrGuard& operator=(STD nullptr_t) noexcept
	{
		m_ptr = nullptr;
		return *this;
	}

	value_type* get()
	{
		fancyPointerToAddress(m_ptr);
	}

	template <typename Alloc2>
	static AllocatedPtrGuard<Alloc2> guarded(Alloc2& alloc)
	{
		return { alloc, STD allocator_traits<Alloc2>::allocate(alloc, 1) };
	}

private:

	Alloc* m_alloc;
	pointer m_ptr;

};



JSTD_END


#endif //UTILITY