#include <cassert>
#include <cstdarg>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "Strings.h"

namespace 
{

enum BuildInType
{
	UNSIGNED_CHAR,
	CHAR,
	UNSIGNED_SHORT,
	SHORT,
	UNSIGNED_INT,
	INT,
	UNSIGNED_LONG,
	LONG,
	UNSIGNED_LONG_LONG,
	LONG_LONG,
	FLOAT,
	DOUBLE,
	LONG_DOUBLE,
	SIZE_T,
	PTRDIFF_T,
	UNKNOWN
};


struct ArgsParser
{
	long long width = 0;
	long long precision = 0;
	int zeroPad = 0;
	const char* f = nullptr;
	BuildInType type = BuildInType::UNKNOWN;
};

template <typename IntegerT>
struct DivT
{
	static_assert(::std::is_integral_v<DivT>, "Value must be integer.");

	DivT quot;
	DivT rem;
};

template <typename IntegerT>
[[maybe_unused]]
DivT<IntegerT> divMod(IntegerT numerator, IntegerT denominator) noexcept
{
	return { numerator / denominator, numerator % denominator };
}

template <typename CharT, typename UnsignedIntT>
CharT* writeUnsignedToBuffer(CharT* first, CharT* last, UnsignedIntT value, const int base) noexcept
{
	static_assert(::std::is_unsigned_v<UnsignedIntT>, "Value must be unsigned.");

	do
	{
		--first;

		if (const auto temp = value % base; base > 10 && temp >= 10)
		{
			*first = static_cast<CharT>('A' + temp - 10);
		}
		else
		{
			*first = static_cast<CharT>('0' + temp);
		}

		value /= base;
	}
	while (value != 0);

	return first;
}

void fillBasePrefix(::std::string& ret, const unsigned int base)
{
	if (base == 8)
	{
		ret += "0O";
	}
	else if (base == 16)
	{
		ret += "0x";
	}
	else if (base == 2)
	{
		ret += "0b";
	}
}

template <typename UnsignedIntT>
void writeUnSignedInteger(::std::string& ret, UnsignedIntT value, const unsigned int base)
{
	static_assert(::std::is_unsigned_v<UnsignedIntT>, "Value must be unsigned.");

	if (base > 32)
	{
		throw ::std::runtime_error("Only support 2 - 32 base integer.");
	}

	char buffer[256];
	auto start = ::std::end(buffer);
	auto end = start;

	start = writeUnsignedToBuffer(
		start,
		end,
		value,
		base
	);

	fillBasePrefix(ret, base);

	for (; start != end; ++start)
	{
		ret += *start;
	}
}

template <typename SignedIntT>
void writeSignedInteger(::std::string& ret, SignedIntT value, const unsigned int base)
{
	static_assert(::std::is_signed_v<SignedIntT>, "Value must be signed.");

	if (base > 32)
	{
		throw ::std::runtime_error("Only support 2 - 32 base integer.");
	}

	char buffer[256];
	auto start = ::std::end(buffer);
	auto end = start;

	if (value < 0)
	{
		start = writeUnsignedToBuffer(
			start,
			end,
			::std::make_unsigned_t<SignedIntT>(-value),
			base
		);

		if (base == 10)
		{
			*--start = '-';
		}
	}
	else
	{
		start = writeUnsignedToBuffer(
			start, 
			end, 
			::std::make_unsigned_t<SignedIntT>(value), 
			base
		);
	}

	fillBasePrefix(ret, base);

	for (; start != end; ++start)
	{
		ret += *start;
	}
}

[[maybe_unused]]
void doubleToStr(::std::string& ret, const double value)
{
	constexpr double inf = ::std::numeric_limits<double>::infinity();
	constexpr double epsilon = ::std::numeric_limits<double>::epsilon();

	const double upper = value + epsilon;
	const double lower = value - epsilon;

	if (value != value)
	{
		ret += "nan";
		return;
	}

	if (upper == inf || lower == inf)
	{
		ret += "inf";
	}
	
}

bool isIntegerFlag(const char ch) noexcept
{
	constexpr ::std::string_view sv = "idoxb";

	for (const char each : sv)
	{
		if (each == ch || ::toupper(static_cast<unsigned char>(each)) == ch)
		{
			return true;
		}
	}

	return false;
}

const char* formatByToken(::std::string& ret, const char* const formatter, va_list& vargs)
{
	ArgsParser argsParser;
	argsParser.f = formatter + 1;

	// long or long long
	if (*argsParser.f == 'l')
	{
		if (isIntegerFlag(argsParser.f[1]))
		{
			argsParser.type = BuildInType::LONG;
			++argsParser.f;
		}
		else if (argsParser.f[1] == 'l' && isIntegerFlag(argsParser.f[2]))
		{
			argsParser.type = BuildInType::LONG_LONG;
			argsParser.f += 2;
		}
	}
	// short or char
	else if (*argsParser.f == 'h')
	{
		if (argsParser.f[1] == 'h')
		{
			argsParser.type = BuildInType::SHORT;
			argsParser.f += 2;
		}
		else
		{
			argsParser.type = BuildInType::CHAR;
			++argsParser.f;
		}
	}

	// size_t
	else if (*argsParser.f == 'z' && isIntegerFlag(argsParser.f[1]))
	{
		argsParser.type = BuildInType::SIZE_T;
		++argsParser.f;
	}

	const auto signConvert = [&argsParser, &ret, &vargs](const unsigned int base)
	{
		switch (argsParser.type)
		{
		case BuildInType::SHORT:
			{
				writeSignedInteger(ret, va_arg(vargs, short), base);
				break;
			}
		case BuildInType::LONG:
			{
				writeSignedInteger(ret, va_arg(vargs, long), base);
				break;
			}
		case BuildInType::LONG_LONG:
			{
				writeSignedInteger(ret, va_arg(vargs, long long), base);
				break;
			}
		case BuildInType::SIZE_T:
			{
				writeSignedInteger(ret, va_arg(vargs, ::std::make_signed_t<size_t>), base);
				break;
			}
		default:
			{
				writeSignedInteger(ret, va_arg(vargs, int), base);
				break;
			}
		}
	};

	const auto unsignedConvert = [&argsParser, &ret, &vargs](const unsigned int base)
	{
		switch (argsParser.type)
		{
		case BuildInType::SHORT:
			{
				writeUnSignedInteger(ret, va_arg(vargs, unsigned short), base);
				break;
			}
		case BuildInType::LONG:
			{
				writeUnSignedInteger(ret, va_arg(vargs, unsigned long), base);
				break;
			}
		case BuildInType::LONG_LONG:
			{
				writeUnSignedInteger(ret, va_arg(vargs, unsigned long long), base);
				break;
			}
		case BuildInType::SIZE_T:
			{
				writeUnSignedInteger(ret, va_arg(vargs, size_t), base);
				break;
			}
		default:
			{
				writeUnSignedInteger(ret, va_arg(vargs, unsigned int), base);
				break;
			}
		}
	};

	switch (*argsParser.f)
	{
	case '%':
		{
			ret += '%';
			break;
		}
	case 'i':
	case 'd':
		{
			signConvert(10);
			break;
		}
	case 'o':
		{
			signConvert(8);
			break;
		}
	case 'x':
	case 'X':
		{
			signConvert(16);
			break;
		}
	case 'b':
	case 'B':
		{
			// TODO: Support unsigned.
			signConvert(2);
			break;
		}
	case 'u':
		{
			unsignedConvert(10);
			break;
		}
	case 'c':
		{
			const int temp = va_arg(vargs, int);
			if (temp < 0 || temp > 127)
			{
				throw ::std::runtime_error("Argument is not a ASCII char.");
			}

			ret += static_cast<char>(temp);
			break;
		}
	case 's':
		{
			ret += va_arg(vargs, const char*);
			break;
		}
	case 'p':
		{
			writeUnSignedInteger(ret, va_arg(vargs, ::std::size_t), 16);
			break;
		}
	case 'f':
	case 'F':
	case 'e':
	case 'E':
	case 'a':
	case 'A':
	case 'g':
	case 'G':
		{
			const double dVal = va_arg(vargs, double);
			doubleToStr(ret, dVal);
			break;
		}
	default:
		{
			throw ::std::runtime_error("Unknown token.");
		}
	}

	++argsParser.f;
	return argsParser.f;
}


}


::std::string strFormat(const char* const formatter, ...)
{
	assert(formatter);

	::std::string ret;
	va_list vargs;
	const char* f = formatter;

	va_start(vargs, formatter);

	try
	{
		while (*f)
		{
			if (*f == '%')
			{
				f = formatByToken(ret, f, vargs);
			}
			else
			{
				if (static_cast<unsigned char>(*f) > 127)
				{
					throw ::std::runtime_error("Non ASCII value.");
				}

				ret += *f;
				++f;
			}
		}
	}
	catch (...)
	{
		va_end(vargs);
		throw;
	}
	
	va_end(vargs);

	return ret;
}