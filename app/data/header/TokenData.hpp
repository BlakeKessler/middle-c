#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"

namespace clef {
   constexpr const char RADIX_POINT = '.';
   constexpr const char RADIX_SEPARATOR = '_';
   // constexpr const char RADIX_SEPARATOR = 'p';
   // constexpr const char DECIMAL_RADIX_SEPARATOR = 'e';

   constexpr const char PREPROC_INIT = '#';
   constexpr const char EOS = ';';
   constexpr const char ESCAPE_CHAR = '\\';
   constexpr const char CHAR_DELIM = '\'';
   constexpr const char STR_DELIM = '\"';
   constexpr const char INTERP_STR_DELIM = '`';
   constexpr const char PLACEHOLDER_INIT = '$';
   constexpr const char PLACEHOLDER_OPEN = '{';
   constexpr const char PLACEHOLDER_CLOSE = '}';
}

#endif //DATA_HPP