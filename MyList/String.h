#pragma once
#ifndef STRING
#define STRING

#include <atomic>
#include <algorithm>
#include <memory>
#include <initializer_list>
#include <string_view>
#include <ios>
#include <cwchar>

#include "Healper.h"

JSTD_START

template <typename _CharT>
struct CharTypes
{
	using int_type = unsigned long;
	using pos_type = STD streampos;
	using off_type = STD streamoff;
	using state_type = STD mbstate_t;
};


template <typename CharT>
class CharTraits
{
public:

	using char_type = CharT;
	using int_type = typename CharTypes<CharT>::int_type;

	CharTraits();
	~CharTraits();

private:

};

template <typename CharT, typename Traits, typename Alloc>
class BasicString
{
private:

	using Char_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<CharT>::other;
	using Alloc_Traits = MyAlloctTraits<Char_Alloc_Type>;

public:

	using traits_type = Traits;
	using value_type = typename Traits::char_type;
	using allocator_type = Char_Alloc_Type;
	using size_type = typename Alloc_Traits::size_type;

	
	static const size_type npos = static_cast<size_type>(-1);

	BasicString();
	~BasicString();

private:

};





JSTD_END

#endif // !STRING
