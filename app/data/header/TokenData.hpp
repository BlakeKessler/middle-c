#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"

namespace clef {
   // extern constexpr mcsl::static_arr<TokenType,256> tokTypeArr;
   extern constexpr TokenType tokType(const char c);
   template<mcsl::str_t str_t> constexpr TokenType tokType(const str_t& str);

   constexpr const char RADIX_SEPARATOR = 'p';
   constexpr const char DECIMAL_RADIX_SEPARATOR = 'e';
}

#include "../src/TokenData.cpp"

#endif //DATA_HPP