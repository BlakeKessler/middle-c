#pragma once
#ifndef MCSL_CHAR_TYPE_HPP
#define MCSL_CHAR_TYPE_HPP

#include "MCSL.hpp"

//!NOTE: implement character locales as a singleton
namespace mcsl {
   constexpr inline bool is_lower(const char ch) {
      return (ch >= 'a') && (ch <= 'z');
   }
   constexpr inline bool is_upper(const char ch) {
      return (ch >= 'A') && (ch <= 'Z');
   }
   constexpr inline char to_lower(const char ch) {
      return is_upper(ch) ? ch | CASE_DIF : ch;
      // return is_upper(ch) ? ch + CASE_DIF : ch;
   }
   constexpr inline char to_upper(const char ch) {
      return is_lower(ch) ? ch | ~CASE_DIF : ch;
      // return is_lower(ch) ? ch - CASE_DIF : ch;
   }
}

#endif //MCSL_CHAR_TYPE_HPP