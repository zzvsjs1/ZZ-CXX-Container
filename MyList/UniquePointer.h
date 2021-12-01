#pragma once
#ifndef UNIQUE_POINTER
#define UNIQUE_POINTER

#include <type_traits>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "Healper.h"

JSTD_START

/*
* Start unique pointer.
*/
template <typename T>
struct DefaultDelete
{
	constexpr DefaultDelete() noexcept = default;

	template <typename ValT, typename = require<STD is_convertible<ValT*, T*>>>
	DefaultDelete(const DefaultDelete<ValT>&) noexcept { }

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
	template <typename ValT, typename = require<STD is_convertible<ValT(*)[], T(*)[]>>>
	DefaultDelete(const DefaultDelete<ValT[]>&) noexcept { }

	void operator()(T* ptr) const
	{
		static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");
		delete[] ptr;
	}
};

template <typename T, typename D>
class UniqPtrDataImpl
{
protected:

	template <typename ValT, typename DelT, typename = void>
	struct Ptr
	{
		using type = ValT*;
	};

	template <typename ValT, typename DelT>
	struct Ptr<ValT, DelT, STD void_t<typename STD remove_reference_t<DelT>::pointer>>
	{
		using type = typename STD remove_reference_t<DelT>::pointer;
	};

public:

	using DeleterConstraint = STD enable_if<STD conjunction_v<STD negation<STD is_pointer<D>>, STD is_default_constructible<D>>>;
	using pointer = typename Ptr<T, D>::type;

	static_assert(!STD is_rvalue_reference_v<D>,
		"unique_ptr's deleter type must be a function object type"
		" or an lvalue reference type");

	UniqPtrDataImpl() 
		: mPair(ZeroThenVariadicArgsT{})
	{ }

	UniqPtrDataImpl(pointer p)
		: mPair(ZeroThenVariadicArgsT{}, p)
	{ }

	template <typename Del>
	UniqPtrDataImpl(pointer p, Del&& d)
		: mPair(OneThenVariadicArgsT{}, STD move(d), p)
	{ }

	UniqPtrDataImpl(UniqPtrDataImpl&& other) noexcept
		: mPair(OneThenVariadicArgsT{}, STD forward<D>(other.getDeleter()), STD move(other.release()))
	{ }

	UniqPtrDataImpl& operator=(UniqPtrDataImpl&& other) noexcept
	{
		reset(other.release());
		getDeleter() = STD forward<D>(other.getDeleter());
		return *this;
	}

	~UniqPtrDataImpl() = default;

	void reset(pointer p) noexcept
	{
		const pointer old = getPtr();
		getPtr() = p;
		if (old)
		{
			getDeleter()(old);
		}
	}

	pointer release() noexcept
	{
		pointer p = getPtr();
		getPtr() = nullptr;
		return p;
	}

	void swap(UniqPtrDataImpl& right) noexcept
	{
		using STD swap;
		swap(getPtr(), right.getPtr());
		swap(getDeleter(), right.getDeleter());
	}

	pointer& getPtr()
	{
		return mPair.second;
	}

	pointer getPtr() const
	{
		return mPair.second;
	}

	D& getDeleter()
	{
		return mPair.first();
	}

	const D& getDeleter() const
	{
		return mPair.first();
	}

private:

	CompressedPair<D, pointer> mPair;

};

template <typename T, typename D, bool = STD is_move_constructible_v<D>, bool = STD is_move_assignable_v<D>>
struct UniqPtrData : UniqPtrDataImpl<T, D>
{
	using UniqPtrDataImpl<T, D>::UniqPtrDataImpl;

	UniqPtrData(UniqPtrData&&) = default;

	UniqPtrData& operator=(UniqPtrData&&) = default;
};

template <typename T, typename D>
struct UniqPtrData<T, D, false, true> : UniqPtrDataImpl<T, D>
{
	using UniqPtrDataImpl<T, D>::UniqPtrDataImpl;

	UniqPtrData(UniqPtrData&&) = delete;

	UniqPtrData& operator=(UniqPtrData&&) = default;
};

template <typename T, typename D>
struct UniqPtrData<T, D, false, false> : UniqPtrDataImpl<T, D>
{
	using UniqPtrDataImpl<T, D>::UniqPtrDataImpl;

	UniqPtrData(UniqPtrData&&) = delete;

	UniqPtrData& operator=(UniqPtrData&&) = delete;
};

template <typename T, typename D = DefaultDelete<T>>
class UniquePtr
{
private:

	template <typename DelT>
	using DeleterConstraint = typename UniqPtrDataImpl<T, DelT>::DeleterConstraint::type;

	UniqPtrData<T, D> ptrData;

public:

	using pointer = typename UniqPtrDataImpl<T, D>::pointer;
	using element_type = T;
	using deleter_type = D;

private:

	template <typename ValT, typename DelT>
	using safe_conversion_valt =
		STD conjunction<
		STD is_convertible<typename UniquePtr<ValT, DelT>::pointer, pointer>, // Can convert to pointer.
		STD negation<STD is_array<ValT>> // Is not an array.
		>;

public:

	template <typename Del = D, typename = DeleterConstraint<Del>>
	constexpr UniquePtr() noexcept
		: ptrData()
	{ }

	template <typename Del = D, typename = DeleterConstraint<Del>>
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

	template <typename Del = deleter_type, typename DelUnref = typename STD remove_reference_t<Del>>
	UniquePtr(pointer, STD enable_if_t<STD is_lvalue_reference_v<Del>, DelUnref&&>) = delete;

	template <typename ValT, typename DelT, typename = require<
		safe_conversion_valt<ValT, DelT>,
		STD conditional_t<STD is_reference_v<D>,
		STD is_same<DelT, D>,
		STD is_convertible<DelT, D>>>>
		UniquePtr(UniquePtr<ValT, DelT>&& other) noexcept
		: ptrData(other.release(), STD forward<DelT>(other.get_deleter()))
	{ }

	template <typename Del = D, typename = DeleterConstraint<Del>>
	constexpr UniquePtr(STD nullptr_t) noexcept
		: ptrData()
	{ }

	UniquePtr(const UniquePtr&) = delete;

	~UniquePtr() noexcept
	{
		static_assert(STD is_invocable_v<deleter_type&, pointer>, "unique_ptr's deleter must be invocable with a pointer");
		auto& ptr = ptrData.getPtr();
		if (ptr)
		{
			get_deleter()(STD move(ptr));
		}

		ptr = pointer();
	}

	UniquePtr& operator=(UniquePtr&& other) = default;

	template <typename ValT, typename DelT>
	STD enable_if_t<
		STD conjunction_v<
			safe_conversion_valt<ValT, DelT>,
			STD is_assignable<deleter_type&, DelT&&>
		>,
		UniquePtr&
	>
		operator=(UniquePtr<ValT, DelT>&& other) noexcept
	{
		reset(other.release());
		get_deleter() = STD forward<DelT>(other.get_deleter());
		return *this;
	}

	UniquePtr& operator=(STD nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	UniquePtr& operator=(const UniquePtr&) = delete;

	STD add_lvalue_reference_t<element_type> operator*() const
	{
		return *get();
	}

	pointer operator->() const noexcept
	{
		return get();
	}

	pointer get() const noexcept
	{
		return ptrData.getPtr();
	}

	const deleter_type& get_deleter() noexcept
	{
		return ptrData.getDeleter();
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
};

template <typename T, typename D>
class UniquePtr<T[], D>
{
private:

	template <typename U>
	using DeleterConstraint = typename UniqPtrDataImpl<T, U>::DeleterConstraint::type;

	template <typename U>
	using IsDerivedT = STD conjunction<STD is_base_of<T, U>, STD negation<STD is_same<STD remove_cv_t<T>, STD remove_cv_t<U>>>>;


public:

	using pointer = typename UniqPtrDataImpl<T, D>::pointer;
	using element_type = T;
	using deleter_type = D;

	template <typename Del = D, typename = DeleterConstraint<Del>>
	constexpr UniquePtr() noexcept
		: ptrData()
	{ }

	~UniquePtr()
	{
		auto& ptr = ptrData.getPtr();
		if (ptr)
		{
			get_deleter()(ptr);
			ptr = pointer();
		}
	}

	UniquePtr& operator=(UniquePtr&&) = default;


	// Disable copy from lvalue.
	UniquePtr(const UniquePtr&) = delete;

	UniquePtr& operator=(const UniquePtr&) = delete;

	STD add_lvalue_reference_t<element_type> operator[](STD size_t n) const
	{
		return get()[n];
	}

	pointer get() const noexcept
	{
		return ptrData.getPtr();
	}

	deleter_type& get_deleter() noexcept
	{
		return ptrData.getDeleter();
	}

	const deleter_type& get_deleter() const noexcept
	{
		return ptrData.getDeleter();
	}

	explicit operator bool() const noexcept
	{
		return get() != pointer();
	}

	pointer release() noexcept
	{
		return ptrData.release();
	}

	void reset(STD nullptr_t) noexcept
	{

	}

	void swap(UniquePtr& other) noexcept
	{
		static_assert(STD is_swappable_v<D>, "deleter must be swappable");
		ptrData.swap(other.ptrData);
	}

private:

	UniqPtrData<T, D> ptrData;
};




template <typename T>
struct MakeUniq
{
	using SingleObject = UniquePtr<T>;
};

template <typename T>
struct MakeUniq<T[]>
{
	using Array = UniquePtr<T[]>;
};

template <typename T, STD size_t Bound>
struct MakeUniq<T[Bound]>
{
	struct InvalidType { };
};

template <typename T, typename... Args>
inline typename MakeUniq<T>::SingleObject MakeUnique(Args&&... args)
{
	return UniquePtr<T>(new T(STD forward<Args>(args)...));
}

template <typename T>
inline typename MakeUniq<T>::Array MakeUnique(STD size_t n)
{
	return UniquePtr<T>(new STD remove_extent_t<T>[n]());
}

template <typename T, typename... Args>
inline typename MakeUniq<T>::InvalidType MakeUnique(Args&&... args) = delete;

JSTD_END

#endif // !UNIQUE_POINTER
