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
#include <chrono>
#include <string>
#include <fstream>

#include "MyList.h"
#include "Healper.h"
#include "Memory.h"
#include "PritorityQueue.h"

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

template <typename T, typename Container, typename Compare>
std::ostream& operator<<(std::ostream& ostr, PriorityQueue<T, Container, Compare>& pq)
{
	for (decltype(pq.size()) size = pq.size(), start = 0; start < size; ++start)
	{
		ostr << pq.top() << " ";
		pq.pop();
	}

	ostr << endl;

	return ostr;
}

struct MyStruct
{

	MyStruct(int i)
		: d(i)	{ }

	~MyStruct() noexcept
	{
		cout << "Good Bye ";
	}

	int d;
};

int main()
{
	//MyList<string> a;
	//a.sort();

	vector v = { 5, 9, 6, 7, 1, 2, 3 };

	PriorityQueue<int> pq{v.begin(), v.end()};

	cout << pq;

	//MyList<string> a;
	////list<string> a;
	//auto c = "cool";

	//const auto start = std::chrono::steady_clock::now();
	//for (size_t i = 0; i < 100000; i++)
	//{
	//	a.emplace_back(c);
	//}
	//const auto end = std::chrono::steady_clock::now();
	//std::chrono::duration<double> elapsed_seconds = end - start;
	//std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	//list<MyStruct> c(3, 5);

	//MyList<list<string>, Allocator<list<string>>> g = { {"a", "b"}, {"a", "b"} };

	//MyList<list<string>, Allocator<list<string>>> k = { {"a", "b"}, {"a", "b"} };

	//g = std::move(k);

	//cout << g;

	/*ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Type a;

	auto* v = ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::allocate(a, 1);*/

	//ListBase<MyStruct, std::allocator<MyStruct>>::Node_Alloc_Traits::construct(a, v, 5);



	return 0;
}