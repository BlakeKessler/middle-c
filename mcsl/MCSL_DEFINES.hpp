#pragma once
#ifndef MCSL_DEFINES_HPP
#define MCSL_DEFINES_HPP

#include <cstdint>
#include <utility>

//self
#define self (*this)

//types
using uint = uint32_t;
using luint = uint64_t;
using sint = int32_t;
using lsint = int64_t;
using byte = uint8_t;
using word = uint64_t;
using real = float;
using lreal = double;

//enums
namespace mcsl {
   enum class ErrCode {
      UNSPEC = 0,
      SEGFAULT,
      ALLOC_FAIL,
   };
   constexpr auto operator+(const ErrCode code) { return std::to_underlying(code); }
}

//simple integer macros
#define sign(num) ((num > 0) - (num < 0))
#define signofdif(a,b) ((a > b) - (a < b))

// //comparison macros
// #define cmpfunc(T) bool cmp(const T& a, const T& b)

constexpr const uint DEFAULT_BUCKET_SIZE = 4;





//bitmask literal
constexpr unsigned long long operator"" _m(const unsigned long long x) {
   return x ? 1LU << (x - 1) : 0;
}

#endif //MCSL_DEFINES_HPP