#pragma once
#ifndef MCSL_DEFINES_HPP
#define MCSL_DEFINES_HPP

#include <cstdint>
#include <utility>

//self
#define self (*this)

//!FIXED WIDTH INTEGER TYPES
using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using sint8  = int8_t;
using sint16 = int16_t;
using sint32 = int32_t;
using sint64 = int64_t;

//!ARCHITECTURE-BASED INTEGER TYPES
using byte = uint8;
using word = uint64;

using ushort = uint16;
using uint = uint32;
using ulong = uint64;

using sshort = sint16;
using sint = sint32;
using slong = sint64;

//!FIXED-WIDTH FLOATING-POINT TYPES
// using float16 = std::float16_t;
// using float32 = std::float32_t;
// using float64 = std::float64_t;
using float32 = float;
using float64 = double;

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