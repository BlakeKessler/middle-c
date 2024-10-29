#pragma once
#ifndef MCSL_CHAR_TYPE_HPP
#define MCSL_CHAR_TYPE_HPP

#include "MCSL.hpp"

//!NOTE: implement character locales as a singleton
namespace mcsl {
   //char/string const
   constexpr byte ASCII_CASE_BIT = 'a' ^ 'A';

   constexpr inline bool is_lower(const char ch) {
      return (ch >= 'a') && (ch <= 'z');
   }
   constexpr inline bool is_upper(const char ch) {
      return (ch <= 'Z') && (ch >= 'A');
   }
   constexpr inline char to_lower(const char ch) {
      return is_upper(ch) ? ch ^ ASCII_CASE_BIT : ch;
   }
   constexpr inline char to_upper(const char ch) {
      return is_lower(ch) ? ch ^ ASCII_CASE_BIT : ch;
   }

   constexpr inline bool is_whitespace(const char ch) {
      switch (ch) {
         case  ' ':
         case '\t':
         case '\n':
         case '\v':
         case '\f':
         case '\r':
            return true;
         default:
            return false;
      }
      // return (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r');
   }
}

#endif //MCSL_CHAR_TYPE_HPP