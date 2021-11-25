#pragma once
#ifndef INITIALIZER_LIST
#define INITIALIZER_LIST

#include <cstddef>

#include "Healper.h"

JSTD_START

template <typename T>
class InitializerList 
{
public:
    using value_type = T;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = size_t;

    using iterator = const T*;
    using const_iterator = const T*;

    constexpr InitializerList() noexcept 
        : first(), last() 
    { }

    constexpr InitializerList(const T* firstArg, const T* lastArg) noexcept
        : first(firstArg), last(lastArg) 
    { }

    NODISCARD constexpr const T* begin() const noexcept 
    {
        return first;
    }

    NODISCARD constexpr const T* end() const noexcept 
    {
        return last;
    }

    NODISCARD constexpr size_type size() const noexcept 
    {
        return static_cast<size_type>(last - first);
    }

private:
    const value_type* first;
    const value_type* last;
};

template <typename T>
NODISCARD constexpr const T* begin(InitializerList<T> ilist) noexcept 
{
    return ilist.begin();
}

template <typename T>
NODISCARD constexpr const T* end(InitializerList<T> ilist) noexcept
{
    return ilist.end();
}

JSTD_END

#endif // INITIALIZER_LIST
