#pragma once
#ifndef ARRAY
#define ARRAY

#include <utility>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <stdexcept>

#include "Healper.h"

JSTD_START

template <typename T, STD size_t N>
struct ArrayTraits
{
    using ArrayType = T[N];
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
    using Is_Swappable = STD true_type;
    using Is_Nothrow_Swappable = STD true_type;

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

template <typename T, STD size_t N>
class Array
{
public:

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = value_type*;
    using size_type = STD size_t;
    using difference_type = STD ptrdiff_t;
    using reverse_iterator = STD reverse_iterator<iterator>;
    using const_reverse_iterator = STD reverse_iterator<const_iterator>;

    using Traits = ArrayTraits<T, N>;

    Array() = default;

    ~Array() = default;

    constexpr void fill(const value_type& value)
    {
        STD fill_n(begin(), size(), value);
    }

    constexpr void swap(Array& other) noexcept(Traits::is_nothrow_swappable_v())
    {
        STD swap_ranges(begin(), end(), other.begin());
    }

    constexpr iterator begin() noexcept
    {
        return iterator(data());
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator(data());
    }

    constexpr iterator end() noexcept
    {
        return iterator(data() + N);
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator(data() + N);
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(data());
    }

    constexpr const_iterator cend() const noexcept
    {
        return const_iterator(data() + N);
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr size_type size() const noexcept 
    { 
        return N; 
    }

    constexpr size_type max_size() const noexcept 
    { 
        return N;
    }

    NODISCARD constexpr bool empty() const noexcept 
    { 
        return size() == 0; 
    }

    constexpr reference operator[](const size_type n) noexcept
    {
        return Traits::getRef(mArray, n);
    }

    constexpr const_reference operator[](const size_type n) const noexcept
    {
        return Traits::getRef(mArray, n);
    }

    constexpr reference at(const size_type n) noexcept
    {
        if (n > N)
        {
            throw STD out_of_range("Out of range.");
        }

        return Traits::getRef(mArray, n);
    }

    constexpr const_reference at(const size_type n) const noexcept
    {
        if (n > N)
        {
            throw STD out_of_range("Out of range.");
        }

        return Traits::getRef(mArray, n);
    }

    constexpr reference fornt() noexcept
    {
        return *begin();
    }

    constexpr const_reference fornt() const noexcept
    {
        return *begin();
    }

    constexpr reference back() noexcept
    {
        return N ? Traits::getRef(mArray, N - 1) : Traits::getRef(mArray, 0);
    }

    constexpr const_reference back() const noexcept
    {
        return N ? Traits::getRef(mArray, N - 1) : Traits::getRef(mArray, 0);
    }

    constexpr pointer data() noexcept
    {
        return Traits::getPtr(mArray);
    }

    constexpr const_pointer data() const noexcept
    {
        return Traits::getPtr(mArray);
    }

private:

    typename Traits::ArrayType mArray;

};

template <typename T, typename... U>
Array(T, U...) -> Array<STD enable_if_t<(STD is_same_v<T, U>&& ...), T>, 1 + sizeof...(U)>;

template <typename T, STD size_t N>
inline bool operator==(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return STD equal(left.begin(), left.end(), right.begin());
}

template <typename T, STD size_t N>
inline bool operator!=(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return !(left == right);
}

template <typename T, STD size_t N>
inline bool operator<(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return STD lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
}

template <typename T, STD size_t N>
inline bool operator>(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return right < left;
}

template <typename T, STD size_t N>
inline bool operator<=(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return !(left > right);
}

template <typename T, STD size_t N>
inline bool operator>=(const Array<T, N>& left, const Array<T, N>& right) noexcept
{
    return !(left < right);
}

template <typename T, STD size_t N>
inline STD enable_if_t<ArrayTraits<T, N>::is_swappable_v()>
swap(Array<T, N>& left, Array<T, N>& right) noexcept(noexcept(left.swap(right)))
{
    left.swap(right);
}

template <typename T, STD size_t N>
inline STD enable_if_t<!ArrayTraits<T, N>::is_swappable_v()>
swap(Array<T, N>& left, Array<T, N>& right) = delete;

template<std::size_t Interger, typename T, STD size_t N>
constexpr T&
get(Array<T, N>& arr) noexcept
{
    static_assert(Interger < N, "array index is within bounds");
    return ArrayTraits<T, N>::getRef(arr.mArray, Interger);
}

template<std::size_t Interger, typename T, STD size_t N>
constexpr T&&
get(Array<T, N>&& arr) noexcept
{
    static_assert(Interger < N, "array index is within bounds");
    return STD move(STD get<Interger>(arr));
}

template<std::size_t Interger, typename T, STD size_t N>
constexpr const T&
get(const Array<T, N>& arr) noexcept
{
    static_assert(Interger < N, "array index is within bounds");
    return ArrayTraits<T, N>::getRef(arr._M_elems, Interger);
}

template<std::size_t Interger, typename T, STD size_t N>
constexpr const T&&
get(const Array<T, N>&& arr) noexcept
{
    static_assert(Interger < N, "array index is within bounds");
    return STD move(STD get<Interger>(arr));
}

template <typename T>
struct tuple_size;

// Partial specialization for Array
template <typename T, STD size_t N>
struct tuple_size<Array<T, N>>
    : public STD integral_constant<STD size_t, N> 
{ };

// tuple_element
template <STD size_t Int, typename T>
struct tuple_element;

// Partial specialization for Array
template <STD size_t Int, typename T, STD size_t N>
struct tuple_element<Int, Array<T, N>>
{
    static_assert(Int < N, "index is out of bounds");
    typedef T type;
};

JSTD_END

#endif // !ARRAY
