#pragma once
#ifndef MCSL_DEFINES_HPP
#define MCSL_DEFINES_HPP

#include <cstdint>

//self
#define self (*this)

//type macros
using uint = uint32_t;
using luint = uint64_t;
using sint = int32_t;
using lsint = int64_t;
using byte = uint8_t;
using word = uint64_t;
using real = float;

//enums
namespace mcsl {
   enum class ErrCode {
      UNSPEC = 0,
      SEGFAULT,
      ALLOC_FAIL,
   };
}

//simple integer macros
#define sign(num) ((num > 0) - (num < 0))
#define signofdif(a,b) ((a > b) - (a < b))

//char/string const
constexpr byte CASE_DIF = 'a' - 'A';

// //comparison macros
// #define cmpfunc(T) bool cmp(const T& a, const T& b)

#endif //MCSL_DEFINES_HPP