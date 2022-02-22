#pragma once
#ifndef OPTIONAL
#define OPTIONAL

#include <optional>
#include <stdexcept>
#include "Config.h"

JSTD_START

template <typename T>
class Optional;

struct Nullopt_T
{
	enum class Construct { Token };

	explicit constexpr Nullopt_T(Construct) { }
};

inline constexpr Nullopt_T NULLOPT{ Nullopt_T::Construct::Token };

class BadOptionalAccess : public STD exception
{
public:

	BadOptionalAccess() = default;

	virtual ~BadOptionalAccess() = default;

	NODISCARD char const* what() const override
	{
		return "bad optional access";
	}
};

[[noreturn]] inline void throwBadOptionalAccess()
{
	throw BadOptionalAccess{};
}



JSTD_END

#endif // !OPTIONAL