#pragma once
#ifndef INITIALIZER_LIST
#define INITIALIZER_LIST

#include <initializer_list>
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
    using size_type = STD size_t;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = const T*;
    using const_iterator = const T*;

    constexpr InitializerList() noexcept 
        : first(), last() 
    { }

    constexpr InitializerList(const pointer firstArg, const pointer lastArg) noexcept
        : first(firstArg), last(lastArg) 
    { }

    constexpr InitializerList(const STD initializer_list<value_type>& ilist) noexcept
        : first(STD addressof(*ilist.begin())), last(STD addressof(*ilist.end()))
    { }

    NODISCARD constexpr const pointer begin() const noexcept
    {
        return first;
    }

    NODISCARD constexpr const pointer end() const noexcept
    {
        return last;
    }

    NODISCARD constexpr size_type size() const noexcept 
    {
        return static_cast<size_type>(last - first);
    }

private:
    const pointer first;
    const pointer last;
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
