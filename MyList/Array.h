#pragma once
#ifndef ARRAY
#define ARRAY

#include <utility>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>

#include "Healper.h"

JSTD_START

template <typename T, STD size_t NumberOfData>
struct ArrayTraits
{
    using ArrayType[NumberOfData] = T;
    using Is_Swappable = STD is_swappable<T> ;
    using Is_Nothrow_Swappable = STD is_nothrow_swappable<T>;

    static constexpr bool is_nothrow_swappable_v()
    {
        return STD is_nothrow_swappable_v<T>;
    }

    static constexpr bool is_swappable_v()
    {
        return STD is_swappable_v<T>;
    }

    static constexpr T& getRef(const ArrayType& array, STD size_t n) noexcept
    {
        return const_cast<T&>(array[n]);
    }

    static constexpr T* getPtr(const ArrayType& array) noexcept
    {
        return const_cast<T*>(array);
    }
};

template <typename T>
struct ArrayTraits<T, 0>
{
    struct ArrayType { };
    typedef STD true_type Is_Swappable;
    typedef STD true_type Is_Nothrow_Swappable;

    static constexpr bool is_nothrow_swappable_v()
    {
        return true;
    }

    static constexpr bool is_swappable_v()
    {
        return true;
    }

    static constexpr T& getRef(const ArrayType&, STD size_t) noexcept
    {
        return *static_cast<T*>(nullptr);
    }

    static constexpr T* getPtr(const ArrayType&) noexcept
    {
        return nullptr;
    }
};

template <typename T, STD size_t Size>
class Array
{
public:

    using value_type = T;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef STD size_t                    	      size_type;
    typedef STD ptrdiff_t                   	      difference_type;
    typedef STD reverse_iterator<iterator>	      reverse_iterator;
    typedef STD reverse_iterator<const_iterator>   const_reverse_iterator;

    using Traits = ArrayTraits<T, Size>;

    Array() = default;

    ~Array() = default;

    constexpr void fill(const value_type& n)
    {
        STD fill_n();
    }



private:

    typename Traits::ArrayType mArray;

};

JSTD_END

#endif // !ARRAY
