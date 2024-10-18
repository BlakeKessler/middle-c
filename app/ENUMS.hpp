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
   
   //!token types bitmask
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
   constexpr auto      operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }
   constexpr TokenType operator|(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) | (+rhs)); }
   constexpr TokenType operator*(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) * (+rhs)); }
   constexpr TokenType operator~(const TokenType lhs) noexcept { return (TokenType)(~+lhs); }
   constexpr TokenType operator*(const TokenType lhs, const uint rhs) noexcept { return (TokenType)((+lhs) * rhs); }

   //operator properties bitmask
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
   constexpr auto   operator+(const OpType t) noexcept { return std::to_underlying(t); }
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

   //!symbol properties bitmask
   enum class SymbolProp {
      NIL = 0,
      
      HAS_NAME_TABLE = 1 << 0,

      KEYWORD        = 1 << 1,   //symbol is a keyword

      FUNDAMENTAL    = 1 << 2,   //fundamental type
      STRUCT         = 1 << 3,   //class or struct type
      UNION          = 1 << 4,   //union-like type
      ENUM           = 1 << 5,   //enum type
      MASK           = 1 << 6,   //bitmask type
      INTERFACE      = 1 << 7,   //interface type
      FUNCTION       = 1 << 8,   //C-style function

      OBJECT         = 1 << 0,   //symbol represents an instantiation of a type
      TYPE           = 1 << 10,   //symbol names a type
      NAMESPACE      = 1 << 11,  //symbol names a namespace
      LABEL          = 1 << 12,  //symbol names an asm-like label
      DIRECTIVE      = 1 << 13,  //symbol represents a preprocessor directive

      CALLABLE       = 1 << 14,  //can be called like a function
      DEREFABLE      = 1 << 15,  //can be dereferenced like a pointer
      INDEXABLE      = 1 << 16,  //can be indexed like an array

      INITIALIZED    = 1 << 17,  //symbol refers to initialized memory
      IN_SCOPE       = 1 << 18,  //symbol is in currently in scope
      COMPLETE       = 1 << 19,  //symbol refers to a complete type

      //type qualifiers
      CONST          = 1 << 20,
      CONSTEXPR      = 1 << 21,
      IMMEDIATE      = 1 << 22,
      FINAL          = 1 << 23,
      MUTABLE        = 1 << 24,
      VOLATILE       = 1 << 25,
      ATOMIC         = 1 << 26,
      EXTERN         = 1 << 27,
      INLINE         = 1 << 28,
      STATIC         = 1 << 29,

      //pointer qualifiers
      POINTER        = 1 << 30,  //C-style raw pointer
      SHARED         = 1 << 31,  //shared pointer
      UNIQUE         = 1 << 32,  //unique pointer (owning)
      WEAK           = 1 << 33,  //weak pointer
      REFERENCE      = 1 << 34,  //C++-style reference
      ITERATOR       = 1 << 35,  //iterator (similar to C++ iterators)
   };
   constexpr auto   operator+(const SymbolProp t) noexcept { return std::to_underlying(t); }
   constexpr SymbolProp operator&(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) & (+rhs)); }
   constexpr SymbolProp operator|(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) | (+rhs)); }
   constexpr SymbolProp operator^(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) ^ (+rhs)); }
   constexpr SymbolProp operator~(const SymbolProp lhs) noexcept { return (SymbolProp)(~+lhs); }
   constexpr SymbolProp operator*(const SymbolProp lhs, const uint rhs) noexcept { return (SymbolProp)((+lhs) * rhs); }

}

#endif //ENUMS_HPP