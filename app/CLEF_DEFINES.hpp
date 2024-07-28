#pragma once
#ifndef CLEF_DEFINES_HPP
#define CLEF_DEFINES_HPP

#include <cstdint>
#include <type_traits>

//CLEF defines
#define MAX_AST_CHILDREN 3
#define MAX_DELIM_LEN 2
#define MAX_OP_LEN 3
#define MAX_KEYWORD_LEN 15

//type macros
using uint = uint32_t;
using luint = uint64_t;
using sint = int32_t;
using lsint = int64_t;
using byte = uint8_t;
using word = uint64_t;
using real = float;

//type macro properties
#define CLEF_TMAX(type) ((std::make_unsigned_t<type>)(~0) >> std::is_signed<type>()) //function macro to calculate max value of integer types - assumes Two's Complement
// #define CLEF_TMAX(type) ((1 << (8 * sizeof(type) - (0 > (type)(~0)))) - 1)
constexpr luint UINT_MAX = CLEF_TMAX(uint);
constexpr luint SINT_MAX = CLEF_TMAX(sint);
constexpr luint LUINT_MAX = CLEF_TMAX(luint);
constexpr luint LSINT_MAX = CLEF_TMAX(lsint);
constexpr luint BYTE_MAX = CLEF_TMAX(byte);
constexpr luint WORD_MAX = CLEF_TMAX(word);
#undef CLEF_TMAX

//bit manipulation macros
#define testbit(num, index) ((num >> index) & 1)

//utils
#define asint(enumVal) ((uint)enumVal)
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

#endif //CLEF_DEFINES_HPP