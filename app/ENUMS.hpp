#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

#include "CLEF_DEFINES.hpp"
#include <utility>

#include "assert.hpp"

#ifdef __GNUC__
   #define ctz __builtin_ctz
   #define popcnt __builtin_popcount
#else
   #include <bit>
   #define ctz std::countr_zero
   #define popcnt std::popcount
#endif

namespace clef {
   //!enum of CLEF error codes
   enum class ErrCode {
      UNSPEC = 0,

      //lexer
      BAD_CMNT,
      BAD_LIT,
      UNREC_SRC_CHAR,
      
      //parser
      MISSING_KW,
      BAD_KW,
      MISSING_OP,
      BAD_OP,
      MISSING_BLOCK_DELIM,
      BAD_BLOCK_DELIM,
      MISSING_EOS,
      MISSING_LABEL,
      BAD_EXPR,
      MISSING_TYPE,
      BAD_TYPE_DEF,
      CONFLICTING_REDECL,
      REDEF,
      TYPE_CONFLICT,
      BAD_GENERIC,
      SYMBOL_NOT_FOUND,

      DUPLICATE_OVERLOAD,
   };
   constexpr auto      operator+(const ErrCode t) noexcept { return std::to_underlying(t); }
   
   //!token types bitmask
   enum class TokenType : uint8 {
      NONE = 0,
      
      IDEN,
      MACRO_INVOKE,
      KEYWORD,
      UINT_NUM,
      SINT_NUM,
      REAL_NUM,
      BOOL_LIT,
      CHAR_LIT,
      STR_LIT,

      __OPLIKE = 0x80,
      OP = 0x81,
      EOS,
      BLOCK_DELIM,
      ATTR,
      PREPROC_INIT,
   };
   constexpr auto      operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }

   constexpr bool isOperator(const TokenType t) { return t == TokenType::OP; }
   constexpr bool isBlockLike(const TokenType t) { return t == TokenType::BLOCK_DELIM; }
   constexpr bool isOperand(const TokenType t) { return !+(t & TokenType::__OPLIKE); }
      constexpr bool isNumber(const TokenType t) { return t == TokenType::UINT_NUM || t == TokenType::SINT_NUM || t == TokenType::REAL_NUM; }
   #pragma region ops
   enum class OpID : uint8 {
      null = 0, //nop or not an operator

      INC, //increment
      DEC, //decrement
      
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
      SHL,
      SHR,
      

      THREE_WAY_COMP, //AKA spaceship
      LESSER, //less than
      GREATER, //greater than
      LESSER_OR_EQ, //less than or equal to
      GREATER_OR_EQ, //greater than or equal to

      IS_EQUAL, //equality comparison
      IS_UNEQUAL, //inequality comparison

      COALESCE, //null coalescing

      ASSIGN,
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

      SLICE,
      
      CALL, //parens
      INDEX, //square brackets
      INIT_LIST, //curly brackets
      SPECIALIZER, //triangle brackets

      INLINE_IF,
      INLINE_ELSE,

      SCOPE_RESOLUTION,

      MEMBER_ACCESS, // .
      PTR_MEMBER_ACCESS, // ->
      METHOD_PTR, // .*
      ARROW_METHOD_PTR, // ->*

      RANGE,
      SPREAD,

      COMMA,


      //keyword pseudo-operators
      GOTO,
      GOTO_CASE,

      BREAK,
      CONTINUE,
      
      ASSERT,
      STATIC_ASSERT,
      ASSUME,

      RETURN,

      CAST,
      UP_CAST,
      BIT_CAST,
      CONST_CAST,

      LET,
      MAKE_TYPE,
      

      TYPEOF,
      SIZEOF,
      ALIGNOF,
      ALIGNAS,
      STRIDEOF,
      STRIDEAS,

      //aliases
      LABEL_DELIM = INLINE_IF,
      EACH_OF = LABEL_DELIM,
      EXTENDS = LABEL_DELIM,
      ADDRESS_OF = BIT_AND,
      REFERENCE = ADDRESS_OF,
      RAW_PTR = MUL,
      DEREF = RAW_PTR,
      VARIADIC_PARAM = SPREAD,
      ARROW = PTR_MEMBER_ACCESS,
      UNARY_PLUS = ADD,
      UNARY_MINUS = SUB,
      RADIX_POINT = MEMBER_ACCESS,

      //helpers
      __FIRST_PSEUDO_OP = GOTO,
      __LAST_PSEUDO_OP = STRIDEAS,
      __FIRST_CAST = CAST,
      __LAST_CAST = CONST_CAST,
   };

   constexpr auto operator+(const OpID x) { return std::to_underlying(x); }
   constexpr OpID operator&(const OpID lhs, const OpID rhs) { return (OpID)(+lhs & +rhs); }
   constexpr OpID& operator&=(OpID& lhs, const OpID rhs) { lhs = lhs & rhs; return lhs; }
   constexpr bool isOperator(const OpID x) { return x < OpID::__FIRST_PSEUDO_OP; }

   constexpr bool isDecl(const OpID op) { return op == OpID::LET; }

   constexpr bool isCast(const OpID op) { return op >= OpID::__FIRST_CAST && op <= OpID::__LAST_CAST; }

   //operator properties bitmask
   enum class [[clang::flag_enum]] OpProps : uint8 {
      null = 0,

      TYPE_MOD       = 1_m,
      OPEN_DELIM     = 2_m,
      CLOSE_DELIM    = 3_m,

      CAN_BE_POSTFIX = 4_m,
      CAN_BE_PREFIX  = 5_m,
      CAN_BE_BINARY  = 6_m,
      
      IS_LEFT_ASSOC  = 7_m, //only applies for use as binary operators

      POSTFIX = CAN_BE_POSTFIX,
      PREFIX = CAN_BE_PREFIX,
      INFIX_LEFT = CAN_BE_BINARY | IS_LEFT_ASSOC,
      INFIX_RIGHT = CAN_BE_BINARY,

      DELIM = OPEN_DELIM | CLOSE_DELIM,
   };
   constexpr auto    operator+(const OpProps t) noexcept { return std::to_underlying(t); }
   constexpr OpProps operator~(const OpProps lhs) noexcept { return (OpProps)(~+lhs); }
   constexpr OpProps operator&(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) & (+rhs)); }
   constexpr OpProps operator^(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) ^ (+rhs)); }
   constexpr OpProps operator|(const OpProps lhs, const OpProps rhs) noexcept { return (OpProps)((+lhs) | (+rhs)); }
   constexpr bool isBinary(const OpProps x) { return +(x & OpProps::CAN_BE_BINARY); }
   constexpr bool isUnary(const OpProps x) { return +(x & (OpProps::CAN_BE_PREFIX | OpProps::CAN_BE_POSTFIX)); }
   constexpr OpProps makeOpProps(const bool canBePostfix, const bool canBePrefix, const bool canBeBinary, const bool canBeTypeMod, const bool canBeOpen, const bool canBeClose, const bool isLeftAssoc) {
      OpProps post  = canBePostfix ? OpProps::CAN_BE_POSTFIX : OpProps::null;
      OpProps pre   = canBePrefix  ? OpProps::CAN_BE_PREFIX  : OpProps::null;
      OpProps bin   = canBeBinary  ? OpProps::CAN_BE_BINARY  : OpProps::null;
      OpProps mod   = canBeTypeMod ? OpProps::TYPE_MOD       : OpProps::null;
      OpProps open  = canBeOpen    ? OpProps::OPEN_DELIM     : OpProps::null;
      OpProps close = canBeClose   ? OpProps::CLOSE_DELIM    : OpProps::null;
      OpProps left  = (isLeftAssoc && canBeBinary) ? OpProps::IS_LEFT_ASSOC : OpProps::null;
      return post | pre | bin | mod | open | close | left;
   }

   enum class Oplike : uint8 {
      null = 0, //nop or not an operator

      INC, //increment
      DEC, //decrement
      
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
      SHL,
      SHR,
      

      THREE_WAY_COMP, //AKA spaceship
      LESSER, //less than
      GREATER, //greater than
      LESSER_OR_EQ, //less than or equal to
      GREATER_OR_EQ, //greater than or equal to

      IS_EQUAL, //equality comparison
      IS_UNEQUAL, //inequality comparison

      COALESCE, //null coalescing

      ASSIGN,
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
      
      //parens
      CALL_OPEN,
      CALL_CLOSE,
      //square brackets
      INDEX_OPEN,
      INDEX_CLOSE,
      //curly brackets
      LIST_OPEN,
      LIST_CLOSE,
      //triangle brackets
      SPECIALIZER_OPEN,
      SPECIALIZER_CLOSE,

      SLICE,

      INLINE_IF,
      INLINE_ELSE,

      SCOPE_RESOLUTION,

      MEMBER_ACCESS, // .
      PTR_MEMBER_ACCESS, // ->
      METHOD_PTR, // .*
      ARROW_METHOD_PTR, // ->*

      RANGE,
      SPREAD,

      COMMA,

      LINE_CMNT,
      BLOCK_CMNT_OPEN,
      BLOCK_CMNT_CLOSE,

      ESC,

      PREPROC,
      EOS,
      CHAR,
      STRING,
      ATTR,

      //keyword pseudo-operators
      ASSERT,
      STATIC_ASSERT,
      ASSUME,

      RETURN,

      CAST,
      UP_CAST,
      BIT_CAST,
      CONST_CAST,

      TYPEOF,
      SIZEOF,
      ALIGNOF,
      ALIGNAS,
      STRIDEOF,
      STRIDEAS,

      //aliases
      LABEL_DELIM = INLINE_IF,
      EACH_OF = LABEL_DELIM,
      EXTENDS = LABEL_DELIM,
      ADDRESS_OF = BIT_AND,
      REFERENCE = ADDRESS_OF,
      RAW_PTR = MUL,
      DEREF = RAW_PTR,
      VARIADIC_PARAM = SPREAD,
      ARROW = PTR_MEMBER_ACCESS,
      UNARY_PLUS = ADD,
      UNARY_MINUS = SUB,
      RADIX_POINT = MEMBER_ACCESS,
   };
   constexpr auto operator+(const Oplike op) { return std::to_underlying(op); }

   constexpr OpID toOpID(const Oplike op) {
      using L = Oplike;
      using O = OpID;
      static constexpr const OpID buf[] = {
         [             +L::null] = O::null,

         [              +L::INC] = O::INC,
         [              +L::DEC] = O::DEC,

         [              +L::ADD] = O::ADD,
         [              +L::SUB] = O::SUB,
         [              +L::MUL] = O::MUL,
         [              +L::DIV] = O::DIV,
         [              +L::MOD] = O::MOD,
         [              +L::EXP] = O::EXP,

         [      +L::LOGICAL_NOT] = O::LOGICAL_NOT,
         [      +L::LOGICAL_AND] = O::LOGICAL_AND,
         [       +L::LOGICAL_OR] = O::LOGICAL_OR,

         [          +L::BIT_NOT] = O::BIT_NOT,
         [          +L::BIT_AND] = O::BIT_AND,
         [           +L::BIT_OR] = O::BIT_OR,
         [          +L::BIT_XOR] = O::BIT_XOR,
         [              +L::SHL] = O::SHL,
         [              +L::SHR] = O::SHR,


         [   +L::THREE_WAY_COMP] = O::THREE_WAY_COMP,
         [           +L::LESSER] = O::LESSER,
         [          +L::GREATER] = O::GREATER,
         [     +L::LESSER_OR_EQ] = O::LESSER_OR_EQ,
         [    +L::GREATER_OR_EQ] = O::GREATER_OR_EQ,

         [         +L::IS_EQUAL] = O::IS_EQUAL,
         [       +L::IS_UNEQUAL] = O::IS_UNEQUAL,

         [         +L::COALESCE] = O::COALESCE,

         [           +L::ASSIGN] = O::ASSIGN,
         [       +L::ADD_ASSIGN] = O::ADD_ASSIGN,
         [       +L::SUB_ASSIGN] = O::SUB_ASSIGN,
         [       +L::MUL_ASSIGN] = O::MUL_ASSIGN,
         [       +L::DIV_ASSIGN] = O::DIV_ASSIGN,
         [       +L::MOD_ASSIGN] = O::MOD_ASSIGN,
         [       +L::EXP_ASSIGN] = O::EXP_ASSIGN,
         [       +L::SHL_ASSIGN] = O::SHL_ASSIGN,
         [       +L::SHR_ASSIGN] = O::SHR_ASSIGN,
         [       +L::AND_ASSIGN] = O::AND_ASSIGN,
         [       +L::XOR_ASSIGN] = O::XOR_ASSIGN,
         [        +L::OR_ASSIGN] = O::OR_ASSIGN,
         [  +L::COALESCE_ASSIGN] = O::COALESCE_ASSIGN,

         [            +L::SLICE] = O::SLICE,

         [        +L::INLINE_IF] = O::INLINE_IF,
         [      +L::INLINE_ELSE] = O::INLINE_ELSE,
         [ +L::SCOPE_RESOLUTION] = O::SCOPE_RESOLUTION,
         [    +L::MEMBER_ACCESS] = O::MEMBER_ACCESS,
         [+L::PTR_MEMBER_ACCESS] = O::PTR_MEMBER_ACCESS,
         [       +L::METHOD_PTR] = O::METHOD_PTR,
         [ +L::ARROW_METHOD_PTR] = O::ARROW_METHOD_PTR,
         [            +L::RANGE] = O::RANGE,
         [           +L::SPREAD] = O::SPREAD,
         [            +L::COMMA] = O::COMMA,

         [        +L::LINE_CMNT] = O::null,
         [  +L::BLOCK_CMNT_OPEN] = O::null,
         [ +L::BLOCK_CMNT_CLOSE] = O::null,
         [              +L::ESC] = O::null,
         [        +L::CALL_OPEN] = O::null,
         [       +L::CALL_CLOSE] = O::null,
         [       +L::INDEX_OPEN] = O::null,
         [      +L::INDEX_CLOSE] = O::null,
         [        +L::LIST_OPEN] = O::null,
         [       +L::LIST_CLOSE] = O::null,
         [ +L::SPECIALIZER_OPEN] = O::null,
         [+L::SPECIALIZER_CLOSE] = O::null,
         [          +L::PREPROC] = O::null,
         [              +L::EOS] = O::null,
         [             +L::CHAR] = O::null,
         [           +L::STRING] = O::null,
         [             +L::ATTR] = O::null,

         [           +L::ASSERT] = O::ASSERT,
         [    +L::STATIC_ASSERT] = O::STATIC_ASSERT,
         [           +L::ASSUME] = O::ASSUME,

         [           +L::RETURN] = O::RETURN,

         [             +L::CAST] = O::CAST,
         [          +L::UP_CAST] = O::UP_CAST,
         [         +L::BIT_CAST] = O::BIT_CAST,
         [       +L::CONST_CAST] = O::CONST_CAST,

         [           +L::TYPEOF] = O::TYPEOF,
         [           +L::SIZEOF] = O::SIZEOF,
         [          +L::ALIGNOF] = O::ALIGNOF,
         [          +L::ALIGNAS] = O::ALIGNAS,
         [         +L::STRIDEOF] = O::STRIDEOF,
         [         +L::STRIDEAS] = O::STRIDEAS,
      };
      return buf[+op];
   }

   #pragma endregion ops
   #pragma region keyword
   enum class KeywordID : uint8 {
      _NOT_A_KEYWORD = 0,


      VOID,
      AUTO,


      CHAR,
      CHAR_8,
      CHAR_16,
      CHAR_32,
      

      BOOL,
      UBYTE,
      USHORT,
      UINT,
      ULONG,
      UOVERLONG,
      UPTR,
      UWORD,

      UINT_8,
      UINT_16,
      UINT_32,
      UINT_64,
      UINT_128,
      UINT_256,


      SIGN_T, //to_signed<bool>
      SBYTE,
      SSHORT,
      SINT,
      SLONG,
      SOVERLONG,
      SPTR,
      SWORD,

      SINT_8,
      SINT_16,
      SINT_32,
      SINT_64,
      SINT_128,
      SINT_256,

      FP3, //to_float<bool>
      FBYTE,
      FSHORT,
      FLOAT,
      FLONG,
      FLEXT,
      FPTR,
      FWORD,

      FLOAT_8,
      FLOAT_16,
      FLOAT_32,
      FLOAT_64,
      FLOAT_80,
      FLOAT_128,
      FLOAT_256,


      __SIGN_TYPE_OFFSET = SBYTE - UBYTE,

      __FIRST_TEXT_TYPE = CHAR,
      __LAST_TEXT_TYPE = CHAR_32,
      __FIRST_INT_TYPE = BOOL,
      __LAST_INT_TYPE = SINT_256,
      __FIRST_UINT_TYPE = BOOL,
      __LAST_UINT_TYPE = UINT_256,
      __FIRST_SINT_TYPE = SIGN_T,
      __LAST_SINT_TYPE = SINT_256,
      __FIRST_FLOAT_TYPE = FP3,
      __LAST_FLOAT_TYPE = FLOAT_256,
      __FIRST_NUM_TYPE = __FIRST_INT_TYPE,
      __LAST_NUM_TYPE = __LAST_FLOAT_TYPE,
      __FIRST_TYPE = VOID,
      __LAST_TYPE = FLOAT_256,





      TYPE, //typename

      CLASS,
      STRUCT,
      TRAIT,
      UNION,
      ENUM,
      ENUM_UNION,
      MASK,
      NAMESPACE,
      TUPLE,
      FUNC,
      MACRO,

      __FIRST_OBJ_TYPE = TYPE,
      __LAST_OBJ_TYPE = MACRO,

      CONST,
      CONSTEXPR,
      IMMEDIATE,
      VIEW,
      NON_OWNING,
      MUTABLE,
      VOLATILE,
      ATOMIC,
      EXTERN,
      INLINE,
      NOEXCEPT,
      VIRTUAL,

      PUBLIC = VIRTUAL + 2,
      PRIVATE,
      PROTECTED,

      __FIRST_QUAL = CONST,
      __LAST_QUAL = VIRTUAL,
      __FIRST_SCOPE_MOD = PUBLIC,
      __LAST_SCOPE_MOD = PROTECTED,

      __FIRST_QUAL_LIKE = __FIRST_QUAL,
      __LAST_QUAL_LIKE = __LAST_SCOPE_MOD,

      STATIC,

      CAST,
      UP_CAST,
      BIT_CAST,
      CONST_CAST,

      __FIRST_CAST = CAST,
      __LAST_CAST = CONST_CAST,

      TYPEOF,
      SIZEOF,
      ALIGNOF,
      ALIGNAS,
      STRIDEOF,
      STRIDEAS,

      __FIRST_TYPE_ASSESSMENT = TYPEOF,
      __LAST_TYPE_ASSESSMENT = STRIDEAS,
      

      GOTO,
      RETURN,
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

      __FIRST_CONTROL_FLOW = GOTO,
      __LAST_CONTROL_FLOW = DEFAULT,




      THIS,
      SELF,

      TRUE,
      FALSE,

      NULLPTR,

      __FIRST_VALUE = THIS,
      __LAST_VALUE = NULLPTR,

      ASSERT,
      STATIC_ASSERT,
      ASSUME,

      __FIRST_ASSERT = ASSERT,
      __LAST_ASSERT = ASSUME,

      LET,
      USING,

      ASM,

      __FIRST_UNSPEC = LET,
      __LAST_UNSPEC = ASM,
   };
   constexpr auto operator+(const KeywordID t) noexcept { return std::to_underlying(t); }
   #define __BETWEEN(name) (id >= KeywordID::__FIRST_##name && id <= KeywordID::__LAST_##name)
   constexpr bool isType(const KeywordID id) noexcept { return __BETWEEN(TYPE); }
      constexpr bool isNumeric(const KeywordID id) noexcept { return __BETWEEN(NUM_TYPE); }
         constexpr bool isFloatingPoint(const KeywordID id) noexcept { return __BETWEEN(FLOAT_TYPE); }
         constexpr bool isInteger(const KeywordID id) noexcept { return __BETWEEN(INT_TYPE); }
            constexpr bool isSint(const KeywordID id) noexcept { return __BETWEEN(SINT_TYPE); }
            constexpr bool isUint(const KeywordID id) noexcept { return __BETWEEN(UINT_TYPE); }
      constexpr bool isText(const KeywordID id) noexcept { return __BETWEEN(TEXT_TYPE); }
         constexpr bool isASCII(const KeywordID id) noexcept { return id == KeywordID::CHAR; }
         constexpr bool isUnicode(const KeywordID id) noexcept { return !isASCII(id) && isText(id); }
   constexpr bool isObjectType(const KeywordID id) noexcept { return __BETWEEN(OBJ_TYPE); }
   constexpr bool isQualLike(const KeywordID id) noexcept { return __BETWEEN(QUAL_LIKE); }
      constexpr bool isQualifier(const KeywordID id) noexcept { return __BETWEEN(QUAL); }
      constexpr bool isScopeMod(const KeywordID id) noexcept { return __BETWEEN(SCOPE_MOD); }
   constexpr bool isCast(const KeywordID id) noexcept { return __BETWEEN(CAST); }
   constexpr bool isControlFlow(const KeywordID id) noexcept { return __BETWEEN(CONTROL_FLOW); }
   constexpr bool isUnspec(const KeywordID id) noexcept { return __BETWEEN(UNSPEC); }
   constexpr bool isValue(const KeywordID id) noexcept { return __BETWEEN(VALUE); }
   #undef __BETWEEN

   constexpr bool isPrefixOpLike(const KeywordID id) noexcept {
      using enum KeywordID;
      switch (id) {
         case ASSERT: fthru;
         case STATIC_ASSERT: fthru;
         case ASSUME: fthru;

         case RETURN:
            return true;
         
         default:
            return false;
      }
   }
   constexpr bool isUnaryFuncLike(const KeywordID id) noexcept {//no specializer parameters
      using enum KeywordID;
      switch (id) {
         case TYPEOF:
         case SIZEOF:
         case ALIGNAS:
         case ALIGNOF:
         case STRIDEAS:
         case STRIDEOF:
            return true;
         
         default:
            return false;
      }
   }
   constexpr OpID toOpID(const KeywordID id) {
      using K = KeywordID;
      using O = OpID;
      static constexpr const O buf[256] = {
         [        +K::BREAK] = O::BREAK,
         [     +K::CONTINUE] = O::CONTINUE,
         
         [       +K::ASSERT] = O::ASSERT,
         [       +K::ASSUME] = O::ASSUME,
         [+K::STATIC_ASSERT] = O::STATIC_ASSERT,
         [       +K::RETURN] = O::RETURN,
         
         [       +K::TYPEOF] = O::TYPEOF,
         [       +K::SIZEOF] = O::SIZEOF,
         [      +K::ALIGNAS] = O::ALIGNAS,
         [      +K::ALIGNOF] = O::ALIGNOF,
         [     +K::STRIDEAS] = O::STRIDEAS,
         [     +K::STRIDEOF] = O::STRIDEOF,

         [         +K::CAST] = O::CAST,
         [      +K::UP_CAST] = O::UP_CAST,
         [     +K::BIT_CAST] = O::BIT_CAST,
         [   +K::CONST_CAST] = O::CONST_CAST,
      }; static_assert(sizeof(id) == 1);
      O val = buf[+id];
      if (val == O::null) {
         UNREACHABLE;
      }
      return val;
   }
   constexpr Oplike toOplike(const KeywordID kw) {
      using K = KeywordID;
      using O = Oplike;
      static constexpr const O buf[256] = {
         [       +K::ASSERT] = O::ASSERT,
         [+K::STATIC_ASSERT] = O::STATIC_ASSERT,
         [       +K::ASSUME] = O::ASSUME,

         [       +K::RETURN] = O::RETURN,

         [         +K::CAST] = O::CAST,
         [      +K::UP_CAST] = O::UP_CAST,
         [     +K::BIT_CAST] = O::BIT_CAST,
         [   +K::CONST_CAST] = O::CONST_CAST,

         [       +K::TYPEOF] = O::TYPEOF,
         [       +K::SIZEOF] = O::SIZEOF,
         [      +K::ALIGNOF] = O::ALIGNOF,
         [      +K::ALIGNAS] = O::ALIGNAS,
         [     +K::STRIDEOF] = O::STRIDEOF,
         [     +K::STRIDEAS] = O::STRIDEAS,
      }; static_assert(sizeof(kw) == 1);
      O val = buf[+kw];
      if (val == O::null) {
         UNREACHABLE;
      }
      return val;
   }

   constexpr KeywordID makeSized_c(const KeywordID id, const char ch) {
      #define HASH(val) (val & 15)
      using enum KeywordID;
      static constexpr const sbyte offsets[] = {
         [HASH(    WORD_LIT_CHAR)] =  4,
         [HASH(     PTR_LIT_CHAR)] =  3,
         [HASH(OVERLONG_LIT_CHAR)] =  2,
         [HASH(    LONG_LIT_CHAR)] =  1,
       //[HASH(                0)] =  0,
         [HASH(   SHORT_LIT_CHAR)] = -1,
         [HASH(    BYTE_LIT_CHAR)] = -2,
      };
      
      assume(id == UINT || id == SINT || id == FLOAT || id == CHAR);

      if (id == CHAR) {
         return ch ? _NOT_A_KEYWORD : id;
      }
      return (KeywordID)(+id + offsets[(uint)HASH(ch)]);
      #undef HASH
   }
   constexpr KeywordID makeSized_n(const KeywordID id, const uint size) {
      using enum KeywordID;
      assume(popcnt(size) == 1);
      assume (id == UINT || id == SINT || id == FLOAT || (id == CHAR && size <= 32));
      return (KeywordID)(+id + ctz(size << 2) + (id == FLOAT && size >= 128));
   }
   #pragma region testsize
   #ifdef __INTELLISENSE__
      #pragma diag_suppress 28
   #endif
   #pragma region testsize_c
   #define CHECK(base, baseChar, offset) static_assert(makeSized_c(KeywordID::base, offset##_LIT_CHAR) == KeywordID::baseChar##offset)
   #define FOVERLONG FLEXT
   CHECK( UINT, U, BYTE); CHECK( UINT, U, SHORT); CHECK( UINT, U, LONG); CHECK( UINT, U, OVERLONG); CHECK( UINT, U, PTR); CHECK( UINT, U, WORD);
   CHECK( SINT, S, BYTE); CHECK( SINT, S, SHORT); CHECK( SINT, S, LONG); CHECK( SINT, S, OVERLONG); CHECK( SINT, S, PTR); CHECK( SINT, S, WORD);
   CHECK(FLOAT, F, BYTE); CHECK(FLOAT, F, SHORT); CHECK(FLOAT, F, LONG); CHECK(FLOAT, F, OVERLONG); CHECK(FLOAT, F, PTR); CHECK(FLOAT, F, WORD);
   #undef FOVERLONG
   #undef CHECK
   #pragma endregion testsize_c

   #pragma region testsize_n
   #define CHECK(base, size) static_assert(makeSized_n(KeywordID::base, size) == KeywordID::base##_##size)
   CHECK( UINT, 8); CHECK( UINT, 16); CHECK( UINT, 32); CHECK( UINT, 64); CHECK( UINT, 128); CHECK( UINT, 256);
   CHECK( SINT, 8); CHECK( SINT, 16); CHECK( SINT, 32); CHECK( SINT, 64); CHECK( SINT, 128); CHECK( SINT, 256);
   CHECK(FLOAT, 8); CHECK(FLOAT, 16); CHECK(FLOAT, 32); CHECK(FLOAT, 64); CHECK(FLOAT, 128); CHECK(FLOAT, 256);
   #undef CHECK
   #pragma endregion testsize_n
   #ifdef __INTELLISENSE__
      #pragma diag_default 28
   #endif
   #pragma endregion testsize
   #pragma endregion keyword

   //delimiter pair specification
   enum class BlockType : uint8 {
      NONE = 0,         //not a block type

      CALL = 3_m,       //PARENTHESES
      INDEX,            //SQUARE BRACKETS
      LIST,             //CURLY BRACKETS
      SPECIALIZER,      //TRIANGLE BRACKETS

      TERNARY,          //ternary statement
   };
   constexpr auto operator+(const BlockType t) noexcept { return std::to_underlying(t); }
   constexpr OpID getInvoker(const BlockType t) {
      using enum BlockType;
      if (t < BlockType::CALL || t > BlockType::TERNARY) {
         UNREACHABLE;
      }
      return (OpID)(+t - +BlockType::CALL + +OpID::CALL);
   }
   static_assert(getInvoker(BlockType::CALL) == OpID::CALL);
   static_assert(getInvoker(BlockType::INDEX) == OpID::INDEX);
   static_assert(getInvoker(BlockType::LIST) == OpID::INIT_LIST);
   static_assert(getInvoker(BlockType::SPECIALIZER) == OpID::SPECIALIZER);
   static_assert(getInvoker(BlockType::TERNARY) == OpID::INLINE_IF);
   //block delimiter token role
   //!NOTE: could merge into BlockType, but that would not provide any performance advantages because significantly more bytes are going to be used for other Token types anyway
   enum class BlockDelimRole : uint8 {
      null = 0,
      OPEN = 1_m,
      CLOSE = 2_m,
      BOTH = OPEN | CLOSE,
   };
   constexpr auto operator+(const BlockDelimRole t) noexcept { return std::to_underlying(t); } 
   constexpr BlockDelimRole operator&(const BlockDelimRole lhs, const BlockDelimRole rhs) noexcept { return (BlockDelimRole)(+lhs & +rhs);}
   constexpr bool isOpener(const BlockDelimRole t) { return +(t & BlockDelimRole::OPEN); }
   constexpr bool isCloser(const BlockDelimRole t) { return +(t & BlockDelimRole::CLOSE); }

   enum class FundTypeID : uint8 {
      null,
      NULLPTR_T,
      STR,
      
      VOID,
      AUTO,

      __FLAGS = 0b1111000,

      __CHARS = 0b0001000,
      CHAR,
      CHAR8,
      CHAR16,
      CHAR32,

      __UINTS = __CHARS << 1,
      BOOL,
      UINT8,
      UINT16,
      UINT32,
      UINT64,
      UINT128,
      UINT256,

      __SINTS = __UINTS << 1,
      SIGN_T,
      SINT8,
      SINT16,
      SINT32,
      SINT64,
      SINT128,
      SINT256,

      __FLOATS = __SINTS << 1,
      FLOAT16 = __FLOATS + UINT16 - __UINTS,
      FLOAT32,
      FLOAT64,
      FLOAT80,
      FLOAT128,
      FLOAT256,

      __INTS = __CHARS | __UINTS | __SINTS,
      __NUMS = __INTS | __FLOATS,
   };
   constexpr auto operator+(const FundTypeID id) noexcept { return std::to_underlying(id); }
   constexpr FundTypeID operator~(const FundTypeID id) noexcept { return (FundTypeID)(~+id); }
   constexpr FundTypeID operator&(FundTypeID id, FundTypeID mask) { return (FundTypeID)(+id & +mask); }
   constexpr FundTypeID operator|(FundTypeID id, FundTypeID mask) { return (FundTypeID)(+id | +mask); }

   constexpr bool isChar(FundTypeID id) { return +(id & FundTypeID::__CHARS); }
   constexpr bool isUint(FundTypeID id) { return +(id & FundTypeID::__UINTS); }
   constexpr bool isSint(FundTypeID id) { return +(id & FundTypeID::__SINTS); }
   constexpr bool isInt(FundTypeID id) { return +(id & FundTypeID::__INTS); }
   constexpr bool isFloat(FundTypeID id) { return +(id & FundTypeID::__FLOATS); }
   constexpr bool isNum(FundTypeID id) { return +(id & FundTypeID::__NUMS); }

   constexpr FundTypeID toUint(FundTypeID id) {
      return (id & ~FundTypeID::__FLAGS) | FundTypeID::__UINTS;
   }
   constexpr FundTypeID toSint(FundTypeID id) {
      return (id & ~FundTypeID::__FLAGS) | FundTypeID::__SINTS;
   }
   constexpr FundTypeID toReal(FundTypeID id) {
      return (id & ~FundTypeID::__FLAGS) | FundTypeID::__FLOATS;
   }
   constexpr FundTypeID toInt(FundTypeID id) {
      if (isInt(id)) {
         return id;
      }
      return toSint(id);
   }
   

   //!TODO: remove `INLINE`, `NOEXCEPT`, and `VIRTUAL` (maybe make a `FuncQualMask` enum?); add `STATIC`
   enum class [[clang::flag_enum]] QualMask : uint16 {
      _no_quals = 0,

      CONST       =  1_m,
      CONSTEXPR   =  2_m,
      IMMEDIATE   =  3_m,
      VIEW        =  4_m,
      NON_OWNING  =  5_m,
      MUTABLE     =  6_m,
      VOLATILE    =  7_m,
      ATOMIC      =  8_m,
      EXTERN      =  9_m,
      INLINE      = 10_m,
      NOEXCEPT    = 11_m,
      VIRTUAL     = 12_m,
      VARIADIC    = 13_m,

      PUBLIC      = 14_m,
      PRIVATE     = 15_m,
      PROTECTED   = 16_m,
   };
   constexpr auto operator+(const QualMask t) noexcept { return std::to_underlying(t); }
   constexpr QualMask operator~(const QualMask lhs) noexcept { return (QualMask)(~+lhs); }
   constexpr QualMask operator&(const QualMask lhs, const QualMask rhs) noexcept { return (QualMask)((+lhs) & (+rhs)); }
   constexpr QualMask operator^(const QualMask lhs, const QualMask rhs) noexcept { return (QualMask)((+lhs) ^ (+rhs)); }
   constexpr QualMask operator|(const QualMask lhs, const QualMask rhs) noexcept { return (QualMask)((+lhs) | (+rhs)); }
   constexpr QualMask& operator&=(QualMask& lhs, const QualMask rhs) noexcept { return lhs = (QualMask)((+lhs) & (+rhs)); }
   constexpr QualMask& operator^=(QualMask& lhs, const QualMask rhs) noexcept { return lhs = (QualMask)((+lhs) ^ (+rhs)); }
   constexpr QualMask& operator|=(QualMask& lhs, const QualMask rhs) noexcept { return lhs = (QualMask)((+lhs) | (+rhs)); }
   
   constexpr QualMask toQual(KeywordID kw) {
      //check range
      if (!isQualLike(kw)) {
         return QualMask::_no_quals;
      }
      //apply identity
      return (QualMask)(1 << (+kw - +KeywordID::__FIRST_QUAL_LIKE));

      //compile-time checks to ensure that this works
      static_assert((+KeywordID::__LAST_QUAL_LIKE - +KeywordID::__FIRST_QUAL_LIKE + 1) == (sizeof(QualMask) * 8));
      static_assert((1 << (+KeywordID::CONST - +KeywordID::__FIRST_QUAL_LIKE)) == +QualMask::CONST);
      static_assert((1 << (+KeywordID::PROTECTED - +KeywordID::__FIRST_QUAL_LIKE)) == +QualMask::PROTECTED);
   }
   static_assert(toQual(KeywordID::PRIVATE) == QualMask::PRIVATE);
}

#endif //ENUMS_HPP