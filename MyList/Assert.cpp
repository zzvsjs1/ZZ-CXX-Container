#include <iostream>
#include <cstdlib>

[[noreturn]] static void myAssertInternal(
	const char* expr, 
	const char* file, 
	const unsigned int line, 
	const char* function,
	const char* message)
{
	std::cerr
		<< "Assertion filed: "
		<< expr
		<< ", file "
		<< file
		<< ", line "
		<< line
		<< ". In function \""
		<< function
		<< "\"."
		<< ::std::endl;

	if (message)
	{
		::std::cerr << message << ::std::endl;
	}

	::std::abort();
}

[[noreturn]] void myAssert(const char* expr, 
	const char* file, 
	const unsigned int line, 
	const char* function, 
	const char* message)
{
	myAssertInternal(expr, file, line, function, message);
}

