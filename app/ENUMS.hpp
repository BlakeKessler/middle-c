#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

// #include "CLEF.hpp"
#include "CLEF_DEFINES.hpp"
#include <utility>

namespace clef {
   //!enum of CLEF error codes
   enum class ErrCode {
      UNSPEC = 0,
      PARSER_UNSPEC,
      UNCLOSED_BLOCK,
      SEGFAULT,
      ALLOC_FAIL,
   };
   
   //!enum of AST node type codes
   enum class NodeType : byte {
      ERROR = 0xFF,        //CLEF INTERNAL ERROR
      NONE = 0x00,         //to-be determined
      COMMENT,             //commented out code

      KEYWORD,             //MiddleC keyword
      IDEN,                //identifier
      DECL,                //declaration
      DEF,                 //definition
      EOS,                 //end of statement

      STATEMENT,           //statement

      ESCAPE,              //escape character

      DELIM_PAIR,          //opening and closing pair of block delimeter
      DELIM_GEN,           //general block delimiter
      DELIM_OPEN,          //opening block delimiter
      DELIM_CLOSE,         //closing block delimiter
      LITERAL,             //numeric or string literal
      OPERATOR,            //MiddleC operator
      OP_OR_DELIM,         //not yet determined between OPERATOR and DELIM_GEN
   };
   constexpr auto operator+(const NodeType t) noexcept { return std::to_underlying(t); }
   //!enum of token types
   enum class TokenType : byte {
      NONE = 0x00, //not a valid member of a token
      EOS  = 0x01, //end of line of code
      PTXT = 0x02, //plaintext segment delimiters (strings, comments)
      BLOC = 0x04, //general block delimiter
      OP   = 0x08, //operator
      STRT = 0x10, //first character of an identifier
      IDEN = 0x20, //identifier (not first character)
      ILIT = 0x40, //integer literal
      FLIT = 0x80, //floating point literal
                   //anything else -> interpret as bitmask

      DLIM = BLOC | OP,
      NUM  = ILIT | FLIT,
      DGIT = IDEN | NUM,
      CHAR = STRT | IDEN,
      XDGT = DGIT | CHAR,
      UNCD = CHAR | OP,
   };
   constexpr auto operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }
   constexpr TokenType operator|(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) | (+rhs)); }
   constexpr TokenType operator*(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) * (+rhs)); }
   constexpr TokenType operator~(const TokenType lhs) noexcept { return (TokenType)(~+lhs); }
   constexpr TokenType operator*(const TokenType lhs, const uint rhs) noexcept { return (TokenType)((+lhs) * rhs); }

   enum class OpType : byte {
      LEFT        = 0x10,
      RIGHT       = 0x20,
      UN          = 0x01,
      BIN         = 0x02,
      SPEC        = 0x08,


      LEFT_UN     =  LEFT | UN,  //left-associative, unary
      LEFT_BIN    =  LEFT | BIN, //left-associative, binary
      LEFT_SPEC   =  LEFT | SPEC,//left-associative, special
      RIGHT_UN    = RIGHT | UN,  //right-associative, unary
      RIGHT_BIN   = RIGHT | BIN, //right-associative, binary
      RIGHT_SPEC  = RIGHT | SPEC,//right-associative, special
      FREE        = 0x00,        //unused (available for any purpose for the user)

   };

   //delimiter pair specification
   enum class DelimPairType : byte {
      NONE = 0xFF,      //not a block type

      PARENS = 0x00,    //PARENTHESES
      SUBSCRIPT,        //SQUARE BRACKETS
      INIT_LIST,        //CURLY BRACKETS
      SPECIALIZER,      //TRIANGLE BRACKETS

      QUOTES_CHAR,      //CHARACTER
      QUOTES_STR,       //STRING

      TERNARY,          //ternary statement

      COMMENT_BLOCK,    //COMMENTED-OUT CODE
      COMMENT_LINE,     //COMMENTED OUT CODE
   };
   constexpr auto operator+(const DelimPairType t) noexcept { return std::to_underlying(t); }
   //!literal type specification
   enum class LitType : byte {
      NONE = 0x00,
      INT = +TokenType::ILIT,
      FLOAT = +TokenType::FLIT,
      CHAR = 0x03,
      STRING,
   };
}

#endif //ENUMS_HPP