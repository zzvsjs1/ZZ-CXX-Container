#pragma once
#ifndef CONFIG
#define CONFIG

#ifndef JSTD_START

#define JSTD_START namespace jstd {
#define JSTD_END }

#endif // !JSTD_START

JSTD_START

#define STD ::std::
#define NODISCARD [[nodiscard]]
#define TRY_START try {
#define CATCH_ALL } catch(...) {
#define THROW_AGAIN throw;
#define END_CATCH }

#define JLIBCXX_NOEXCEPT noexcept
#define JLIBCXX_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)
#define JLIBCXX_USE_NOEXCEPT noexcept
#define JLIBCXX_THROW(_EXC)

JSTD_END

#endif // !CONFIG
