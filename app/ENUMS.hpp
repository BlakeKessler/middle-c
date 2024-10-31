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
   constexpr auto      operator+(const ErrCode t) noexcept { return std::to_underlying(t); }
   
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
      NONE = 0_m, //not a valid member of a token (whitespace)
      EOS  = 1_m, //end of line of code
      PTXT = 2_m, //plaintext segment delimiters (strings, comments)
      BLOC = 3_m, //general block delimiter
      ESC  = 4_m, //escape character
      OP   = 5_m, //operator
      STRT = 6_m, //first character of an identifier
      IDEN = 7_m, //identifier (not first character)
      NUM  = 8_m, //base-10 digit

      DLIM = BLOC | OP,    //delimiter
      CHAR = STRT | IDEN,  //character
      DGIT = IDEN | NUM,   //base-10 digit
      XDGT = DGIT | CHAR,  //hex digit
      CMNT = PTXT | OP,    //comment

      ANY = 0xFF
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
      FREE        = 0_m,
      //associativity
      LEFT_ASSOC  = 1_m,
      RIGHT_ASSOC = 2_m,

      //operand count
      UNARY       = 3_m,
      BINARY      = 4_m,

      //other
      BLOCK_DELIM = 6_m,
      TYPE_MOD    = 7_m,


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
   // //!literal type specification
   // enum class LitType : byte {
   //    NONE = 0x00,
   //    INT = +TokenType::ILIT,
   //    FLOAT = +TokenType::FLIT,
   //    CHAR = 0x03,
   //    STRING,
   // };

   //!symbol properties bitmask
   enum class SymbolProp : luint {
      NIL            = 0_m,
      
      HAS_NAME_TABLE = 1_m,

      KEYWORD        = 2_m,   //symbol is a keyword

      FUNDAMENTAL    = 3_m,   //fundamental type
      STRUCT         = 4_m,   //class or struct type
      UNION          = 5_m,   //union-like type
      ENUM           = 6_m,   //enum type
      MASK           = 7_m,   //bitmask type
      INTERFACE      = 8_m,   //interface type
      FUNCTION       = 9_m,   //C-style function

      OBJECT         = 10_m,  //symbol represents an instantiation of a type
      TYPE           = 11_m,   //symbol names a type
      NAMESPACE      = 12_m,  //symbol names a namespace
      LABEL          = 13_m,  //symbol names an asm-like label
      DIRECTIVE      = 14_m,  //symbol represents a preprocessor directive

      CALLABLE       = 15_m,  //can be called like a function
      DEREFABLE      = 16_m,  //can be dereferenced like a pointer
      INDEXABLE      = 17_m,  //can be indexed like an array

      INITIALIZED    = 18_m,  //symbol refers to initialized memory
      IN_SCOPE       = 19_m,  //symbol is in currently in scope
      COMPLETE       = 20_m,  //symbol refers to a complete type

      //type qualifiers
      CONST          = 21_m,
      CONSTEXPR      = 22_m,
      IMMEDIATE      = 23_m,
      FINAL          = 24_m,
      MUTABLE        = 25_m,
      VOLATILE       = 26_m,
      ATOMIC         = 27_m,
      EXTERN         = 28_m,
      INLINE         = 29_m,
      STATIC         = 30_m,

      //pointer qualifiers
      POINTER        = 31_m,  //C-style raw pointer
      SHARED         = 32_m,  //shared pointer
      UNIQUE         = 33_m,  //unique pointer (owning)
      WEAK           = 34_m,  //weak pointer
      REFERENCE      = 35_m,  //C++-style reference
      ITERATOR       = 36_m,  //iterator (similar to C++ iterators)
   };
   constexpr auto       operator+(const SymbolProp t) noexcept { return std::to_underlying(t); }
   constexpr SymbolProp operator&(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) & (+rhs)); }
   constexpr SymbolProp operator|(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) | (+rhs)); }
   constexpr SymbolProp operator^(const SymbolProp lhs, const SymbolProp rhs) noexcept { return (SymbolProp)((+lhs) ^ (+rhs)); }
   constexpr SymbolProp operator~(const SymbolProp lhs) noexcept { return (SymbolProp)(~+lhs); }
   constexpr SymbolProp operator*(const SymbolProp lhs, const uint rhs) noexcept { return (SymbolProp)((+lhs) * rhs); }

}

#endif //ENUMS_HPP