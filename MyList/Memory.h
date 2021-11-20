#pragma once
#ifndef MEMORY
#define MEMORY

#include <type_traits>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "Healper.h"

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

	AllocatorBase(AllocatorBase& other) noexcept {}

	template <typename T2>
	AllocatorBase(const AllocatorBase<T2>& other) noexcept {}

	~AllocatorBase() = default;

	T* allocate(size_type size, const void* = static_cast<const void*>(0))
	{
		if (size == 0)
		{
			return nullptr;
		}

		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			return static_cast<T*>(::operator new(size * sizeof(T), STD align_val_t(alignof(T))));
		}

		return static_cast<T*>(::operator new(size * sizeof(T)));
	}

	void deallocate(T* p, size_type size)
	{
		if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			::operator delete(p, size * sizeof(T), STD align_val_t(alignof(T)));
		}

		::operator delete(p, size * sizeof(T));
	}

	size_type max_size() const noexcept
	{
		return STD numeric_limits<size_type>::max() / static_cast<size_type>(sizeof(T));
	}

	template <typename Type, typename... Args>
	void construct(Type* p, Args&&... args) noexcept(STD is_nothrow_constructible_v<Type, Args...>)
	{
		::new(reinterpret_cast<void*>(p)) Type(STD forward<Args>(args)...);
	}

	template <typename Type>
	void destroy(Type* p) noexcept(STD is_nothrow_destructible_v<Type>)
	{
		p->~Type();
	}

	pointer address(reference r) const noexcept
	{
		return STD addressof(r);
	}

	const_pointer address(const_reference r) const noexcept
	{
		return STD addressof(r);
	}

};

template <typename T>
class Allocator : public AllocatorBase<T>
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
		using other = Allocator<T2>;
	};

	Allocator() = default;

	Allocator(const Allocator& other) noexcept
		: AllocatorBase<T>(other) { }

	Allocator& operator=(const Allocator& other) = default;

	template <typename T2>
	Allocator(const Allocator<T2>& other) noexcept { }

	~Allocator() = default;

	friend bool operator==(const Allocator&, const Allocator&) noexcept
	{
		return true;
	}

	friend bool operator!=(const Allocator&, const Allocator&) noexcept
	{
		return false;
	}

};

template<typename T1, typename T2>
inline bool
operator==(const Allocator<T1>&, const Allocator<T2>&) noexcept
{
	return true;
}

template<typename T1, typename T2>
inline bool
operator!=(const Allocator<T1>&, const Allocator<T2>&) noexcept
{
	return false;
}

/*
* Start unique pointer.
*/
template <typename T>
struct DefaultDelete
{
	constexpr DefaultDelete() noexcept = default;


	template <typename U, typename = require<STD is_convertible<U*, T*>>>
	DefaultDelete(const DefaultDelete<U>&) noexcept { }

	void operator()(T* ptr) const
	{
		static_assert(!STD is_void_v<T>, "can't delete pointer to incomplete type");
		static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");
		delete ptr;
	}
};

template <typename T>
struct DefaultDelete<T[]>
{
	constexpr DefaultDelete() noexcept = default;

	/*
	* Converting constructor.
	* Allows conversion from a deleter for arrays of another type,
	* such as a const-qualified version of `T`.
	* Conversions from types derived from `T`
	* are not allowed because it is undefined to `delete[]`
	* an array of derived types through a pointer to the base type.
	*/
	template <typename U, typename = require<STD is_convertible<U(*)[], T(*)[]>>>
	DefaultDelete(const DefaultDelete<U[]>&) noexcept { }

	void operator()(T* ptr) const
	{
		static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");
		delete[] ptr;
	}
};

template <typename T, typename D>
class UniqPtrImpl
{
	template <typename U, typename E, typename = void>
	struct Ptr
	{
		using type = U*;
	};

	template <typename U, typename E>
	struct Ptr<U, E, STD void_t<typename STD remove_reference_t<E>::pointer>>
	{
		using type = typename STD remove_reference_t<E>::pointer;
	};

public:

	using DeleterConstraint = STD enable_if<STD conjunction_v<STD negation<STD is_pointer<D>>, STD is_default_constructible<D>>>;
	using pointer = typename Ptr<T, D>::type;

	static_assert(!STD is_rvalue_reference_v<D>, "unique_ptr's deleter type must be a function object type"
		" or an lvalue reference type");

	UniqPtrImpl() = default;

	UniqPtrImpl(pointer p)
		: mT()
	{
		mPtr() = p;
	}

	template <typename Del>
	UniqPtrImpl(pointer p, Del&& d)
		: mT(p, STD forward<Del>(d)) { }

	UniqPtrImpl(UniqPtrImpl&& other) noexcept
		: mT(STD move(other.mT))
	{
		other.mT = nullptr;
	}

	UniqPtrImpl& operator=(UniqPtrImpl&& other) noexcept
	{
		reset(other.release());
		mDeleter() = STD forward<D>(other.mDeleter());
		return *this;
	}

	~UniqPtrImpl() = default;

	void reset(pointer p) noexcept
	{
		const pointer old = mPtr();
		mPtr() = p;
		if (old)
		{
			mDeleter()(old);
		}
	}

	pointer release() noexcept
	{
		pointer p = mPtr();
		mPtr() = nullptr;
		return p;
	}

	void swap(UniqPtrImpl& right) noexcept
	{
		using STD swap;
		swap(mPtr(), right.mPtr());
		swap(mDeleter(), right.mDeleter());
	}

	pointer& mPtr()
	{
		return STD get<0>(mT);
	}

	pointer mPtr() const
	{
		return STD get<0>(mT);
	}

	D& mDeleter()
	{
		return STD get<1>(mT);
	}

	const D& mDeleter() const
	{
		return STD get<1>(mT);
	}

private:

	STD tuple<pointer, D> mT;
};

template <typename T, typename D, bool = STD is_move_constructible_v<D>, bool = STD is_move_assignable_v<D>>
struct UniqPtrData : UniqPtrImpl<T, D>
{
	using UniqPtrImpl<T, D>::UniqPtrImpl;

	UniqPtrData(UniqPtrData&&) = default;
	UniqPtrData& operator=(UniqPtrData&&) = default;
};

template <typename T, typename D>
struct UniqPtrData<T, D, false, true> : UniqPtrImpl<T, D>
{
	using UniqPtrImpl<T, D>::UniqPtrImpl;

	UniqPtrData(UniqPtrData&&) = delete;
	UniqPtrData& operator=(UniqPtrData&&) = default;
};

template <typename T, typename D>
struct UniqPtrData<T, D, false, false> : UniqPtrImpl<T, D>
{
	using UniqPtrImpl<T, D>::UniqPtrImpl;

	UniqPtrData(UniqPtrData&&) = delete;
	UniqPtrData& operator=(UniqPtrData&&) = delete;
};

template <typename T, typename D = DefaultDelete<T>>
class UniquePtr
{

	template <typename U>
	using DeleterConstraint = typename UniqPtrImpl<T, U>::DeleterConstraint::type;

	UniqPtrData<T, D> ptrData;

public:

	using pointer = typename UniqPtrImpl<T, D>::pointer;
	using element_type = T;
	using deleter_type = D;

private:

	template <typename U, typename E>
	using safe_conversion_u = STD conjunction<
		STD is_convertible<typename UniquePtr<U, E>::pointer, pointer>, 
		STD negation<STD is_array<U>>
	>;

public:

	template <typename Del = D, typename = DeleterConstraint<Del>>
	constexpr UniquePtr() noexcept
		: ptrData() 
	{ }

	template<typename Del = D, typename = DeleterConstraint<Del>>
	explicit UniquePtr(pointer ptr) noexcept
		: ptrData(ptr)
	{ }

	template<typename Del = deleter_type, typename = require<STD is_copy_constructible<Del>>>
		UniquePtr(pointer ptr, const deleter_type& del) noexcept
		: ptrData(ptr, del) 
	{ }

	template <typename Del = deleter_type,
		typename = require<STD is_move_constructible<Del>>>
		UniquePtr(pointer ptr, STD enable_if_t<!STD is_lvalue_reference_v<Del>, Del&&> del) noexcept
		: ptrData(ptr, STD move(del))
	{ }

	template<typename Del = deleter_type, typename DelUnref = typename STD remove_reference_t<Del>>
	UniquePtr(pointer, STD enable_if_t<STD is_lvalue_reference_v<Del>, DelUnref&&>) = delete;

	template<typename U, typename E, typename = require<
		safe_conversion_u<U, E>,
		typename STD conditional_t<
			STD is_reference_v<D>, 
			STD is_same<E, D>,
			STD is_convertible<E, D>>>>
		UniquePtr(UniquePtr<U, E>&& other) noexcept
		: ptrData(other.release(), STD forward<E>(other.get_deleter()))
	{ }

	template <typename Del = D, typename = DeleterConstraint<Del>>
	constexpr UniquePtr(STD nullptr_t) noexcept
		: ptrData()
	{ }

	~UniquePtr() noexcept
	{
		static_assert(STD is_invocable_v<deleter_type&, pointer>, "unique_ptr's deleter must be invocable with a pointer");
		auto& ptr = ptrData.mPtr();
		if (ptr)
		{
			get_deleter()(STD move(ptr));
		}
			
		ptr = pointer();
	}

	template <typename U, typename E>
	typename STD enable_if_t<STD conjunction_v<safe_conversion_u<U, E>, STD is_assignable<deleter_type&, E&&>, UniquePtr&>>
		operator=(UniquePtr<U, E>&& other) noexcept
	{
		reset(other.release());
		get_deleter() = STD forward<E>(other.get_deleter());
		return *this;
	}

	UniquePtr& operator=(UniquePtr&&) = default;

	UniquePtr& operator=(STD nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	typename STD add_lvalue_reference_t<element_type> operator*() const
	{
		return *get();
	}

	pointer operator->() const noexcept
	{
		return get();
	}

	pointer get() const noexcept
	{
		return ptrData.mPtr();
	}

	const deleter_type& get_deleter() noexcept
	{
		return ptrData.mDeleter();
	}

	explicit operator bool() const noexcept
	{
		return get() != pointer();
	}

	pointer release() noexcept
	{
		return ptrData.release();
	}

	void reset(pointer p = pointer()) noexcept
	{
		static_assert(STD is_invocable_v<deleter_type&, pointer>, "unique_ptr's deleter must be invocable with a pointer");
		ptrData.reset(STD move(p));
	}

	void swap(UniquePtr& other) noexcept
	{
		static_assert(STD is_swappable_v<D>, "deleter must be swappable");
		ptrData.swap(other.ptrData);
	}

	UniquePtr(const UniquePtr&) = delete;

	UniquePtr& operator=(const UniquePtr&) = delete;

};

template <typename T, typename D>
class UniquePtr<T[], D>
{
public:
	UniquePtr();
	~UniquePtr();

private:

};



JSTD_END

#endif // !ALLOCATOR
