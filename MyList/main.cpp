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
#include "Memory.h"

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

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const MyList<T, Alloc>& list)
{
    for (const auto &i : list) 
	{
        ostr << i << " ";
    }

	ostr << endl;

    return ostr;
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const list<T, Alloc>& list)
{
	for (const auto &i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const vector<T, Alloc>& list)
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
		: d(i)	{ }

	int d;
};

int main()
{
	MyList<string, Allocator<string>> b = { "Herllo" };

	MyList<list<string>, Allocator<list<string>>> g = { {"a", "b"}, {"a", "b"} };

	MyList<list<string>, Allocator<list<string>>> k = { {"a", "b"}, {"a", "b"} };

	g = std::move(k);

	cout << g;

	/*ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Type a;

	auto* v = ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::allocate(a, 1);*/

	//ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::construct(a, v, 5);



	return 0;
}