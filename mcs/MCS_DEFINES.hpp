#pragma once
#ifndef MCS_DEFINES_HPP
#define MCS_DEFINES_HPP

#include <cstdint>

//type macros
using uint = uint32_t;
using luint = uint64_t;
using sint = int32_t;
using lsint = int64_t;
using byte = uint8_t;
using word = uint64_t;
using real = float;

//enums
namespace mcs {
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

#endif //MCS_DEFINES_HPP