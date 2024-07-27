#pragma once
#ifndef CLEF_ERR_HPP
#define CLEF_ERR_HPP

#include "CLEF.hpp"
#include <cstdio>
#include <stdexcept>

namespace clef {
   extern const char* ERR_MSG_ARR[];
   void throwError(const clef::ErrCode code, const char* formatStr, auto... args);
   void throwError(const clef::ErrCode code, const uint lineNum, const char* formatStr, auto... args);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

//!CLEF formatted error thrower
void clef::throwError(const clef::ErrCode code, const char* formatStr, auto... args) {
   std::fprintf(stderr, "\033[31;1;4mCLEF ERROR:\033[0m %s", ERR_MSG_ARR[asint(code)]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, "\n");
   std::exit(EXIT_FAILURE);
}
//!CLEF formatted error thrower with line num
void clef::throwError(const clef::ErrCode code, const uint lineNum, const char* formatStr, auto... args) {
   std::fprintf(stderr, "\033[31;1;4mCLEF ERROR:\033[0m %s", ERR_MSG_ARR[asint(code)]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, " \033[35m(line %u)\033[0m\n", lineNum);
   std::exit(EXIT_FAILURE);
}

#pragma GCC diagnostic pop

#endif //CLEF_ERR_HPP