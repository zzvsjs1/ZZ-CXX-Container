#include <iostream>

#include <list>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <forward_list>
#include <set>

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

int main()
{
	MyList b = { 1, 2, 2, 3, 3, 2, 1, 1, 2 };
	MyList<string> a;

	list d = { 1, 2, 2, 3, 3, 2, 1, 1, 2 };

	cout << b;

	return 0;
}