#include <iostream>

#include <algorithm>
#include <list>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <forward_list>
#include <set>
#include <vector>
#include <chrono>
#include <string>

#include "Tst.h"

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
	vector<string> data = { "cute" };


	try
	{
		auto tst = TernarySearchTree(data.begin(), data.end());

		tst.addWord("farm");
		tst.addWord("sdfsdf");
		tst.addWord("ssdfghdfhfdghdfghdf");
		tst.addWord("kliuhsldighilshgskjlghkjsdghjl");


		cout << tst.search("kliuhsldighilshgskjlghkjsdghjl");
	}
	catch (...) {}


	//auto a = tst.deleteWord("app");
	//a = tst.deleteWord("cut");
	//a = tst.deleteWord("farm");
	//a = tst.deleteWord("cute");
	//a = tst.deleteWord("cup");

	
	return 0;
}