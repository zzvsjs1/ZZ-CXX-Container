#pragma once
#ifndef ALLOCATOR
#define ALLOCATOR

#include <type_traits>
#include <limits>

JSTD_START

template <typename T>
class AllocatorBase
{
public:

	using value_type = T;
	using size_type = STD size_t;
	using difference_type = STD ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using propagate_on_container_move_assignment = STD true_type;

	template <typename T2>
	struct rebind
	{
		using other = AllocatorBase<T2>;
	};

	AllocatorBase() = default;

	AllocatorBase(AllocatorBase&) noexcept { }

	template <typename T2>
	AllocatorBase(const AllocatorBase<T2>&) noexcept { }

	~AllocatorBase() = default;

	T* allocate(size_type size)
	{
		if (size == 0)
		{
			return nullptr;
		}

		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			return static_cast<T*>(::operator new(size * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
		}

		return static_cast<T*>(::operator new(size * sizeof(T)));
	}

	void deallocate(T* p, size_type size)
	{
		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			::operator delete(p, size * sizeof(T), static_cast<std::align_val_t>(alignof(T)));
		}

		::operator delete(p, size * sizeof(T));
	}

	NODISCARD size_type max_size() const noexcept
	{
		return STD numeric_limits<size_type>::max() /sizeof(T);
	}

	template <typename Type, typename... Args>
	void construct(Type* p, Args&&... args) noexcept(STD is_nothrow_constructible_v<Type, Args...>)
	{
		::new(reinterpret_cast<void*>(p)) Type(STD forward<Args>(args)...);
	}

	template <typename ArrayType>
	void destroy(ArrayType* p) noexcept(STD is_nothrow_destructible_v<ArrayType>)
	{
		p->~ArrayType();
	}

	NODISCARD pointer address(reference r) const noexcept
	{
		return STD addressof(r);
	}

	NODISCARD const_pointer address(const_reference r) const noexcept
	{
		return STD addressof(r);
	}
};

template <typename T>
class MyAllocator : public AllocatorBase<T>
{
public:

	static_assert(!STD is_const_v<T>, "The C++ Standard forbids containers of const elements "
									  "because allocator<const T> is ill-formed.");

	using value_type = T;
	using size_type = STD size_t;
	using difference_type = STD ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	using propagate_on_container_move_assignment = STD true_type;
	using is_always_equal = STD true_type;

	template <typename T2>
	struct rebind
	{
		using other = MyAllocator<T2>;
	};

	MyAllocator() = default;

	MyAllocator(const MyAllocator& other) noexcept
		: AllocatorBase<T>(other) { }

	MyAllocator& operator=(const MyAllocator& other) = default;

	template <typename T2>
	MyAllocator(const MyAllocator<T2>& other) noexcept { }

	MyAllocator(MyAllocator&&) = default;

	~MyAllocator() = default;

	friend bool operator==(const MyAllocator&, const MyAllocator&) noexcept
	{
		return true;
	}

	friend bool operator!=(const MyAllocator&, const MyAllocator&) noexcept
	{
		return false;
	}
};

template<typename T1, typename T2>
inline bool
operator==(const MyAllocator<T1>&, const MyAllocator<T2>&) noexcept
{
	return true;
}

template<typename T1, typename T2>
inline bool
operator!=(const MyAllocator<T1>&, const MyAllocator<T2>&) noexcept
{
	return false;
}

JSTD_END

#endif // !ALLOCATOR
