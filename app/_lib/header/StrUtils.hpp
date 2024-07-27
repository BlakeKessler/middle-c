#pragma once
#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP

#include "CLEF.hpp"

namespace clef {
   //!compare two null-terminated strings
   //!if the two have different lengths, they are automatically not equal
   inline sint strcmp(const char* lhs, const char* rhs, const uint length) {
      for (uint i = 0; i < length; ++i) {
         if (!lhs[i] || !rhs[i] || (lhs[i] != rhs[i])) {
            return lhs[i] - rhs[i];
         }
      }
      return 0;
   }
   //!compare two null-terminated strings
   //!if the two have different lengths, only characters within the domain of the shorter string are considered
   inline sint substrcmp(const char* lhs, const char* rhs, const uint length) {
      for (uint i = 0; i < length; ++i) {
         if (!lhs[i] || !rhs[i]) {
            return 0;
         }
         if (lhs[i] != rhs[i]) {
            return lhs[i] - rhs[i];
         }
      }
      return 0;
   }
   constexpr const char UNDERLINE_S[] = "\033[4m";
   constexpr const char UNDERLINE_E[] = "\033[24m";
   constexpr const char BOLD[] = "\033[;1m";
   constexpr const char NOT_BOLD[] = "\033[0m";
}

#endif //STR_UTILS_HPP