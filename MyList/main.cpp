#include <iostream>

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <forward_list>
#include <set>
#include <vector>

#include "MyList.h"
#include "Healper.h"

using std::cin;
using std::cout;
using std::end;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::forward_list;
using std::move;

using namespace jstd;

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const MyList<T>& list)
{
    for (const auto &i : list) 
	{
        ostr << i << " ";
    }

	ostr << endl;

    return ostr;
}

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const list<T>& list)
{
	for (const auto &i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const vector<T>& list)
{
	for (const auto &i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

struct MyStruct
{

	MyStruct(int i)
		: d(i)
	{}

	int d;
};

int main()
{
	MyList<MyStruct> b;

	b.emplace_back(10);

	sizeof(vector<int>);


	ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Type a;

	auto* v = ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::allocate(a, 1);

	//ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::construct(a, v, 5);



	return 0;
}