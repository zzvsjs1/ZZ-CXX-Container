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

JSTD_END

#endif // !CONFIG
