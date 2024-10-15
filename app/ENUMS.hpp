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

      LEXER_UNSPEC,

      PARSER_UNSPEC,
      //syntax errors
      UNCLOSED_BLOCK,
      INVALID_LIT
   };
   
   //!enum of AST node type codes
   enum class NodeType : byte {
      ERROR = 0xFF,        //CLEF INTERNAL ERROR
      NONE = 0x00,         //to-be determined

      COMMENT,             //commented out code
      LITERAL,             //numeric, string, or character literal

      TYPE,                //fundamental or object type
      OBJ,                 //instantiation of a type
      QUAL,                //type qualifier (EX: const, static, &, *)
      IDEN,                //identifier
      DECL,                //declaration
      DEF,                 //definition  
      
      STATEMENT,           //statement (AKA: line of code)
      
      BLOCK,               //block-delimiter delimited block

      OPERATOR             //MiddleC operator (binary infix, unary prefix, or unary postfix)
   };
   constexpr auto operator+(const NodeType t) noexcept { return std::to_underlying(t); }
   
   //!enum of token types
   enum class TokenType : byte {
      NONE = 0x00, //not a valid member of a token (whitespace)
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
      CMNT = PTXT | OP,
   };
   constexpr auto operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }
   constexpr TokenType operator|(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) | (+rhs)); }
   constexpr TokenType operator*(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) * (+rhs)); }
   constexpr TokenType operator~(const TokenType lhs) noexcept { return (TokenType)(~+lhs); }
   constexpr TokenType operator*(const TokenType lhs, const uint rhs) noexcept { return (TokenType)((+lhs) * rhs); }

   enum class OpType : byte {
      //!bitmask atoms
      //unused (available for user-definition)
      FREE        =          0,
      //associativity
      LEFT_ASSOC  =        0b1,
      RIGHT_ASSOC =       0b10,

      //operand count
      UNARY       =      0b100,
      BINARY      =     0b1000,

      //other
      BLOCK_DELIM =   0b100000,
      TYPE_MOD    =  0b1000000,


      //!full operator types
      PREFIX      = UNARY  | RIGHT_ASSOC,
      POSTFIX     = UNARY  | LEFT_ASSOC,
      INFIX_LEFT  = BINARY | LEFT_ASSOC,
      INFIX_RIGHT = BINARY | RIGHT_ASSOC,

   };
   constexpr auto operator+(const OpType t) noexcept { return std::to_underlying(t); }
   constexpr OpType operator&(const OpType lhs, const OpType rhs) noexcept { return (OpType)((+lhs) & (+rhs)); }
   constexpr OpType operator|(const OpType lhs, const OpType rhs) noexcept { return (OpType)((+lhs) | (+rhs)); }
   constexpr OpType operator*(const OpType lhs, const OpType rhs) noexcept { return (OpType)((+lhs) * (+rhs)); }
   constexpr OpType operator~(const OpType lhs) noexcept { return (OpType)(~+lhs); }
   constexpr OpType operator*(const OpType lhs, const uint rhs) noexcept { return (OpType)((+lhs) * rhs); }

   //delimiter pair specification
   enum class DelimPairType : byte {
      NONE = 0x00,      //not a block type

      PARENS = 0x01,    //PARENTHESES
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