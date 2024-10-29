#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"
#include "raw_str.hpp"
#include "raw_buf_str.hpp"
#include "static_arr.hpp"
#include "pair.hpp"
#include "string.hpp"

#include <unordered_set>
#include <cassert>
#include <cstring>

constexpr uint testStrlen(const char str[MAX_OP_LEN + 1]) {
   for (uint i = 0; i <= MAX_OP_LEN; ++i) {
      if (!str[i]) {
         return i;
      }
   }
   return MAX_OP_LEN;
}

//operator string + metadata
struct clef::Operator {
   mcsl::raw_buf_str<MAX_OP_LEN,byte> opStr;
   byte precedence;
   OpType opType; //unary/binary/special, left/right associative

   constexpr Operator(): opStr(""),precedence(0),opType(static_cast<OpType>(0)) {}
   constexpr Operator(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type):
      opStr(str),precedence(prec),opType(type) {}
   
   constexpr byte size() const { return opStr.size(); }
};
//delimiter pair strings
struct clef::DelimPair {
   mcsl::raw_buf_str<MAX_OP_LEN, byte> open;
   mcsl::raw_buf_str<MAX_OP_LEN, byte> close;
};

using enum clef::OpType;
namespace clef {
   extern const mcsl::static_arr<TokenType,256> tokTypeArr;
   constexpr TokenType tokType(const char c) { return tokTypeArr[c]; }

   static constexpr const mcsl::static_arr OPERATORS{
      Operator("#",     0,  PREFIX),   //invoke preprocessor

      Operator("\"",    1,  BLOCK_DELIM), //string
      Operator("\'",    1,  BLOCK_DELIM), //char
      Operator("//",    1,  PREFIX),//line comment
      Operator("/*",    1,  BLOCK_DELIM),//block comment
      Operator("*/",    1,  BLOCK_DELIM), //block comment

      Operator("::",    2,  INFIX_LEFT),  //scope resolution

      Operator("++",    3,  PREFIX),   //pre-increment
      Operator("--",    3,  PREFIX),   //pre-decrement
      Operator("(",     3,  BLOCK_DELIM),//function calls/functional casts
      Operator("[",     3,  BLOCK_DELIM),//subscript
      Operator("{",     3,  BLOCK_DELIM),//scope/functional casts
      Operator("<",     3,  BLOCK_DELIM),//specifier
      Operator(")",     3,  BLOCK_DELIM), //function calls/functional casts
      Operator("]",     3,  BLOCK_DELIM), //subscript
      Operator("}",     3,  BLOCK_DELIM), //scope/functional casts
      Operator(">",     3,  BLOCK_DELIM), //specifier
      Operator(".",     3,  INFIX_LEFT),  //element access
      Operator("->",    3,  INFIX_LEFT),  //element access
      // Operator("?",     3,  LEFT_SPEC), //safe access modifier
      Operator("..",    3,  INFIX_LEFT),  //range
      Operator("...",   3,  INFIX_LEFT),  //array spread

      Operator("++",    4,  POSTFIX),  //post-increment
      Operator("--",    4,  POSTFIX),  //post-decrement
      Operator("+",     4,  POSTFIX),  //unary plus
      Operator("-",     4,  POSTFIX),  //integer negation
      Operator("!",     4,  POSTFIX),  //logical negation
      Operator("~",     4,  POSTFIX),  //bitwise negation
      Operator("&",     4,  TYPE_MOD | PREFIX),  //reference/address of
      Operator("*",     4,  TYPE_MOD | PREFIX),  //raw pointer/dereference
      Operator("@",     4,  TYPE_MOD),  //unique pointer
      Operator("$",     4,  TYPE_MOD),  //shared pointer
      Operator("\\",    4,  TYPE_MOD),  //weak pointer
      Operator("%",     4,  TYPE_MOD),  //iterator
      
      Operator(".*",    5,  INFIX_LEFT),  //pointer to member
      Operator("->*",   5,  INFIX_LEFT),  //pointer to member

      Operator("**",    6,  INFIX_LEFT),  //exponentiation

      Operator("*",     7,  INFIX_LEFT),  //multiplication
      Operator("/",     7,  INFIX_LEFT),  //division
      Operator("%",     7,  INFIX_LEFT),  //modulo

      Operator("+",     8,  INFIX_LEFT),  //addition
      Operator("-",     8,  INFIX_LEFT),  //subtraction

      Operator("<<",    9,  INFIX_LEFT),  //left bit-shift
      Operator(">>",    9,  INFIX_LEFT),  //right bit-shift

      Operator("<=>",   10, INFIX_LEFT),  //three-way comparison

      Operator(">",     11, INFIX_LEFT),  //less than
      Operator("<",     11, INFIX_LEFT),  //greater than
      Operator("<=",    11, INFIX_LEFT),  //less than or equal to
      Operator(">=",    11, INFIX_LEFT),  //greather than or equal to

      Operator("==",    12, INFIX_LEFT),  //equality
      Operator("!=",    12, INFIX_LEFT),  //inequality
      Operator("===",   12, INFIX_LEFT),  //strict equality
      Operator("!==",   12, INFIX_LEFT),  //strict inequality

      Operator("&",     13, INFIX_LEFT),  //bitwise AND
      Operator("^",     14, INFIX_LEFT),  //bitwise XOR
      Operator("|",     15, INFIX_LEFT),  //bitwise OR
      Operator("&&",    16, INFIX_LEFT),  //logical AND
      Operator("||",    17, INFIX_LEFT),  //logical OR

      Operator("\?\?",  18, INFIX_RIGHT), //null coalescing
      Operator("?",     18, INFIX_RIGHT), //inline if
      Operator(":",     18, INFIX_RIGHT), //inline else
      Operator("=",     18, INFIX_RIGHT), //direct assignment
      Operator(":=",    18, INFIX_RIGHT), //const assignment
      Operator("+=",    18, INFIX_RIGHT), //compound assignment (add)
      Operator("-=",    18, INFIX_RIGHT), //compound assignment (sub)
      Operator("*=",    18, INFIX_RIGHT), //compound assignment (mul)
      Operator("/=",    18, INFIX_RIGHT), //compound assignment (div)
      Operator("%=",    18, INFIX_RIGHT), //compound assignment (mod)
      Operator("**=",   18, INFIX_RIGHT), //compound assignment (exp)
      Operator("<<=",   18, INFIX_RIGHT), //compound assignment (leftshift)
      Operator(">>=",   18, INFIX_RIGHT), //compound assignment (rightshift)
      Operator("&=",    18, INFIX_RIGHT), //compound assignment (AND)
      Operator("^=",    18, INFIX_RIGHT), //compound assignment (XOR)
      Operator("|=",    18, INFIX_RIGHT), //compound assignment (OR)
      Operator("\?\?=", 18, INFIX_RIGHT), //compound assignment (null coalescing)

      Operator(",",     19, INFIX_LEFT)  //comma

      //not included in array: triangle (free/unspecified) operators
   };
   static constexpr const mcsl::static_arr BLOCK_DELIMS{
      DelimPair{},           //NULL DELIM PAIR
      DelimPair{"(", ")"},   //PARENS
      DelimPair{"[", "]"},   //SUBSCRIPT
      DelimPair{"{", "}"},   //INITIALIZER LIST
      DelimPair{"<", ">"},   //SPECIALIZER

      // DelimPair{"?\0", ":\0"},   //TERNARY STATEMENT
   };
   static constexpr const mcsl::static_arr PTXT_DELIMS{
      DelimPair{},           //NULL DELIM PAIR
      DelimPair{"\'", "\'"}, //CHARACTER
      DelimPair{"\"", "\""}, //STRING
      
      DelimPair{"/*", "*/"},     //BLOCK COMMENT
      DelimPair{"//", "\n\0"},   //SINGLE-LINE COMMENT
   };
   // // extern const mcsl::static_arr<mcsl::string> KEYWORDS;
   static const std::unordered_set<mcsl::string> KEYWORDS{
      "void", "auto",
      "uint", "ushort", "ulong", "ubyte",
      "sint", "sshort", "slong", "sbyte",
      "bool", "float", "num",
      "char", "char_utf8", "char_utf16", "char_utf32",
      "uint8", "uint16", "uint32", "uint64", "uint128", "uint256",
      "sint8", "sint16", "sint32", "sint64", "sint128", "sint256",
      "float16", "float32", "float64", "float128", "float256",
      "half", "single", "double", "quad", "oct",
      "num8", "num16", "num32", "num64", "num128", "num256",
      
      "true", "false", "nullptr", "null",

      "const", "constexpr", "immediate", "final", "mutable", "volatile", "atomic",
      "class", "struct", "union", "enum", "namespace",
      "private", "protected", "public", "friend",
      "extern", "inline", "static", "virtual", "override", "explicit", "noexcept",
      "this", "self",

      "new", "delete", "sizeof", "arrsizeof",
      "typeof", "typeid", "typename", "alignas", "alignof",
      "template",
      "using",

      "asm",
      "return",
      "operator",
      "assert", "static_assert",
      "type_cast", "bit_cast", "safe_cast", "quick_cast", "const_cast",

      "if", "else", "for", "while", "do", "break", "continue", "switch", "match", "case", "default", "goto",
      "try", "catch", "throw"
   };
   static const std::unordered_set<mcsl::string> TYPE_KEYWORDS{
      "void", "auto",
      "uint", "ushort", "ulong", "ubyte",
      "sint", "sshort", "slong", "sbyte",
      "bool", "float", "num",
      "char", "char_utf8", "char_utf16", "char_utf32",
      "uint8", "uint16", "uint32", "uint64", "uint128", "uint256",
      "sint8", "sint16", "sint32", "sint64", "sint128", "sint256",
      "float16", "float32", "float64", "float128", "float256",
      "half", "single", "double", "quad", "oct",
      "num8", "num16", "num32", "num64", "num128", "num256"
   };
   static const std::unordered_set<mcsl::string> TYPE_QUAL_KEYWORDS{
      "const", "constexpr", "immediate", "final", "mutable", "volatile", "atomic", "static"
   };
   static const std::unordered_set<mcsl::string> OBJ_TYPE_KEYWORDS{
      "class", "struct", "union", "enum"
   };
   static const std::unordered_set<mcsl::string> OBJ_SCOPE_SPEC_KEYWORDS{
      "private", "protected", "public"
   };
   static const std::unordered_set<mcsl::string> CAST_KEYWORDS{
      "type_cast", "bit_cast", "safe_cast", "quick_cast", "const_cast"
   };



   //block delims
   template<mcsl::str_t strT> DelimPairType blockDelimType(const strT& str) {
      if (!str.size() || str.size() > MAX_DELIM_LEN) {
         return DelimPairType::NONE;
      }

      //search array
      for (uint i = 0; i < BLOCK_DELIMS.size(); ++i) {
         if (str == BLOCK_DELIMS[i].open || str == BLOCK_DELIMS[i].close) {
            return static_cast<DelimPairType>(i);
         }
      }
      return DelimPairType::NONE;
   }

   //operators
   template<mcsl::str_t strT> uint maxOpLen(const strT& str) {
      uint maxlen = 0;
      for (uint i = 0; i < OPERATORS.size(); ++i) {
         //check length
         if (str.size() < OPERATORS[i].size()) {
            continue;
         }
         //find first differing char
         for (uint j = 0; j < OPERATORS[i].size(); ++j) {
            if (str[j] == OPERATORS[i].opStr[j]) {
               maxlen = j >= maxlen ? j + 1 : maxlen;
            }
            else {
               break;
            }
         }
      }

      //triangle-bracket-enclosed operators
      return (((maxlen < 3 && str.size() >= 3)
            && (str[0] == '<' || str[0] == '>') && (str[2] == '<' || str[2] == '>'))
            && +(tokTypeArr[+str[1]] & TokenType::OP))
         ? 3 : maxlen;
   }
   // template<mcsl::str_t strT> const Operator* getOpData(const strT& str, bool banBinary = true) {
   //    uint maxlen = 0;
   //    const Operator* op = nullptr;
   //    for (uint i = 0; i < OPERATORS.size(); ++i) {
   //       //handle unary vs. binary
   //       if (banBinary && !+(OPERATORS[i].opType & OpType::BIN)) {
   //          continue;
   //       }
   //       //check length
   //       if (OPERATORS[i].size() > str.size()) {
   //          continue;
   //       }
   //       //find first differing char
   //       for (uint j = 0; j < OPERATORS[i].size(); ++j) {
   //          if (str[j] == OPERATORS[i].opStr[j]) {
   //             if (j >= maxlen) {
   //                maxlen = j + 1;
   //                op = &OPERATORS[i];
   //             }
   //          }
   //          else {
   //             break;
   //          }
   //       }
   //    }

   //    return op;
   // }

   //!function to determine if a string is a Middle-C keyword
   template<mcsl::str_t strT> inline bool isKeyword(const strT& str) { return KEYWORDS.contains(str); }
}

#endif //DATA_HPP