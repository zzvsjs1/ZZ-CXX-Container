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
#include "Healper.h"

JSTD_START

template <typename T, typename Alloc>
class VectorBase
{
public:

	using T_Alloc_Type = typename MyAlloctTraits<Alloc>:: template rebind<T>::other;
	using pointer = typename MyAlloctTraits<T_Alloc_Type>::pointer;

	class VectorData
	{
	public:
		pointer mStart;
		pointer mLast;
		pointer mEnd;


	};

	class VectorImpl : public T_Alloc_Type, public VectorData
	{
	public:
		
	};
	
};





JSTD_END

#endif // !VECTOR