#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

// #include "CLEF.hpp"
#include "CLEF_DEFINES.hpp"
#include <utility>
#include <bit>

#include <assert.hpp>

namespace clef {
   //!enum of CLEF error codes
   enum class ErrCode {
      UNSPEC = 0,

      LEXER_UNSPEC,
      LEXER_NOT_IMPLEMENTED,

      PARSER_UNSPEC,
      PARSER_NOT_IMPLEMENTED,
      //syntax errors
      BAD_PARAM_BLOCK,
      BAD_LITERAL,
      BAD_CMNT,
      BAD_IDEN,
      BAD_KEYWORD,
      MISSING_EOS,
      BAD_STMT,
      BAD_EXPR,
      BAD_BLOCK_DELIM,
      UNCLOSED_BLOCK,
      BAD_FUNC,
      BAD_DECL,
      //ast/ast node errors
      BAD_LIT_CAST,
      BAD_NODE_CAST,
   };
   constexpr auto      operator+(const ErrCode t) noexcept { return std::to_underlying(t); }
   
   //!enum of AST node type codes
   enum class [[clang::flag_enum]] NodeType : uint8 {
      ERROR = 0xFF,  //CLEF INTERNAL ERROR
      NONE = 0x00,   //to-be determined

      KEYWORD,
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
   enum class TokenType : uint8 {
      NONE = 0,
      
      IDEN,
      KEYWORD,
      INT_NUM,
      REAL_NUM,

      OP = 0x81,
      PREPROC_INIT,
      PREPROC_EOS,
      EOS,
      ESC,
      BLOCK_DELIM,
      PTXT_SEG,

      __OPLIKE = 0x80,
   };
   constexpr auto      operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }

   constexpr bool isOperator(const TokenType t) { return t == TokenType::OP; }
   constexpr bool isBlockLike(const TokenType t) { return t == TokenType::BLOCK_DELIM; }
   constexpr bool isOperand(const TokenType t) { return !+(t & TokenType::__OPLIKE); }
      constexpr bool isNumber(const TokenType t) { return t == TokenType::INT_NUM || t == TokenType::REAL_NUM; }
   #pragma region ops
   enum class OpID : uint8 {
      NULL = 0, //not an operator or no-op

      ESCAPE, //escape character
      EOS,

      STRING,
      CHAR,

      LINE_CMNT,
      BLOCK_CMNT,
         BLOCK_CMNT_OPEN,
         BLOCK_CMNT_CLOSE,

      CALL_INVOKE, //parens
         CALL_OPEN,
         CALL_CLOSE,
      SUBSCRIPT_INVOKE, //square brackets
         SUBSCRIPT_OPEN,
         SUBSCRIPT_CLOSE,
      LIST_INVOKE, //curly brackets
         LIST_OPEN,
         LIST_CLOSE,
      SPECIALIZER_INVOKE, //triangle brackets
         SPECIALIZER_OPEN,
         SPECIALIZER_CLOSE,


      PREPROCESSOR,

      SCOPE_RESOLUTION,

      INC, //increment
      DEC, //decrement

      MEMBER_ACCESS, // .
      MEMBER_OF_POINTER_ACCESS, // ->
      POINTER_TO_MEMBER, // .*
      POINTER_TO_MEMBER_OF_POINTER, // ->*

      RANGE,
      SPREAD,
      
      ADD, //addition
      SUB, //subtraction
      MUL, //multiplication
      DIV, //division
      MOD, //modulo
      EXP, //exponentation
      
      LOGICAL_NOT,
      LOGICAL_AND,
      LOGICAL_OR,

      BIT_NOT,
      BIT_AND,
      BIT_OR,
      BIT_XOR,
      SHIFT_LEFT,
      SHIFT_RIGHT,
      
      
      UNIQUE_PTR,
      SHARED_PTR,
      WEAK_PTR,
      ITERATOR,
      

      THREE_WAY_COMP, //AKA spaceship
      LESSER, //less than
      GREATER, //greater than
      LESSER_OR_EQ, //less than or equal to
      GREATER_OR_EQ, //greater than or equal to

      IS_EQUAL, //equality comparison
      IS_UNEQUAL, //inequality comparison
      // IS_EQUAL_STRICT, //!NOTE: NOT USED BY MIDDLE C
      // IS_UNEQUAL_STRICT, //!NOTE: NOT USED BY MIDDLE C

      COALESCE, //null coalescing

      INLINE_IF, //ternary operator opener
      INLINE_ELSE, //ternary operator closer

      ASSIGN,
      // CONST_ASSIGN, //!NOTE: NOT USED BY MIDDLE C
      ADD_ASSIGN, //compound assignment (addition)
      SUB_ASSIGN, //compound assignment (substraction)
      MUL_ASSIGN, //compound assignment (multiplication)
      DIV_ASSIGN, //compound assignment (division)
      MOD_ASSIGN, //compound assignment (modulo)
      EXP_ASSIGN, //compound assignment (exponentiation)
      SHL_ASSIGN, //compound assignment (left-shift)
      SHR_ASSIGN, //compound assignment (right-shift)
      AND_ASSIGN, //compound assignment (bitwise and)
      XOR_ASSIGN, //compound assignment (bitwise exclusive or)
      OR_ASSIGN, //compound assignment (bitwise or)
      COALESCE_ASSIGN, //compound assignment (null-coalescing)

      COMMA,



      //keyword pseudo-operators
      FOR,
      FOREACH,
      WHILE,
      DO_WHILE,

      IF,

      SWITCH,
      MATCH,

      TRY_CATCH,
      ASM,

      //other pseudo-operators
      DECL,

      //aliases
      LABEL_DELIM = INLINE_IF,
      EACH_OF = LABEL_DELIM,
      EXTENDS = LABEL_DELIM,
      ADDRESS_OF = BIT_AND,
      REFERENCE = ADDRESS_OF,
      RAW_PTR = MUL,
      DEREF = RAW_PTR,
      VARIADIC_PARAM = SPREAD,
      ARROW = MEMBER_OF_POINTER_ACCESS,
      UNARY_PLUS = ADD,
      UNARY_MINUS = SUB,
      RADIX_POINT = MEMBER_ACCESS,

      //helpers
      __FIRST_PSEUDO_OP = FOR,
      __FIRST_LOOP = FOR,
      __LAST_LOOP = DO_WHILE,
   };

   constexpr auto operator+(const OpID x) { return std::to_underlying(x); }
   constexpr OpID operator&(const OpID lhs, const OpID rhs) { return (OpID)(+lhs & +rhs); }
   constexpr OpID& operator&=(OpID& lhs, const OpID rhs) { lhs = lhs & rhs; return lhs; }
   constexpr bool isOperator(const OpID x) { return x < OpID::__FIRST_PSEUDO_OP; }

   constexpr bool isDecl(const OpID op) { return op == OpID::DECL; }

   constexpr bool isForLoop(const OpID op) { return op == OpID::FOR; }
   constexpr bool isForeachLoop(const OpID op) { return op == OpID::FOREACH; }
   constexpr bool isWhileLoop(const OpID op) { return op == OpID::WHILE; }
   constexpr bool isDoWhileLoop(const OpID op) { return op == OpID::DO_WHILE; }

   constexpr bool isLoop(const OpID op) { return op >= OpID::__FIRST_LOOP && op <= OpID::__LAST_LOOP; }
   constexpr bool isSimpleLoop(const OpID op) { return isWhileLoop(op) || isDoWhileLoop(op); }

   constexpr bool isIf(const OpID op) { return op == OpID::IF; }
   constexpr bool isSwitch(const OpID op) { return op == OpID::SWITCH; }
   constexpr bool isMatch(const OpID op) { return op == OpID::MATCH; }

   constexpr bool isStringLike(const OpID op) { return op == OpID::STRING || op == OpID::CHAR; }
   constexpr bool isOpener(const OpID op) { return op == OpID::CALL_OPEN || op == OpID::SUBSCRIPT_OPEN || op == OpID::LIST_OPEN || op == OpID::SPECIALIZER_OPEN; }
   constexpr OpID getCloser(const OpID opener) {
      debug_assert(isOpener(opener));
      return (OpID)(+opener + 1);
   }
   constexpr OpID getInvoker(const OpID opener) {
      debug_assert(isOpener(opener));
      return (OpID)(+opener - 1);
   }

   //operator properties bitmask
   enum class [[clang::flag_enum]] OpProps : uint8 {
      NULL = 0,

      CAN_BE_POSTFIX = 8_m,
      CAN_BE_PREFIX  = 7_m,
      CAN_BE_BINARY  = 6_m,
      IS_LEFT_ASSOC  = 5_m, //only applies for use as binary operators

      __PRECEDENCE_BITS = 4_m | 3_m | 2_m | 1_m,

      POSTFIX = CAN_BE_POSTFIX,
      PREFIX = CAN_BE_PREFIX,
      INFIX_LEFT = CAN_BE_BINARY | IS_LEFT_ASSOC,
      INFIX_RIGHT = CAN_BE_BINARY,

      OPEN_DELIM = PREFIX,
      CLOSE_DELIM = POSTFIX,
      DELIM = OPEN_DELIM | CLOSE_DELIM,

      TYPE_MOD = POSTFIX,
   };
   constexpr auto    operator+(const OpProps t) noexcept { return std::to_underlying(t); }
   constexpr OpProps operator~(const OpProps lhs) noexcept { return (OpProps)(~+lhs); }
   constexpr OpProps operator&(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) & (+rhs)); }
   constexpr OpProps operator^(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) ^ (+rhs)); }
   constexpr OpProps operator|(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) | (+rhs)); }
   constexpr uint8 precedence(const OpProps x) { return +(x & OpProps::__PRECEDENCE_BITS); }
   constexpr bool isBinary(const OpProps x) { return +(x & OpProps::CAN_BE_BINARY); }
   constexpr OpProps makeOpProps(const bool canBePostfix, const bool canBePrefix, const bool canBeBinary, const bool isLeftAssocWhenBinary, const uint8 precedence) {
      assert(precedence <= +OpProps::__PRECEDENCE_BITS, "precedence value overflows into property flag bits"); //maybe make debug_assert
      OpProps post = canBePostfix ? OpProps::CAN_BE_POSTFIX : OpProps::NULL;
      OpProps pre  = canBePrefix  ? OpProps::CAN_BE_PREFIX  : OpProps::NULL;
      OpProps bin  = canBeBinary  ? OpProps::CAN_BE_BINARY  : OpProps::NULL;
      OpProps left = (isLeftAssocWhenBinary && canBeBinary) ? OpProps::IS_LEFT_ASSOC : OpProps::NULL;
      return post | pre | bin | left | (OpProps)precedence;
   }
   constexpr OpProps makeOpProps(const uint8 flags, const uint8 precedence) {
      assert(flags      <= +OpProps::__PRECEDENCE_BITS, "property flag bits underflow into precedence bits"); //maybe make debug_assert
      assert(precedence <= +OpProps::__PRECEDENCE_BITS, "precedence value overflows into property flag bits"); //maybe make debug_assert

      return (OpProps)((flags << 4) | precedence);
   }
   constexpr OpProps makeOpProps(const OpProps flags, const uint8 precedence) {
      assert(+flags     >  +OpProps::__PRECEDENCE_BITS, "property flag bits underflow into precedence bits"); //maybe make debug_assert
      assert(precedence <= +OpProps::__PRECEDENCE_BITS, "precedence value overflows into property flag bits"); //maybe make debug_assert

      return (OpProps)(+flags | precedence);
   }
   #pragma endregion ops
   #pragma region keyword
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
      UPTR,
      UWORD,

      SIGN_T         = __TYPE | __NUMERIC | __SIGNED,
      SBYTE,
      SSHORT,
      SINT,
      SLONG,
      SPTR,
      SWORD,


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
      DEBUG_ASSERT,

      TRUE,
      FALSE,

      NULLPTR,

      RETURN,

      USING,

      ASM,

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
   #pragma endregion keyword

   //delimiter pair specification
   enum class BlockType : uint8 {
      NONE = 0,      //not a block type

      CALL = 3_m,       //PARENTHESES
      SUBSCRIPT,        //SQUARE BRACKETS
      INIT_LIST,        //CURLY BRACKETS
      SPECIALIZER,      //TRIANGLE BRACKETS

      QUOTES_CHAR,      //CHARACTER
      QUOTES_STR,       //STRING
      QUOTES_INTERP,    //INTERPOLATED STRING

      TERNARY,          //ternary statement

      COMMENT_BLOCK,    //COMMENTED-OUT CODE
      COMMENT_LINE,     //COMMENTED OUT CODE
   };
   constexpr auto operator+(const BlockType t) noexcept { return std::to_underlying(t); }
   //block delimiter token role
   //!NOTE: could merge into BlockType, but that would not provide any performance advantages because significantly more bytes are going to be used for other Token types anyway
   enum class BlockDelimRole : uint8 {
      NULL = 0,
      OPEN = 1_m,
      CLOSE = 2_m,
      BOTH = OPEN | CLOSE,
   };
   constexpr auto operator+(const BlockDelimRole t) noexcept { return std::to_underlying(t); } 
   constexpr bool isOpener(const BlockDelimRole t) { return +t & +BlockDelimRole::OPEN; }
   constexpr bool isCloser(const BlockDelimRole t) { return +t & +BlockDelimRole::CLOSE; }

   //plaintext segment type
   enum class PtxtType : uint8 {
      NULL = 0,

      CHAR = 1,
      WCHAR,   //!NOTE: CURRENETLY UNSUPPORTED
      CHAR8,   //!NOTE: CURRENETLY UNSUPPORTED
      CHAR16,  //!NOTE: CURRENETLY UNSUPPORTED
      CHAR32,  //!NOTE: CURRENETLY UNSUPPORTED

      STR = 9,
      WSTR,    //!NOTE: CURRENETLY UNSUPPORTED
      STR8,    //!NOTE: CURRENETLY UNSUPPORTED
      STR16,   //!NOTE: CURRENETLY UNSUPPORTED
      STR32,   //!NOTE: CURRENETLY UNSUPPORTED

      UNPROCESSED_STR = 25,
      UNPROCESSED_WSTR,    //!NOTE: CURRENETLY UNSUPPORTED
      UNPROCESSED_STR8,    //!NOTE: CURRENETLY UNSUPPORTED
      UNPROCESSED_STR16,   //!NOTE: CURRENETLY UNSUPPORTED
      UNPROCESSED_STR32,   //!NOTE: CURRENETLY UNSUPPORTED

      __STR_TYPE = 8,
   };
   constexpr auto operator+(const PtxtType t) noexcept { return std::to_underlying(t); }
   constexpr bool isChar(const PtxtType t) { return +t && !(+t & +PtxtType::__STR_TYPE); }
   constexpr bool isString(const PtxtType t) { return +t & +PtxtType::__STR_TYPE; }
   
   //!literal type specification
   enum class LitType : uint8 {
      NONE = 0,

      UINT,
      SINT,
      FLOAT,

      CHAR,
      STRING,
      INTERP_STR,
      FORMAT,
      REGEX,

      TYPEID,
   };
   constexpr auto operator+(const LitType t) noexcept { return std::to_underlying(t); }

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
      NULL           = 0_m,
      
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


   enum class FundTypeID : uint8 {
      NULL,
      FUNCTION_SIGNATURE,
      //!NOTE: UNFINISHED
   };
}

#endif //ENUMS_HPP