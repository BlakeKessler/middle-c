#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"
#include "raw_str.hpp"
#include "raw_buf_str.hpp"
#include "static_arr.hpp"
#include "pair.hpp"

#include <unordered_set>
#include <cassert>
#include <cstring>

//operator string + metadata
struct clef::Operator {
   mcsl::raw_str<MAX_OP_LEN> opStr;
   byte size;
   byte precedence;
   OpType opType; //unary/binary/special, left/right associative

   constexpr Operator(): opStr(""),size(0),precedence(0),opType(static_cast<OpType>(0)) {}
   constexpr Operator(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type):
      opStr(""),size(0),precedence(prec),opType(type) {
         //strlen
         for (; size < opStr.size(); ++size) {
            if (!str[size]) { break; }
         }
         //bounds-check
         assert(size <= MAX_OP_LEN);
         
         //copy string
         for (uint i = 0; i < size; ++i) {
            opStr[i] = str[i];
         }
   }
};
//delimiter pair strings
struct clef::DelimPair {
   mcsl::raw_buf_str<MAX_OP_LEN, byte> open;
   mcsl::raw_buf_str<MAX_OP_LEN, byte> close;
};

using enum clef::OpType;
namespace clef {
   extern const mcsl::static_arr<TokenType,256> tokTypeArr;

   static constexpr const mcsl::static_arr OPERATORS{
      Operator("#",     0,  LEFT_UN),   //invoke preprocessor

      Operator("\"",    1,  LEFT_SPEC), //string
      Operator("\'",    1,  LEFT_SPEC), //char
      Operator("//",    1,  RIGHT_SPEC),//line comment
      Operator("/*",    1,  RIGHT_SPEC),//block comment
      Operator("*/",    1,  LEFT_SPEC), //block comment

      Operator("::",    2,  LEFT_BIN),  //scope resolution

      Operator("++",    3,  LEFT_UN),   //pre-increment
      Operator("--",    3,  LEFT_UN),   //pre-decrement
      Operator("(",     3,  RIGHT_SPEC),//function calls/functional casts
      Operator("[",     3,  RIGHT_SPEC),//subscript
      Operator("{",     3,  RIGHT_SPEC),//scope/functional casts
      Operator("<",     3,  RIGHT_SPEC),//specifier
      Operator(")",     3,  LEFT_SPEC), //function calls/functional casts
      Operator("]",     3,  LEFT_SPEC), //subscript
      Operator("}",     3,  LEFT_SPEC), //scope/functional casts
      Operator(">",     3,  LEFT_SPEC), //specifier
      Operator(".",     3,  LEFT_BIN),  //element access
      Operator("->",    3,  LEFT_BIN),  //element access
      Operator("?",     3,  LEFT_SPEC), //safe access modifier
      Operator("..",    3,  LEFT_BIN),  //range
      Operator("...",   3,  LEFT_BIN),  //array spread

      Operator("++",    4,  RIGHT_UN),  //post-increment
      Operator("--",    4,  RIGHT_UN),  //post-decrement
      Operator("+",     4,  RIGHT_UN),  //unary plus
      Operator("-",     4,  RIGHT_UN),  //integer negation
      Operator("!",     4,  RIGHT_UN),  //logical negation
      Operator("~",     4,  RIGHT_UN),  //bitwise negation
      Operator("&",     4,  RIGHT_UN),  //reference/address of
      Operator("*",     4,  RIGHT_UN),  //raw pointer/dereference
      Operator("@",     4,  RIGHT_UN),  //unique pointer
      Operator("$",     4,  RIGHT_UN),  //shared pointer
      Operator("\\",    4,  RIGHT_UN),  //weak pointer
      Operator("%",     4,  RIGHT_UN),  //iterator
      
      Operator(".*",    5,  LEFT_BIN),  //pointer to member
      Operator("->*",   5,  LEFT_BIN),  //pointer to member

      Operator("**",    6,  LEFT_BIN),  //exponentiation

      Operator("*",     7,  LEFT_BIN),  //multiplication
      Operator("/",     7,  LEFT_BIN),  //division
      Operator("%",     7,  LEFT_BIN),  //modulo

      Operator("+",     8,  LEFT_BIN),  //addition
      Operator("-",     8,  LEFT_BIN),  //subtraction

      Operator("<<",    9,  LEFT_BIN),  //left bit-shift
      Operator(">>",    9,  LEFT_BIN),  //right bit-shift

      Operator("<=>",   10, LEFT_BIN),  //three-way comparison

      Operator(">",     11, LEFT_BIN),  //less than
      Operator("<",     11, LEFT_BIN),  //greater than
      Operator("<=",    11, LEFT_BIN),  //less than or equal to
      Operator(">=",    11, LEFT_BIN),  //greather than or equal to

      Operator("==",    12, LEFT_BIN),  //equality
      Operator("!=",    12, LEFT_BIN),  //inequality
      Operator("===",   12, LEFT_BIN),  //strict equality
      Operator("!==",   12, LEFT_BIN),  //strict inequality

      Operator("&",     13, LEFT_BIN),  //bitwise AND
      Operator("^",     14, LEFT_BIN),  //bitwise XOR
      Operator("|",     15, LEFT_BIN),  //bitwise OR
      Operator("&&",    16, LEFT_BIN),  //logical AND
      Operator("||",    17, LEFT_BIN),  //logical OR

      Operator("\?\?",  18, RIGHT_BIN), //null coalescing
      Operator("?",     18, RIGHT_BIN), //inline if
      Operator(":",     18, RIGHT_BIN), //inline else
      Operator("=",     18, RIGHT_BIN), //direct assignment
      Operator(":=",    18, RIGHT_BIN), //const assignment
      Operator("+=",    18, RIGHT_BIN), //compound assignment (add)
      Operator("-=",    18, RIGHT_BIN), //compound assignment (sub)
      Operator("*=",    18, RIGHT_BIN), //compound assignment (mul)
      Operator("/=",    18, RIGHT_BIN), //compound assignment (div)
      Operator("%=",    18, RIGHT_BIN), //compound assignment (mod)
      Operator("**=",   18, RIGHT_BIN), //compound assignment (exp)
      Operator("<<=",   18, RIGHT_BIN), //compound assignment (leftshift)
      Operator(">>=",   18, RIGHT_BIN), //compound assignment (rightshift)
      Operator("&=",    18, RIGHT_BIN), //compound assignment (AND)
      Operator("^=",    18, RIGHT_BIN), //compound assignment (XOR)
      Operator("|=",    18, RIGHT_BIN), //compound assignment (OR)
      Operator("\?\?=", 18, RIGHT_BIN), //compound assignment (null coalescing)

      Operator(",",     19, LEFT_SPEC | BIN)  //comma

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
   // static const std::unordered_set<mcsl::string> KEYWORDS{
   //    "void", "auto",
   //    "uint", "ushort", "ulong", "ubyte",
   //    "sint", "sshort", "slong", "sbyte",
   //    "bool", "float", "num",
   //    "char", "char_utf8", "char_utf16", "char_utf32",
   //    "uint8", "uint16", "uint32", "uint64", "uint128", "uint256",
   //    "sint8", "sint16", "sint32", "sint64", "sint128", "sint256",
   //    "float16", "float32", "float64", "float128", "float256",
   //    "half", "single", "double", "quad", "oct",
   //    "num8", "num16", "num32", "num64", "num128", "num256",
      
   //    "true", "false", "nullptr",

   //    "const", "constexpr", "immediate", "mutable", "volatile", "atomic",
   //    "class", "struct", "union", "enum", "namespace",
   //    "private", "protected", "public", "friend",
   //    "extern", "inline", "static", "virtual", "override", "explicit", "noexcept",
   //    "this", "self",

   //    "new", "delete", "sizeof", "arrsizeof",
   //    "typeof", "typeid", "typename", "alignas", "alignof",
   //    "template",
   //    "using",

   //    "asm",
   //    "return",
   //    "operator",
   //    "assert", "static_assert",
   //    "type_cast", "bit_cast", "safe_cast", "quick_cast", "const_cast",

   //    "if", "else", "for", "while", "do", "break", "continue", "switch", "match", "case", "default", "goto",
   //    "try", "catch", "throw"
   // };
   
   //block delims
   DelimPairType blockDelimType(const mcsl::raw_str_span& str);

   //operators
   uint maxOpLen(const mcsl::raw_str_span& str);
   const Operator* getOpData(const mcsl::raw_str_span& str, bool allowUnary = true);

   //!function to determine if a string is a Middle-C keyword
   inline bool isKeyword(const mcsl::raw_str_span& str) { return false; /*return KEYWORDS.contains(str);*/ }
}

#endif //DATA_HPP