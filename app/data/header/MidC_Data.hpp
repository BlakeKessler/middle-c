#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"
#include "OpData.hpp"

#include "raw_str_span.hpp"
#include "raw_str.hpp"
#include "raw_buf_str.hpp"
#include "static_arr.hpp"
#include "pair.hpp"
#include "string.hpp"

#include <unordered_set>

namespace clef {
   extern const mcsl::static_arr<TokenType,256> tokTypeArr;
   constexpr TokenType tokType(const char c) { return tokTypeArr[c]; }

   // // extern const mcsl::static_arr<mcsl::string> KEYWORDS;
   static const std::unordered_set<mcsl::string> KEYWORDS{
      "void", "auto",
      "uint", "ushort", "ulong", "ubyte",
      "sint", "sshort", "slong", "sbyte",
      "bool", "float", "num_t",
      "char", "char_utf8", "char_utf16", "char_utf32",
      "uint8", "uint16", "uint32", "uint64", "uint128", "uint256",
      "sint8", "sint16", "sint32", "sint64", "sint128", "sint256",
      "float16", "float32", "float64", "float128", "float256",
      "half", "single", "double", "quad", "oct",
      "num8", "num16", "num32", "num64", "num128", "num256",
      
      "true", "false", "nullptr", "null",

      "const", "constexpr", "immediate", "final", "mutable", "volatile", "atomic",
      "class", "struct", "union", "enum", "mask", "namespace",
      "private", "protected", "public", "friend",
      "extern", "inline", "static", "virtual", "override", "explicit", "noexcept",
      "this", "self",

      "new", "delete", "sizeof", "arrlen",
      "typeof", "typeid", "typename", "alignas", "alignof",
      "template",
      "using",

      "asm",
      "return",
      "operator",
      "assert", "static_assert",
      "cast", "up_cast", "dyn_cast", "bit_cast", "const_cast",

      "if", "else", "for", "while", "do", "break", "continue", "switch", "match", "case", "default", "goto",
      "try", "catch", "throw"
   };
   static const std::unordered_set<mcsl::string> TYPE_KEYWORDS{
      "void", "auto",
      "uint", "ushort", "ulong", "ubyte",
      "sint", "sshort", "slong", "sbyte",
      "bool", "float", "num_t",
      "char", "char_utf8", "char_utf16", "char_utf32",
      "uint8", "uint16", "uint32", "uint64", "uint128", "uint256",
      "sint8", "sint16", "sint32", "sint64", "sint128", "sint256",
      "float16", "float32", "float64", "float128", "float256",
      "half", "single", "double", "quad", "oct",
      "num8", "num16", "num32", "num64", "num128", "num256"
   };
   static const std::unordered_set<mcsl::string> TYPE_QUAL_KEYWORDS{
      "const", "constexpr", "immediate", "final", "mutable", "volatile", "atomic", "static", "extern"
   };
   static const std::unordered_set<mcsl::string> OBJ_TYPE_KEYWORDS{
      "class", "struct", "union", "enum", "mask"
   };
   static const std::unordered_set<mcsl::string> OBJ_SCOPE_SPEC_KEYWORDS{
      "private", "protected", "public"
   };
   static const std::unordered_set<mcsl::string> CAST_KEYWORDS{
      "cast", "up_cast", "const_cast", "bit_cast", "dyn_cast" 
   };

   //!function to determine if a string is a Middle-C keyword
   template<mcsl::str_t strT> inline bool isKeyword(const strT& str) { return KEYWORDS.contains(str); }
}

#endif //DATA_HPP