#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

// #include "CLEF.hpp"
#include "CLEF_DEFINES.hpp"
#include <utility>
#include <bit>

namespace clef {
   //!enum of CLEF error codes
   enum class ErrCode {
      UNSPEC = 0,

      LEXER_UNSPEC,

      PARSER_UNSPEC,
      PARSER_NOT_IMPLEMENTED,
      //syntax errors
      UNCLOSED_BLOCK,
      BAD_PARAM_BLOCK,
      BAD_LITERAL,
      BAD_CMNT,
      BAD_IDEN,
      //ast/ast node errors
      BAD_LIT_CAST,
      BAD_NODE_CAST,
   };
   constexpr auto      operator+(const ErrCode t) noexcept { return std::to_underlying(t); }
   
   //!enum of AST node type codes
   enum class [[clang::flag_enum]] NodeType : uint8 {
      ERROR = 0xFF,  //CLEF INTERNAL ERROR
      NONE = 0x00,   //to-be determined

      IDEN,
         VAR,
         FUNC,
         TYPE,
            VAR_PARAM,
            FUND_TYPE,
            FUNC_SIG,
            ENUM,
            MASK,
            UNION,
            NAMESPACE,
            INTERFACE,
            STRUCT,
            CLASS,
            GENERIC,
      SCOPE,
      LITERAL,
      OPERATOR,
      EXPR,
         STMT,
            DECL,
            LOOP,
            IF,
            SWITCH,
            MATCH,
            // ASM,
            TRY_CATCH,
      FOR_LOOP_PARAMS,
      FOREACH_LOOP_PARAMS,
      SWITCH_CASES,
      MATCH_CASES,
      STMT_SEQ,
      ARG_LIST,
      PARAM_LIST,
   };
   constexpr auto operator+(const NodeType t) noexcept { return std::to_underlying(t); }
   constexpr bool canDownCastTo(const NodeType from, const NodeType to) {
      switch (to) {
         case NodeType::IDEN:
            return from >= NodeType::IDEN && from <= NodeType::GENERIC;
         case NodeType::TYPE:
            return from >= NodeType::TYPE && from <= NodeType::GENERIC;
         case NodeType::EXPR:
            return from >= NodeType::EXPR && from <= NodeType::TRY_CATCH;
         case NodeType::STMT:
            return from >= NodeType::STMT && from <= NodeType::TRY_CATCH;
         
         default:
            return from == to;
      };
   }
   
   //!token types bitmask
   enum class [[clang::flag_enum]] TokenType : uint8 {
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
      CMNT = PTXT | OP,    //comment
      CHAR = STRT | IDEN,  //character
      DGIT = IDEN | NUM,   //base-10 digit
      XDGT = DGIT | CHAR,  //hex digit

      OPLIKE = EOS | PTXT | BLOC | ESC | OP, //syntatically vaguely like an operator

      ANY = 0xFF
   };
   constexpr auto      operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }
   constexpr TokenType operator|(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) | (+rhs)); }
   constexpr TokenType operator*(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) * (+rhs)); }
   constexpr TokenType operator~(const TokenType lhs) noexcept { return (TokenType)(~+lhs); }
   constexpr TokenType operator*(const TokenType lhs, const uint rhs) noexcept { return (TokenType)((+lhs) * rhs); }

   //operator properties bitmask
   enum class [[clang::flag_enum]] OpType : uint8 {
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
      BLOCK_DELIM = 7_m,
      TYPE_MOD    = 8_m,


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
   enum class BlockType : uint8 {
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
   constexpr auto operator+(const BlockType t) noexcept { return std::to_underlying(t); }
   //!literal type specification
   enum class LitType : uint8 {
      NONE = 0x00,
      UINT,
      SINT,
      FLOAT,
      CHAR,
      STRING,
      FORMAT,
      REGEX,
      TYPEID,
   };
   //!type qualifier bitmask
   enum class [[clang::flag_enum]] TypeQualMask : uint16 {
      NONE           =  0_m,

      CONST          =  1_m,
      CONSTEXPR      =  2_m,
      IMMEDIATE      =  3_m,
      READONLY       =  4_m,

      MUTABLE        =  5_m,
      VOLATILE       =  6_m,
      ATOMIC         =  7_m,
      
      EXTERN         =  8_m,
      INLINE         =  9_m,
      STATIC         = 10_m,
      
      POINTER        = 11_m,  //C-style raw pointer
      SHARED         = 12_m,  //shared pointer
      UNIQUE         = 13_m,  //unique pointer (owning)
      WEAK           = 14_m,  //weak pointer
      REFERENCE      = 15_m,  //C++-style reference
      ITERATOR       = 16_m,  //iterator (similar to C++ iterators)
   };
   constexpr auto operator+(TypeQualMask mask) { return std::to_underlying(mask); }
   constexpr TypeQualMask operator&(TypeQualMask lhs, TypeQualMask rhs) { return (TypeQualMask)(+lhs & +rhs); }

   //!symbol properties bitmask
   enum class [[clang::flag_enum]] SymbolProp : uint64 {
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
      LABEL          = 10_m,  //ASM-style label

      OBJECT         = 11_m,  //symbol represents an instantiation of a type
      TYPE           = 12_m,  //symbol names a type
      NAMESPACE      = 13_m,  //symbol names a namespace
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
      READONLY       = 24_m,
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


   enum class KeywordID : uint8 {
      __TYPE         = 8_m,
         __NUMERIC      = 7_m, __NUMERIC_TYPE = __TYPE | __NUMERIC,
            __FIXED_WIDTH  = 6_m,
            __FLOATING     = 5_m,
            __SIGNED       = 4_m,
         __TEXT         = 4_m,
      __TYPE_ADJACENT = 7_m,
         __OBJECT_TYPE  = 6_m,
         __QUALIFIER    = 5_m,
         __CAST         = 4_m,
      __CONTROL_FLOW = 6_m,
      ___LAST_SPEC = __CONTROL_FLOW,

      _8_BITS        = 1,


      _NOT_A_KEYWORD = 0,


      VOID           = __TYPE + 1,
      AUTO,


      CHAR           = __TYPE | __TEXT,
      CHAR_UTF_8     = __TYPE | __TEXT | __FIXED_WIDTH | _8_BITS,
      CHAR_UTF_16,
      CHAR_UTF_32,
      

      BOOL           = __TYPE | __NUMERIC,
      UBYTE,
      USHORT,
      UINT,
      ULONG,

      SIGN_T         = __TYPE | __NUMERIC | __SIGNED,
      SBYTE,
      SSHORT,
      SINT,
      SLONG,


      UINT_8         = __TYPE | __NUMERIC | __FIXED_WIDTH | _8_BITS,
      UINT_16,
      UINT_32,
      UINT_64,
      UINT_128,
      UINT_256,

      SINT_8         = UINT_8 | __SIGNED,
      SINT_16,
      SINT_32,
      SINT_64,
      SINT_128,
      SINT_256,

      FLOAT             = SINT | __FLOATING,

      FLOAT_16          = SINT_16 | __FLOATING,
      FLOAT_32,
      FLOAT_64,
      FLOAT_128,
      FLOAT_256,







      CLASS             = __TYPE_ADJACENT | __OBJECT_TYPE + 1,
      STRUCT,
      INTERFACE,
      UNION,
      ENUM,
      MASK,
      NAMESPACE,
      FUNC,

      CONST             = __TYPE_ADJACENT | __QUALIFIER + 1,
      CONSTEXPR,
      IMMEDIATE,
      READONLY,
      MUTABLE,
      VOLATILE,
      ATOMIC,
      EXPLICIT,

      CAST              = __TYPE_ADJACENT | __CAST + 1,
      UP_CAST,
      DYN_CAST,
      BIT_CAST,
      CONST_CAST,

      TYPEOF            = __TYPE_ADJACENT + 1,
      TYPEID,
      TYPENAME,
      SIZEOF,
      ARRLEN,
      ALIGNAS,
      ALIGNOF,
      

      GOTO              = __CONTROL_FLOW + 1,
      IF,
      ELSE,
      FOR,
      FOREACH,
      WHILE,
      DO,
      BREAK,
      CONTINUE,
      SWITCH,
      MATCH,
      CASE,
      DEFAULT,
      TRY,
      CATCH,
      THROW,




      THIS = 1,
      SELF,

      NEW,
      DELETE,

      ASSERT,
      STATIC_ASSERT,

      TRUE,
      FALSE,

      NULLPTR,

      RETURN,

      USING,

      // ASM,

      TEMPLATE,
   };
   constexpr auto operator+(const KeywordID t) noexcept { return std::to_underlying(t); }
   constexpr bool isType(const KeywordID id) noexcept { return +id & +KeywordID::__NUMERIC; }
      constexpr bool isNumeric(const KeywordID id) noexcept { return +id & +KeywordID::__NUMERIC_TYPE; }
         constexpr bool isFloatingPoint(const KeywordID id) noexcept { return +id & (+KeywordID::__NUMERIC_TYPE | +KeywordID::__FLOATING); }
         constexpr bool isInteger(const KeywordID id) noexcept { return ~(+id & +KeywordID::__NUMERIC_TYPE) & +KeywordID::__FLOATING; }
            constexpr bool isSint(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__NUMERIC_TYPE | +KeywordID::__SIGNED)) & +KeywordID::__FLOATING; }
            constexpr bool isUint(const KeywordID id) noexcept { return ~(+id & +KeywordID::__NUMERIC_TYPE) & (+KeywordID::__FLOATING | +KeywordID::__SIGNED); }
      constexpr bool isText(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE | +KeywordID::__TEXT)) & (+KeywordID::__NUMERIC); }
         constexpr bool isASCII(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE | +KeywordID::__TEXT)) & (+KeywordID::__NUMERIC | +KeywordID::__FIXED_WIDTH); }
         constexpr bool isUnicode(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE | +KeywordID::__TEXT | +KeywordID::__FIXED_WIDTH)) & +KeywordID::__NUMERIC; }
   constexpr bool isTypeAdjacent(const KeywordID id) noexcept { return ~(+id & +KeywordID::__TYPE_ADJACENT) & +KeywordID::__TYPE; }
      constexpr bool isObjectType(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE_ADJACENT | +KeywordID::__OBJECT_TYPE)) & +KeywordID::__TYPE; }
      constexpr bool isQualifier(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE_ADJACENT | +KeywordID::__QUALIFIER)) & (+KeywordID::__TYPE | +KeywordID::__OBJECT_TYPE); }
      constexpr bool isCast(const KeywordID id) noexcept { return ~(+id & (+KeywordID::__TYPE_ADJACENT | +KeywordID::__CAST)) & (+KeywordID::__TYPE | +KeywordID::__OBJECT_TYPE | +KeywordID::__QUALIFIER); }
   constexpr bool isControlFlow(const KeywordID id) noexcept { return ~(+id & +KeywordID::__CONTROL_FLOW) & +KeywordID::__NUMERIC_TYPE; }
   constexpr bool isUnspec(const KeywordID id) noexcept { return +id < +KeywordID::___LAST_SPEC; }

   enum class FundTypeID : uint8 {
      NULL,
      FUNCTION_SIGNATURE,
   };

   #define toenum0() [](){constexpr char buf[4] = "\0\0\0"; return std::bit_cast<unsigned int>(buf); }()
   #define toenum1(str) [](){constexpr char buf[4] = str "\0\0"; return std::bit_cast<unsigned int>(buf); }()
   #define toenum2(str) [](){constexpr char buf[4] = str "\0"; return std::bit_cast<unsigned int>(buf); }()
   #define toenum3(str) [](){constexpr char buf[4] = str; return std::bit_cast<unsigned int>(buf); }()
   #define _CLOSE_LIST_CHAR "}" //get around broken vscode colorization
   enum class OperatorID : uint32 {
      NULL = toenum0(),

      ESCAPE = toenum1("\\"),
      EOS = toenum1(";"),

      STRING = toenum1("\""),
      CHAR = toenum1("\'"),

      LINE_CMNT = toenum2("/" "/"),
      BLOCK_CMNT_OPEN = toenum2("/" "*"),
      BLOCK_CMNT_CLOSE = toenum2("*" "/"),

      CALL_OPEN = toenum1("("),
      CALL_CLOSE = toenum1(")"),
      CALL = toenum2("()"),
      SUBSCRIPT_OPEN = toenum1("["),
      SUBSCRIPT_CLOSE = toenum1("]"),
      LIST_OPEN = toenum1("{"),
      LIST_CLOSE = toenum1(_CLOSE_LIST_CHAR),
      SPECIALIZER_OPEN = toenum1("<"),
      SPECIALIZER_CLOSE = toenum1(">"),


      PREPROCESSOR = toenum1("#"),

      SCOPE_RESOLUTION = toenum2("::"),

      LABEL_DELIM = toenum1(":"),

      INCREMENT = toenum2("++"),
      DECREMENT = toenum2("--"),

      MEMBER_ACCESS = toenum1("."),
      MEMBER_OF_POINTER_ACCESS = toenum2("->"),
      POINTER_TO_MEMBER = toenum2(".*"),
      POINTER_TO_MEMBER_OF_POINTER = toenum3("->*"),

      RANGE = toenum2(".."),
      SPREAD = toenum3("..."),
      
      ADD = toenum1("+"),
      SUB = toenum1("-"),
      MUL = toenum1("*"),
      DIV = toenum1("/"),
      MOD = toenum1("%"),
      EXP = toenum2("^^"),
      
      LOGICAL_NOT = toenum1("!"),
      LOGICAL_AND = toenum2("&&"),
      LOGICAL_OR = toenum2("||"),

      BIT_NOT = toenum1("~"),
      BIT_AND = toenum1("&"),
      BIT_OR = toenum1("|"),
      BIT_XOR = toenum1("^"),
      SHIFT_LEFT = toenum2("<<"),
      SHIFT_RIGHT = toenum2(">>"),
      
      
      UNIQUE_PTR = toenum1("@"),
      SHARED_PTR = toenum1("$"),
      WEAK_PTR = toenum1("`"),
      

      THREE_WAY_COMP = toenum3("<=>"),
      LESSER = toenum1("<"),
      GREATER = toenum1(">"),
      LESSER_OR_EQ = toenum2("<="),
      GREATER_OR_EQ = toenum2(">="),

      EQUAL = toenum2("=="),
      UNEQUAL = toenum2("!="),
      // EQUAL_STRICT = toenum3("==="),
      // UNEQUAL_STRICT = toenum3("!=="),

      COALESCE = toenum2("??"),

      INLINE_IF = toenum1("?"),
      INLINE_ELSE = toenum1(":"),

      ASSIGN = toenum1("="),
      // CONST_ASSIGN = toenum2(":="),
      ADD_ASSIGN = toenum2("+="),
      SUB_ASSIGN = toenum2("-="),
      MUL_ASSIGN = toenum2("*="),
      DIV_ASSIGN = toenum2("/="),
      MOD_ASSIGN = toenum2("%="),
      EXP_ASSIGN = toenum3("^^="),
      SHL_ASSIGN = toenum3("<<="),
      SHR_ASSIGN = toenum3(">>="),
      AND_ASSIGN = toenum2("&="),
      XOR_ASSIGN = toenum2("^="),
      OR_ASSIGN = toenum2("|="),
      COALESCE_ASSIGN = toenum3("??="),

      COMMA = toenum1(","),



      //keyword pseudo-operators
      _LOOPS = toenum3("\0\0a"),
      FOR = _LOOPS | toenum2("\0a"),
      FOREACH = _LOOPS | toenum2("\0b"),
      WHILE = _LOOPS | toenum2("\0c"),
      DO_WHILE = _LOOPS | toenum2("\0d"),

      IF = toenum3("\0\0e"),

      SWITCH = toenum3("\0\0f"),
      MATCH = toenum3("\0\0g"),

      TRY_CATCH = toenum3("\0\0h"),
      // ASM = toenum3("\0\0i"),

      //other pseudo-operators
      DECL = toenum3("\0\0A"),

      //helpers
      FIRST_CHAR_MASK = toenum3("\xff\0\0"),
   };
   #undef toenum0
   #undef toenum1
   #undef toenum2
   #undef toenum3
   #undef _CLOSE_LIST_CHAR

   constexpr auto operator+(const OperatorID x) { return std::to_underlying(x); }
   constexpr OperatorID operator&(const OperatorID lhs, const OperatorID rhs) { return (OperatorID)(+lhs & +rhs); }
   constexpr OperatorID& operator&=(OperatorID& lhs, const OperatorID rhs) { lhs = lhs & rhs; return lhs; }
   constexpr bool isOperator(const OperatorID x) { return +x & +OperatorID::FIRST_CHAR_MASK; }
   constexpr OperatorID bytemask(OperatorID e) {
      char* str = std::bit_cast<char*>(&e);
      for (uint i = 0; i < sizeof(e); ++i) {
         str[i] = str[i] ? ~0 : 0;
      }
      return e;
   }
   constexpr uint opidlen(const OperatorID x) {
      const char* str = std::bit_cast<const char*>(&x);
      for (uint i = 0; i < sizeof(OperatorID); ++i) {
         if (!str[i]) { return i; }
      }
      return sizeof(OperatorID); 
   }
   constexpr bool opidcmp(const OperatorID x, const char* str) { //!NOTE: succeptible to segfaults
      OperatorID op = *std::bit_cast<const OperatorID*>(str);
      op &= bytemask(x);
      return op == x;
   }

   constexpr bool isDecl(const OperatorID op) { return op == OperatorID::DECL; }

   constexpr bool isForLoop(const OperatorID op) { return op == OperatorID::FOR; }
   constexpr bool isForeachLoop(const OperatorID op) { return op == OperatorID::FOREACH; }
   constexpr bool isWhileLoop(const OperatorID op) { return op == OperatorID::WHILE; }
   constexpr bool isDoWhileLoop(const OperatorID op) { return op == OperatorID::DO_WHILE; }

   constexpr bool isLoop(const OperatorID op) { return +(op & OperatorID::_LOOPS); }
   constexpr bool isSimpleLoop(const OperatorID op) { return isLoop(op) && !isForLoop(op) && !isForeachLoop(op); }

   constexpr bool isIf(const OperatorID op) { return op == OperatorID::IF; }
   constexpr bool isSwitch(const OperatorID op) { return op == OperatorID::SWITCH; }
   constexpr bool isMatch(const OperatorID op) { return op == OperatorID::MATCH; }
}

#endif //ENUMS_HPP