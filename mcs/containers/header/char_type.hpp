#pragma once
#ifndef CHAR_TYPE_HPP
#define CHAR_TYPE_HPP

#include "MCS.hpp"

//!NOTE: implement character locales as a singleton
namespace mcs {
   inline bool is_lower(const char ch) {
      return (ch >= 'a') && (ch <= 'z');
   }
   inline bool is_upper(const char ch) {
      return (ch >= 'A') && (ch <= 'Z');
   }
   inline char to_lower(const char ch) {
      return is_upper(ch) ? ch + CASE_DIF : ch;
   }
   inline char to_upper(const char ch) {
      return is_lower(ch) ? ch - CASE_DIF : ch;
   }
}

#endif //CHAR_TYPE_HPP