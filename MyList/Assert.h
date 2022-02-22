#pragma once
#ifndef ASSERT_H
#define ASSERT_H

[[noreturn]] void myAssert(const char*, const char*, unsigned int, const char*, const char*);

#define ASSERT_M(expr, message) (static_cast<bool>(expr) ? \
		void(0) : \
		myAssert(#expr, __FILE__, __LINE__, __func__, message))

#define MY_ASSERT(expr) (ASSERT_M(expr, nullptr))

#endif // !ASSERT_H