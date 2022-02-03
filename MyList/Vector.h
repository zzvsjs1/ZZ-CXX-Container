#pragma once
#ifndef VECTOR
#define VECTOR

#include <algorithm>
#include <cassert>
#include <memory>
#include <iterator>
#include <type_traits>
#include <initializer_list>
#include <stdexcept>

#include "Config.h"
#include "MyIterator.h"
#include "Healper.h"

JSTD_START

template <typename T, typename Alloc>
class VectorBase
{
public:

	using T_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<T>::other;
	using pointer = typename MyAlloctTraits<T_Alloc_Type>::pointer;

	class VectorImplData
	{
	public:
		pointer mStart;
		pointer mLast;
		pointer mEnd;

		VectorImplData() JLIBCXX_NOEXCEPT
			: mStart(), mLast(), mEnd()
		{ }

		VectorImplData(VectorImplData&& other) JLIBCXX_NOEXCEPT
			: mStart(other.mStart), mLast(other.mLast), mEnd(other.mEnd)
		{
			other.mStart = pointer();
			other.mLast = pointer();
			other.mEnd = pointer();
		}

		void copyData(const VectorImplData& other) JLIBCXX_NOEXCEPT
		{
			mStart = other.mStart;
			mLast = other.mLast;
			mEnd = other.mEnd;
		}

		void swapData(VectorImplData& other) JLIBCXX_NOEXCEPT
		{
			VectorImplData temp;
			temp.copyData(*this);
			copyData(other);
			other.copyData(temp);
		}
	};

	class VectorImpl : public T_Alloc_Type, public VectorImplData
	{
	public:

		VectorImpl() JLIBCXX_NOEXCEPT_IF(STD is_nothrow_default_constructible_v<T_Alloc_Type>)
			: T_Alloc_Type()
		{ }

		VectorImpl(const T_Alloc_Type& other) JLIBCXX_NOEXCEPT
			: T_Alloc_Type(other)
		{ }

		VectorImpl(VectorImpl&& other) JLIBCXX_NOEXCEPT
			: T_Alloc_Type(STD move(other)), VectorImplData(STD move(other))
		{ }

		VectorImpl(T_Alloc_Type&& other) JLIBCXX_NOEXCEPT
			: T_Alloc_Type(STD move(other))
		{ }

		VectorImpl(T_Alloc_Type&& a, VectorImpl&& b) JLIBCXX_NOEXCEPT
			: T_Alloc_Type(STD move(a)), VectorImplData(STD move(b))
		{ }
	};

	VectorImpl mImpl;

	using allocator_type = Alloc;

	T_Alloc_Type& getTAllocator() JLIBCXX_NOEXCEPT
	{
		return mImpl;
	}

	NODISCARD const T_Alloc_Type& getTAllocator() const JLIBCXX_NOEXCEPT
	{
		return mImpl;
	}

	NODISCARD allocator_type get_allocator() const JLIBCXX_NOEXCEPT
	{
		return allocator_type(getTAllocator());
	}

	VectorBase() = default;

	VectorBase(const allocator_type& alloc) JLIBCXX_NOEXCEPT
		: mImpl(alloc)
	{ }

	VectorBase(const STD size_t n)
		: mImpl()
	{
		createStorage(n);
	}

	VectorBase(const STD size_t n, const allocator_type& alloc)
		: mImpl(alloc)
	{
		createStorage(n);
	}

	VectorBase(T_Alloc_Type&& alloc) JLIBCXX_NOEXCEPT
		: mImpl(STD move(alloc))
	{ }

	VectorBase(VectorBase&& other, const allocator_type& alloc)
		: mImpl(alloc)
	{
		if (alloc == other.get_allocator())
		{
			mImpl.swapData(other.mImpl);
		}
		else
		{
			createStorage(other.mImpl.mEnd - other.mImpl.mStart);
		}
	}

	VectorBase(const allocator_type& alloc, VectorBase&& other)
		: mImpl(T_Alloc_Type(alloc), STD move(other.mImpl))
	{ }

	~VectorBase() JLIBCXX_NOEXCEPT
	{
		myDestroy(mImpl.mStart, mImpl.mLast, getTAllocator());
		deallocateArray(mImpl.mStart, mImpl.mEnd - mImpl.mStart);
	}

	pointer allocateArray(STD size_t n)
	{
		return  n != 0 ? MyAlloctTraits<T_Alloc_Type>::allocate(mImpl, n) : pointer();
	}

	void deallocateArray(pointer ptr, STD size_t n)
	{
		if (ptr)
		{
			MyAlloctTraits<T_Alloc_Type>::deallocate(mImpl, ptr, n);
		}
	}


protected:

	void createStorage(STD size_t n)
	{
		mImpl.mStart = allocateArray(n);
		mImpl.mEnd = mImpl.mStart;
		mImpl.mEnd = mImpl.mStart + n;
	}
};

template <typename T, typename Alloc = STD allocator<T>>
class Vector : protected VectorBase<T, Alloc>
{
private:

	static_assert(STD is_same_v<STD remove_cv_t<T>, T>, "jstd::vector must have a non-const, non-volatile value_type");
	static_assert(STD is_same_v<typename Alloc::value_type, T>, "jstd::vector must have the same value_type as its allocator");

	using Base = VectorBase<T, Alloc>;
	using T_Alloc_Type = typename Base::T_Alloc_Type;
	using Alloc_Traits = MyAlloctTraits<T_Alloc_Type>;

public:

	using value_type = T;
	using pointer = typename Base::pointer;
	using const_pointer = typename Alloc_Traits::const_pointer;
	using reference = typename Alloc_Traits::reference;
	using const_reference = typename Alloc_Traits::const_reference;
	using iterator = NormalIterator<pointer, Vector>;
	using const_iterator = NormalIterator<const_pointer, Vector>;
	using reverse_iterator = STD reverse_iterator<iterator>;
	using const_reverse_iterator = STD reverse_iterator<const_iterator>;
	using size_type = typename Alloc_Traits::size_type;
	using difference_type = typename Alloc_Traits::difference_type;
	using allocator_type = Alloc;

private:

	static size_type maxSize(const T_Alloc_Type& alloc) noexcept
	{
		const STD size_t diffMax = STD numeric_limits<STD ptrdiff_t>::max() / sizeof(T);
		const STD size_t allocMax = Alloc_Traits::max_size(alloc);
		return STD min(diffMax, allocMax);
	}

	[[noreturn]] static void throwLengthError(const char* str)
	{
		throw STD runtime_error("cannot create jstd::vector larger than max_size()");
	}

	static size_type checkLength(size_type n, const allocator_type& alloc)
	{
		if (n > maxSize(T_Alloc_Type(alloc)))
		{
			throwLengthError("cannot create jstd::vector larger than max_size()");
		}

		return n;
	}

	template <typename... Args>
	void initializeNValues(size_type n, Args... args)
	{
		auto first = this->mImpl.mStart;
		auto alloc = this->getTAllocator();
		using traits = MyAlloctTraits<decltype(alloc)>;

		TRY_START
		for (; n; --n, ++first)
		{
			traits::construct(alloc, STD addressof(*first), STD forward<Args>(args)...);
		}

		this->mImpl.mLast = first - 1;
		CATCH_ALL
		myDestroy(this->mImpl.mStart, first, alloc);
		THROW_AGAIN
		END_CATCH
	}

protected:

	using Base::allocateArray;
	using Base::deallocateArray;
	using Base::mImpl;
	using Base::getTAllocator;

public:
	/**
	 * \brief Create a Vector with no elements.
	 */
	Vector() = default;

	explicit Vector(const allocator_type& alloc) JLIBCXX_NOEXCEPT
		: Base(alloc)
	{ }

	explicit Vector(size_type n, const allocator_type& alloc = allocator_type())
		: Base(checkLength(n, alloc), alloc)
	{
		initializeNValues(n);
	}

	Vector(size_type n, const value_type& value, const allocator_type& alloc = allocator_type())
		: Base(checkLength(n, alloc), alloc)
	{
		initializeNValues(n, value);
	}











	~Vector() = default;

};



JSTD_END

#endif // !VECTOR