#include <iostream>

#include <algorithm>
#include <list>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <forward_list>

#include <vector>
#include <chrono>
#include <string>
#include "Strings.h"
#include <limits>


using std::cin;
using std::cout;
using std::end;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::forward_list;
using std::move;


//template <typename T, typename Alloc>
//std::ostream& operator<<(std::ostream& ostr, const Vector<T, Alloc>& list)
//{
//	for (const auto& i : list)
//	{
//		ostr << i << " ";
//	}
//
//	ostr << endl;
//
//	return ostr;
//}

int main()
{
	cout << ::std::boolalpha;

	int i = 5;

	cout << strFormat("%zu %%p good\n", static_cast<size_t>(50000000));
	
	return 0;
}