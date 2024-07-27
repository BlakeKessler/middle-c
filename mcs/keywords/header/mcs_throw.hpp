#pragma once
#ifndef MCS_ERR_HPP
#define MCS_ERR_HPP

#include "MCS.hpp"
#include <stdexcept>
#include <cstdio>

namespace mcs {
   extern const char* ERR_MSG_ARR[];
   void mcs_throw(const ErrCode code, const char* formatStr, auto... args);
   void mcs_throw(const ErrCode code, const uint lineNum, const char* formatStr, auto... args);
}



#pragma region src
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
//!MCS formatted error thrower
void mcs::mcs_throw(const ErrCode code, const char* formatStr, auto... args) {
   std::fprintf(stderr, "\033[31;1;4mMCS ERROR:\033[0m %s", ERR_MSG_ARR[(uint)code]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, "\n");
   std::exit(EXIT_FAILURE);
}
//!MCS formatted error thrower with line num
void mcs::mcs_throw(const ErrCode code, const uint lineNum, const char* formatStr, auto... args) {
   std::fprintf(stderr, "\033[31;1;4mMCS ERROR:\033[0m %s", ERR_MSG_ARR[(uint)code]);
   std::fprintf(stderr, formatStr, args...);
   std::fprintf(stderr, " \033[35m(line %u)\033[0m\n", lineNum);
   std::exit(EXIT_FAILURE);
}
#pragma GCC diagnostic pop
#pragma endregion src

#endif //MCS_ERR_HPP