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

#include "Array.h"
#include "MyList.h"
#include "Healper.h"
#include "Memory.h"
#include "PritorityQueue.h"
#include "MyForwardList.h"

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
	for (const auto& i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const list<T, Alloc>& list)
{
	for (const auto& i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const vector<T, Alloc>& list)
{
	for (const auto& i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename t, typename container, typename compare>
std::ostream& operator<<(std::ostream& ostr, PriorityQueue<t, container, compare>& pq)
{
	for (decltype(pq.size()) size = pq.size(), start = 0; start < size; ++start)
	{
		ostr << pq.top() << " ";
		pq.pop();
	}

	ostr << endl;

	return ostr;
}

template <typename T, STD size_t N>
std::ostream& operator<<(std::ostream& ostr, Array<T, N> arr)
{
	for (const auto& d : arr)
	{
		ostr << d << " ";
	}

	ostr << endl;

	return ostr;
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& ostr, const FList<T, Alloc>& list)
{
	for (const auto& i : list)
	{
		ostr << i << " ";
	}

	ostr << endl;

	return ostr;
}

struct MyStruct
{

	MyStruct(int i)
		: d(i) { }

	~MyStruct() noexcept
	{
		cout << "Good Bye ";
	}

	int d;
};

int main()
{
	FList<string> d = { "cool", "wa" , "sdfsdfsd", "dfghfghfghfghg"};
	d.erase_after(++d.before_begin(), d.end());
	cout << d;


	return 0;
}