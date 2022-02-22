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
#include <string_view>

#include "Config.h"
#include "MyIterator.h"
#include "Healper.h"
#include "Utility.h"

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

		void copyPointerFrom(const VectorImplData& other) JLIBCXX_NOEXCEPT
		{
			mStart = other.mStart;
			mLast = other.mLast;
			mEnd = other.mEnd;
		}

		void swapData(VectorImplData& other) JLIBCXX_NOEXCEPT
		{
			VectorImplData temp;
			temp.copyPointerFrom(*this);
			copyPointerFrom(other);
			other.copyPointerFrom(temp);
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
		deallocateWholeArray();
	}

	pointer allocateArray(STD size_t n)
	{
		return  n != 0 ? MyAlloctTraits<T_Alloc_Type>::allocate(mImpl, n) : pointer{};
	}

	void deallocateArray(pointer ptr, STD size_t n) JLIBCXX_NOEXCEPT
	{
		if (ptr)
		{
			MyAlloctTraits<T_Alloc_Type>::deallocate(mImpl, ptr, n);
		}
	}

	void deallocateWholeArray() JLIBCXX_NOEXCEPT
	{
		deallocateArray(mImpl.mStart, mImpl.mEnd - mImpl.mStart);
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

	friend ClearGuard<Vector>;

	static_assert(STD is_same_v<STD remove_cv_t<T>, T>, 
		"jstd::vector must have a non-const, non-volatile value_type.");
	static_assert(STD is_same_v<typename Alloc::value_type, T>,
		"jstd::vector must have the same value_type as its allocator.");

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
		constexpr STD size_t diffMax = STD numeric_limits<STD ptrdiff_t>::max() / sizeof(T);
		const STD size_t allocMax = Alloc_Traits::max_size(alloc);
		return STD min(diffMax, allocMax);
	}

	[[noreturn]] static void throwLengthError(const char* str)
	{
		throw STD runtime_error(str);
	}

	NODISCARD static size_type checkLength(size_type n, const allocator_type& alloc)
	{
		if (n > maxSize(T_Alloc_Type(alloc)))
		{
			throwLengthError("cannot create jstd::vector larger than max_size()");
		}

		return n;
	}

	template <typename... Args>
	void initializeNValues(size_type n, Args&&... args)
	{
		auto current = this->mImpl.mStart;

		TRY_START
		for (; n; --n, ++current)
		{
			Alloc_Traits::construct(this->mImpl, STD addressof(*current), STD forward<Args>(args)...);
		}

		this->mImpl.mLast = current;
		CATCH_ALL
		myDestroy(this->mImpl.mStart, current, this->mImpl);
		THROW_AGAIN
		END_CATCH
	}

	template <typename InputIterator>
	void copyNValuesFromRanges(InputIterator first, InputIterator last)
	{
		InputIterator current = this->mImpl.mStart;
		TRY_START
		for (; first != last; ++current, ++first)
		{
			Alloc_Traits::construct(this->mImpl, STD addressof(*current), *first);
		}

		this->mImpl.mLast = current;
		CATCH_ALL
		myDestroy(this->mImpl.mStart, current, this->mImpl);
		THROW_AGAIN
		END_CATCH
	}

	void eraseToEnd(pointer pos) JLIBCXX_NOEXCEPT
	{
		myDestroy(pos, this->mImpl.mLast, this->getTAllocator());
		this->mImpl.mLast = pos;
	}

	void rangeCheck(size_type n) const
	{
		if (n >= size())
		{
			throw STD logic_error(myFormat(
				"Vector::rangeCheck: n "
				"(which is %zu) >= size() "
				"(which is %zu)", 
				n, 
				size()));
		}
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
	Vector() JLIBCXX_NOEXCEPT = default;

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

	Vector(const Vector& other)
		: Base(other.size(), Alloc_Traits::select_on_container_copy_construction(other.getTAllocator()))
	{
		copyNValuesFromRanges(other.cbegin(), other.cend());
	}

	Vector(Vector&&) noexcept = default;

	Vector(const Vector& other, const allocator_type& alloc)
		: Base(other.size(), alloc)
	{
		copyNValuesFromRanges(other.cbegin(), other.cend());
	}

	Vector(
		STD initializer_list<value_type> ilist, 
		const allocator_type& alloc = allocator_type())
		: Base(ilist.size(), alloc)
	{
		copyNValuesFromRanges(ilist.begin(), ilist.end());
	}

private:

	template <typename InputIterator>
	void doRangeInitialize(InputIterator first, InputIterator last, STD input_iterator_tag)
	{
		ClearGuard guard{ this };

		for (; first != last; ++first)
		{
			// emplace_back(*first);
		}
	}

	template <typename InputIterator>
	void doRangeInitialize(InputIterator first, InputIterator last, STD forward_iterator_tag)
	{
		this->createStorage(checkLength(STD distance(first, last), getTAllocator()));
		copyNValuesFromRanges(first, last);
	}

public:

	template <typename InputIterator,
		typename = RequireInputIter<InputIterator>>
		Vector(
			InputIterator first, 
			InputIterator last, 
			const allocator_type& alloc = allocator_type())
		: Base(alloc)
	{
		doRangeInitialize(first, last, STD iterator_traits<InputIterator>::iterator_category);
	}

	~Vector() JLIBCXX_NOEXCEPT
	{
		clear();
	}

private:

	template <typename InputIterator>
	void assignRange(InputIterator first, InputIterator last, STD input_iterator_tag)
	{



	}

	template <typename InputIterator>
	void assignRange(InputIterator first, InputIterator last, STD forward_iterator_tag)
	{
		const auto length = myDistance(first, last);


	}




public:

	constexpr void assign(size_type count, const_reference value)
	{
		// Reallocate and swap.
		if (count > capacity())
		{
			Vector temp(count, value);
			temp.mImpl.swapData(mImpl);
			return;
		}

		// Fill already constructed position and construct uninitialized rest.
		if (count > size())
		{
			fillRanges(begin(), end(), value);
			const size_type needToAdd = count - size();
			mImpl.mLast = uninitializedFillRanges(mImpl.mLast, needToAdd, value, getTAllocator());
			return;
		}

		// Within the range. We fill the range and erase rest of that space if needed.
		eraseToEnd(fillN(mImpl.mStart, count, value));
	}

	template <typename InputIt, typename = RequireInputIter<InputIt>>
	constexpr void assign(InputIt first, InputIt last)
	{
		// We need to check the iterator category first....
		assignRange(first, last, typename STD iterator_traits<InputIt>::iterator_category{});
	}

	constexpr void assign(STD initializer_list<value_type> ilist)
	{
		assign(ilist.begin(), ilist.end());
	}

	using Base::get_allocator;

	NODISCARD iterator begin() JLIBCXX_NOEXCEPT
	{
		return iterator(this->mImpl.mStart);
	}

	NODISCARD const_iterator begin() const JLIBCXX_NOEXCEPT
	{
		return const_iterator(this->mImpl.mStart);
	}

	NODISCARD iterator end() JLIBCXX_NOEXCEPT
	{
		return iterator(this->mImpl.mLast);
	}

	NODISCARD const_iterator end() const JLIBCXX_NOEXCEPT
	{
		return const_iterator(this->mImpl.mLast);
	}

	NODISCARD reverse_iterator rbegin() JLIBCXX_NOEXCEPT
	{
		return reverse_iterator(end());
	}

	NODISCARD const_reverse_iterator rbegin() const JLIBCXX_NOEXCEPT
	{
		return const_reverse_iterator(end());
	}

	NODISCARD reverse_iterator rend() JLIBCXX_NOEXCEPT
	{
		return reverse_iterator(begin());
	}

	NODISCARD const_reverse_iterator rend() const JLIBCXX_NOEXCEPT
	{
		return const_reverse_iterator(begin());
	}

	NODISCARD const_iterator cbegin() const JLIBCXX_NOEXCEPT
	{
		return const_iterator(this->mImpl.mStart);
	}

	NODISCARD const_iterator cend() const JLIBCXX_NOEXCEPT
	{
		return const_iterator(this->mImpl.mLast);
	}

	NODISCARD const_reverse_iterator crbegin() const JLIBCXX_NOEXCEPT
	{
		return const_reverse_iterator(end());
	}

	NODISCARD const_reverse_iterator crend() const JLIBCXX_NOEXCEPT
	{
		return const_reverse_iterator(begin());
	}

	NODISCARD size_type size() const JLIBCXX_NOEXCEPT
	{
		return static_cast<size_type>(this->mImpl.mLast - this->mImpl.mStart);
	}

	NODISCARD size_type max_size() const JLIBCXX_NOEXCEPT
	{
		return maxSize(this->getTAllocator());
	}

	void resize(const size_type newSize)
	{
		
	}

	void shrink_to_fit()
	{
		if (size() == capacity())
		{
			return;
		}
		
		Vector(makeMoveIfNoexceptIterator(begin()),
			makeMoveIfNoexceptIterator(end()), 
			get_allocator())
		.swap(*this);
	}

	NODISCARD size_type capacity() const JLIBCXX_NOEXCEPT
	{
		return size_type(this->mImpl.mEnd - this->mImpl.mStart);
	}

	NODISCARD bool empty() const JLIBCXX_NOEXCEPT
	{
		return this->mImpl.mStart == this->mImpl.mLast;
	}

	void reserve(const size_type n)
	{
		
	}

	NODISCARD reference operator[](size_type n) JLIBCXX_NOEXCEPT
	{
		return *(this->mImpl.mStart + n);
	}

	NODISCARD const_reference operator[](size_type n) const JLIBCXX_NOEXCEPT
	{
		return *(this->mImpl.mStart + n);
	}

	NODISCARD reference at(size_type n)
	{
		rangeCheck(n);
		return this->operator[](n);
	}

	NODISCARD const_reference at(size_type n) const
	{
		rangeCheck(n);
		return this->operator[](n);
	}

	NODISCARD reference front() JLIBCXX_NOEXCEPT
	{
		return *begin();
	}

	NODISCARD const_reference front() const JLIBCXX_NOEXCEPT
	{
		return *begin();
	}

	NODISCARD reference back() JLIBCXX_NOEXCEPT
	{
		return *(end() - 1);
	}

	NODISCARD const_reference back() const JLIBCXX_NOEXCEPT
	{
		return *(end() - 1);
	}

private:

	template <typename Pointer>
	NODISCARD
	typename STD pointer_traits<Pointer>::element_type*
	getDataPointer(Pointer pointer) const JLIBCXX_NOEXCEPT
	{
		return empty() ? nullptr : STD to_address(pointer);
	}


public:


	NODISCARD pointer data() JLIBCXX_NOEXCEPT
	{
		return getDataPointer(this->mImpl.mStart);
	}

	NODISCARD const_pointer data() const JLIBCXX_NOEXCEPT
	{
		return getDataPointer(this->mImpl.mStart);
	}

	void clear() JLIBCXX_NOEXCEPT
	{
		eraseToEnd(this->mImpl.mStart);
	}

private:

	size_type checkLengthByAndDisplayStr(const size_type n, const char* string) const
	{
		if (n > max_size() - size())
		{
			throwLengthError(string);
		}

		const auto newLen = size() + STD max(size(), n);
		return newLen < size() || newLen > max_size() ? max_size() : newLen;
	}

	// TODO:
	template <typename... Args>
	void reallocateInsert(iterator pos, Args&&... args)
	{
		const auto len = checkLengthByAndDisplayStr(static_cast<size_type>(1), "Vector::reallocateInsert");
		auto oldStart = this->mImpl.mStart;
		auto oldLast = this->mImpl.mLast;
		const auto elementBefore = pos - begin();
		pointer newStart = this->allocateArray(len);
		pointer newLast = newStart;

		TRY_START

		Alloc_Traits::construct(getTAllocator(), newStart + elementBefore, STD forward<Args>(args)...);
		newLast = pointer();



		CATCH_ALL

		if (!newLast)
		{
			Alloc_Traits::destroy(getTAllocator(), newStart + elementBefore);
		}
		else
		{
			myDestroy(newStart, newLast, getTAllocator());
		}

		this->deallocateArray(newStart, len);

		THROW_AGAIN
		END_CATCH


	}

public:

	void push_back(const_reference value)
	{
		emplace_back(value);
	}

	void push_back(value_type&& value)
	{
		emplace_back(STD move(value));
	}

	template <typename... Args>
	reference emplace_back(Args&&... args)
	{
		if (mImpl.mLast != mImpl.mEnd)
		{
			Alloc_Traits::construct(getTAllocator(), mImpl.mLast, STD forward<Args>(args)...);
			++mImpl.mLast;
		}
		else
		{
			reallocateInsert(cend(), STD forward<Args>(args)...);
		}

		return back();
	}






	void swap(Vector& other) JLIBCXX_NOEXCEPT
	{
		mImpl.swapData(other.mImpl);
		Alloc_Traits::doSwap(getTAllocator(), other.getTAllocator());
	}





};

template<typename InputIterator, typename T
	= typename STD iterator_traits<InputIterator>::value_type,
	typename Allocator = STD allocator<T>,
	typename = RequireInputIter<InputIterator>,
	typename = RequireAllocator<Allocator>>
	Vector(InputIterator, InputIterator, Allocator = Allocator())
->Vector<T, Allocator>;

JSTD_END

#endif // !VECTOR