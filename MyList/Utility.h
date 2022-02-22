#pragma once
#ifndef UTILITY
#define UTILITY

#include <cstddef>
#include <memory>
#include <string>
#include <stdexcept>

#include "Config.h"

JSTD_START

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

template <typename T>
static auto forwardStringToConstCharPtr(T&& value) noexcept
{
	if constexpr (STD is_same_v<STD remove_cv_t<T>, STD string>)
	{
		return STD forward<T>(value).c_str();
	}

	return STD forward<T>(value);
}

template <typename... Args>
static STD string doStringFormat(const char* const fmt, Args&&... args)
{
	const auto size = STD snprintf(nullptr, 0, fmt, args...);
	if (size < 0)
	{
		throw STD runtime_error("Format failed");
	}

	const auto finalSize = static_cast<STD size_t>(size) + 1;

	const auto buffer = STD make_unique<char[]>(finalSize); // Throw
	const auto finalStr = STD snprintf(buffer.get(), finalSize, fmt, args...);
	if (finalStr < 0)
	{
		throw STD runtime_error("Format failed");
	}

	return buffer.get();
}

template <typename... Args>
STD string myFormat(const char* const fmt, Args&&... args)
{
	return doStringFormat(fmt, forwardStringToConstCharPtr(STD forward<Args>(args))...);
}

template <typename... Args>
STD string myFormat(STD string& fmt, Args&&... args)
{
	return myFormat(fmt.c_str(), forwardStringToConstCharPtr(STD forward<Args>(args))...);
}

template <typename Container>
struct NODISCARD ClearGuard
{
	Container* const mTarget;

	constexpr ~ClearGuard()
	{
		if (mTarget)
		{
			mTarget->clear();
		}
	}
};

template <class InputIterator>
static NODISCARD constexpr STD iter_difference_t<InputIterator> myDistance(
	InputIterator first,
	InputIterator last)
{
	if constexpr (
		STD is_convertible_v<
		typename STD iterator_traits<InputIterator>::iterator_category,
		STD random_access_iterator_tag>)
	{
		return last - first;
	}

	STD iter_difference_t<InputIterator> len = 0;
	for (; first != last; ++first, ++len) {}
	return len;
}

template <typename InputIterator, typename V>
constexpr static InputIterator fillN(InputIterator dest, STD size_t n, const V& value)
{
	if (n == 0)
	{
		return dest;
	}

	for (; n; --n, ++dest)
	{
		*dest = value;
	}

	return dest;
}

template <typename InputIterator, typename V>
constexpr static InputIterator fillRanges(InputIterator first, InputIterator last, const V& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}

	return first;
}

template <typename InputIterator, typename SizeT, typename V, typename Allocator>
constexpr static InputIterator uninitializedFillRanges(
	InputIterator dest,
	SizeT count,
	const V& value,
	Allocator& alloc)
{
	InputIterator current = dest;
	TRY_START
	for (; count; --count, ++current)
	{
		STD allocator_traits<Allocator>::construct(alloc, STD addressof(*current), value);
	}

	return current;
	CATCH_ALL
	myDestroy(dest, current, alloc);

	THROW_AGAIN
	END_CATCH
}

JSTD_END

#endif // !UTILITY