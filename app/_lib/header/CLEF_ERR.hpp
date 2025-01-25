#pragma once
#ifndef CLEF_ERR_HPP
#define CLEF_ERR_HPP

#include "CLEF.hpp"
#include <cstdio>
#include <stdexcept>
#ifdef MCSL
   #undef NULL
#endif

namespace clef {
   extern const char* ERR_MSG_ARR[];
   [[noreturn, gnu::format(printf,2,3)]] void throwError(const clef::ErrCode code, const char* formatStr, auto&&... args);
   [[noreturn, gnu::format(printf,3,4)]] void throwError(const clef::ErrCode code, const uint lineNum, const char* formatStr, auto&&... args);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

//!CLEF formatted error thrower
[[noreturn, gnu::format(printf,2,3)]] void clef::throwError(const clef::ErrCode code, const char* formatStr, auto&&... args) {
   std::fprintf(stderr, "\033[31;1;4mCLEF ERROR:\033[0m %s", ERR_MSG_ARR[+code]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, "\n");
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}
//!CLEF formatted error thrower with line num
[[noreturn, gnu::format(printf,3,4)]] void clef::throwError(const clef::ErrCode code, const uint lineNum, const char* formatStr, auto&&... args) {
   std::fprintf(stderr, "\033[31;1;4mCLEF ERROR:\033[0m %s", ERR_MSG_ARR[+code]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, " \033[35m(line %u)\033[0m\n", lineNum);
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}

#pragma GCC diagnostic pop

#endif //CLEF_ERR_HPP