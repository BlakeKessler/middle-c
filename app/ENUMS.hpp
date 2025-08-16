#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

#include "CLEF_DEFINES.hpp"
#include <utility>

#include "assert.hpp"

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
      
      CALL, //parens
      INDEX, //square brackets
      INIT_LIST, //curly brackets
      SPECIALIZER, //triangle brackets

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
      switch (op) {
         case Oplike::null: return OpID::null;

         case Oplike::INC: return OpID::INC;
         case Oplike::DEC: return OpID::DEC;

         case Oplike::ADD: return OpID::ADD;
         case Oplike::SUB: return OpID::SUB;
         case Oplike::MUL: return OpID::MUL;
         case Oplike::DIV: return OpID::DIV;
         case Oplike::MOD: return OpID::MOD;
         case Oplike::EXP: return OpID::EXP;

         case Oplike::LOGICAL_NOT: return OpID::LOGICAL_NOT;
         case Oplike::LOGICAL_AND: return OpID::LOGICAL_AND;
         case Oplike::LOGICAL_OR: return OpID::LOGICAL_OR;

         case Oplike::BIT_NOT: return OpID::BIT_NOT;
         case Oplike::BIT_AND: return OpID::BIT_AND;
         case Oplike::BIT_OR: return OpID::BIT_OR;
         case Oplike::BIT_XOR: return OpID::BIT_XOR;
         case Oplike::SHL: return OpID::SHL;
         case Oplike::SHR: return OpID::SHR;


         case Oplike::THREE_WAY_COMP: return OpID::THREE_WAY_COMP;
         case Oplike::LESSER: return OpID::LESSER;
         case Oplike::GREATER: return OpID::GREATER;
         case Oplike::LESSER_OR_EQ: return OpID::LESSER_OR_EQ;
         case Oplike::GREATER_OR_EQ: return OpID::GREATER_OR_EQ;

         case Oplike::IS_EQUAL: return OpID::IS_EQUAL;
         case Oplike::IS_UNEQUAL: return OpID::IS_UNEQUAL;

         case Oplike::COALESCE: return OpID::COALESCE;

         case Oplike::ASSIGN: return OpID::ASSIGN;
         case Oplike::ADD_ASSIGN: return OpID::ADD_ASSIGN;
         case Oplike::SUB_ASSIGN: return OpID::SUB_ASSIGN;
         case Oplike::MUL_ASSIGN: return OpID::MUL_ASSIGN;
         case Oplike::DIV_ASSIGN: return OpID::DIV_ASSIGN;
         case Oplike::MOD_ASSIGN: return OpID::MOD_ASSIGN;
         case Oplike::EXP_ASSIGN: return OpID::EXP_ASSIGN;
         case Oplike::SHL_ASSIGN: return OpID::SHL_ASSIGN;
         case Oplike::SHR_ASSIGN: return OpID::SHR_ASSIGN;
         case Oplike::AND_ASSIGN: return OpID::AND_ASSIGN;
         case Oplike::XOR_ASSIGN: return OpID::XOR_ASSIGN;
         case Oplike::OR_ASSIGN: return OpID::OR_ASSIGN;
         case Oplike::COALESCE_ASSIGN: return OpID::COALESCE_ASSIGN;

         case Oplike::SLICE: return OpID::SLICE;

         case Oplike::INLINE_IF: return OpID::INLINE_IF;
         case Oplike::INLINE_ELSE: return OpID::INLINE_ELSE;
         case Oplike::SCOPE_RESOLUTION: return OpID::SCOPE_RESOLUTION;
         case Oplike::MEMBER_ACCESS: return OpID::MEMBER_ACCESS;
         case Oplike::PTR_MEMBER_ACCESS: return OpID::PTR_MEMBER_ACCESS;
         case Oplike::METHOD_PTR: return OpID::METHOD_PTR;
         case Oplike::ARROW_METHOD_PTR: return OpID::ARROW_METHOD_PTR;
         case Oplike::RANGE: return OpID::RANGE;
         case Oplike::SPREAD: return OpID::SPREAD;
         case Oplike::COMMA: return OpID::COMMA;

         case Oplike::LINE_CMNT: fthru;
         case Oplike::BLOCK_CMNT_OPEN: fthru;
         case Oplike::BLOCK_CMNT_CLOSE: fthru;
         case Oplike::ESC: fthru;
         case Oplike::CALL_OPEN: fthru;
         case Oplike::CALL_CLOSE: fthru;
         case Oplike::INDEX_OPEN: fthru;
         case Oplike::INDEX_CLOSE: fthru;
         case Oplike::LIST_OPEN: fthru;
         case Oplike::LIST_CLOSE: fthru;
         case Oplike::SPECIALIZER_OPEN: fthru;
         case Oplike::SPECIALIZER_CLOSE: fthru;
         case Oplike::PREPROC: fthru;
         case Oplike::EOS: fthru;
         case Oplike::CHAR: fthru;
         case Oplike::STRING: fthru;
         case Oplike::ATTR: return OpID::null;

         case Oplike::ASSERT: return OpID::ASSERT;
         case Oplike::STATIC_ASSERT: return OpID::STATIC_ASSERT;
         case Oplike::ASSUME: return OpID::ASSUME;

         case Oplike::RETURN: return OpID::RETURN;

         case Oplike::CAST: return OpID::CAST;
         case Oplike::UP_CAST: return OpID::UP_CAST;
         case Oplike::BIT_CAST: return OpID::BIT_CAST;
         case Oplike::CONST_CAST: return OpID::CONST_CAST;

         case Oplike::TYPEOF: return OpID::TYPEOF;
         case Oplike::SIZEOF: return OpID::SIZEOF;
         case Oplike::ALIGNOF: return OpID::ALIGNOF;
         case Oplike::ALIGNAS: return OpID::ALIGNAS;
         case Oplike::STRIDEOF: return OpID::STRIDEOF;
         case Oplike::STRIDEAS: return OpID::STRIDEAS;
      }
      UNREACHABLE;
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
      switch (id) {
         case KeywordID::BREAK         : return OpID::BREAK;
         case KeywordID::CONTINUE      : return OpID::CONTINUE;
         
         case KeywordID::ASSERT        : return OpID::ASSERT;
         case KeywordID::ASSUME        : return OpID::ASSUME;
         case KeywordID::STATIC_ASSERT : return OpID::STATIC_ASSERT;
         case KeywordID::RETURN        : return OpID::RETURN;
         
         case KeywordID::TYPEOF        : return OpID::TYPEOF;
         case KeywordID::SIZEOF        : return OpID::SIZEOF;
         case KeywordID::ALIGNAS       : return OpID::ALIGNAS;
         case KeywordID::ALIGNOF       : return OpID::ALIGNOF;
         case KeywordID::STRIDEAS      : return OpID::STRIDEAS;
         case KeywordID::STRIDEOF      : return OpID::STRIDEOF;

         case KeywordID::CAST          : return OpID::CAST;
         case KeywordID::UP_CAST       : return OpID::UP_CAST;
         case KeywordID::BIT_CAST      : return OpID::BIT_CAST;
         case KeywordID::CONST_CAST    : return OpID::CONST_CAST;

         default: UNREACHABLE;
      }
   }
   constexpr Oplike toOplike(const KeywordID kw) {
      switch (kw) {
         case KeywordID::ASSERT: return Oplike::ASSERT;
         case KeywordID::STATIC_ASSERT: return Oplike::STATIC_ASSERT;
         case KeywordID::ASSUME: return Oplike::ASSUME;

         case KeywordID::RETURN: return Oplike::RETURN;

         case KeywordID::CAST: return Oplike::CAST;
         case KeywordID::UP_CAST: return Oplike::UP_CAST;
         case KeywordID::BIT_CAST: return Oplike::BIT_CAST;
         case KeywordID::CONST_CAST: return Oplike::CONST_CAST;

         case KeywordID::TYPEOF: return Oplike::TYPEOF;
         case KeywordID::SIZEOF: return Oplike::SIZEOF;
         case KeywordID::ALIGNOF: return Oplike::ALIGNOF;
         case KeywordID::ALIGNAS: return Oplike::ALIGNAS;
         case KeywordID::STRIDEOF: return Oplike::STRIDEOF;
         case KeywordID::STRIDEAS: return Oplike::STRIDEAS;

         default: UNREACHABLE;
      }
   }

   constexpr KeywordID makeSized_c(const KeywordID id, const char ch) {
      using enum KeywordID;
      assume(id == UINT || id == SINT || id == FLOAT || id == CHAR);

      if (id == CHAR) {
         return ch ? _NOT_A_KEYWORD : id;
      }
      switch (ch) {
         case     WORD_LIT_CHAR: return (KeywordID)(+id + 4);
         case      PTR_LIT_CHAR: return (KeywordID)(+id + 3);
         case OVERLONG_LIT_CHAR: return (KeywordID)(+id + 2);
         case     LONG_LIT_CHAR: return (KeywordID)(+id + 1);
         case                 0: return              id     ;
         case    SHORT_LIT_CHAR: return (KeywordID)(+id - 1);
         case     BYTE_LIT_CHAR: return (KeywordID)(+id - 2);

         default: return KeywordID::_NOT_A_KEYWORD;
      }
   }
   constexpr KeywordID makeSized_n(const KeywordID id, const uint size) {
      using enum KeywordID;
      assume(id == UINT || id == SINT || id == FLOAT || id == CHAR);

      static_assert(+_NOT_A_KEYWORD == 0);
      constexpr KeywordID table[4][8] = {
         { UINT,  UINT_8,  UINT_16,  UINT_32,  UINT_64,  UINT_128,  UINT_256, _NOT_A_KEYWORD},
         { SINT,  SINT_8,  SINT_16,  SINT_32,  SINT_64,  SINT_128,  SINT_256, _NOT_A_KEYWORD},
         {FLOAT, FLOAT_8, FLOAT_16, FLOAT_32, FLOAT_64, FLOAT_128, FLOAT_256, _NOT_A_KEYWORD},
         { CHAR,  CHAR_8,  CHAR_16,  CHAR_32,       {},        {},        {}, _NOT_A_KEYWORD}
      };

      uint offset;
      switch (size) {
         case   0: offset = 0; break;
         case   8: offset = 1; break;
         case  16: offset = 2; break;
         case  32: offset = 3; break;
         case  64: offset = 4; break;
         case 128: offset = 5; break;
         case 256: offset = 6; break;

         default: offset = 7; break;
      }
      switch (id) {
         case UINT : return table[0][offset];
         case SINT : return table[1][offset];
         case FLOAT: return table[2][offset];
         case CHAR : return table[3][offset];
         default: UNREACHABLE;
      }
   }
   #pragma region testsize
   #pragma region testsize_c
   static_assert(makeSized_c(KeywordID::UINT , 0) == KeywordID::UINT );
   static_assert(makeSized_c(KeywordID::SINT , 0) == KeywordID::SINT );
   static_assert(makeSized_c(KeywordID::FLOAT, 0) == KeywordID::FLOAT);

   #define CHECK(base, baseChar, offset) static_assert(makeSized_c(KeywordID::base, offset##_LIT_CHAR) == KeywordID::baseChar##offset)
   #define FOVERLONG FLEXT
   CHECK( UINT, U, BYTE); CHECK( UINT, U, SHORT); CHECK( UINT, U, LONG); CHECK( UINT, U, OVERLONG); CHECK( UINT, U, PTR); CHECK( UINT, U, WORD);
   CHECK( SINT, S, BYTE); CHECK( SINT, S, SHORT); CHECK( SINT, S, LONG); CHECK( SINT, S, OVERLONG); CHECK( SINT, S, PTR); CHECK( SINT, S, WORD);
   CHECK(FLOAT, F, BYTE); CHECK(FLOAT, F, SHORT); CHECK(FLOAT, F, LONG); CHECK(FLOAT, F, OVERLONG); CHECK(FLOAT, F, PTR); CHECK(FLOAT, F, WORD);
   #undef FOVERLONG
   #undef CHECK
   #pragma endregion testsize_c

   #pragma region testsize_n
   static_assert(makeSized_n(KeywordID::UINT , 0) == KeywordID::UINT );
   static_assert(makeSized_n(KeywordID::SINT , 0) == KeywordID::SINT );
   static_assert(makeSized_n(KeywordID::FLOAT, 0) == KeywordID::FLOAT);

   #define CHECK(base, size) static_assert(makeSized_n(KeywordID::base, size) == KeywordID::base##_##size)
   CHECK( UINT, 8); CHECK( UINT, 16); CHECK( UINT, 32); CHECK( UINT, 64); CHECK( UINT, 128); CHECK( UINT, 256);
   CHECK( SINT, 8); CHECK( SINT, 16); CHECK( SINT, 32); CHECK( SINT, 64); CHECK( SINT, 128); CHECK( SINT, 256);
   CHECK(FLOAT, 8); CHECK(FLOAT, 16); CHECK(FLOAT, 32); CHECK(FLOAT, 64); CHECK(FLOAT, 128); CHECK(FLOAT, 256);
   #undef CHECK
   #pragma endregion testsize_n
   #pragma endregion testsize
   #pragma endregion keyword

   //delimiter pair specification
   enum class BlockType : uint8 {
      NONE = 0,      //not a block type

      CALL = 3_m,       //PARENTHESES
      INDEX,        //SQUARE BRACKETS
      LIST,             //CURLY BRACKETS
      SPECIALIZER,      //TRIANGLE BRACKETS

      TERNARY,          //ternary statement
   };
   constexpr auto operator+(const BlockType t) noexcept { return std::to_underlying(t); }
   constexpr OpID getInvoker(const BlockType t) {
      using enum BlockType;
      switch (t) {
         case CALL         : return OpID::CALL;
         case INDEX    : return OpID::INDEX;
         case LIST         : return OpID::INIT_LIST;
         case SPECIALIZER  : return OpID::SPECIALIZER;

         case TERNARY      : return OpID::INLINE_IF;

         default: UNREACHABLE;
      }
   }
   //block delimiter token role
   //!NOTE: could merge into BlockType, but that would not provide any performance advantages because significantly more bytes are going to be used for other Token types anyway
   enum class BlockDelimRole : uint8 {
      null = 0,
      OPEN = 1_m,
      CLOSE = 2_m,
      BOTH = OPEN | CLOSE,
   };
   constexpr auto operator+(const BlockDelimRole t) noexcept { return std::to_underlying(t); } 
   constexpr bool isOpener(const BlockDelimRole t) { return +t & +BlockDelimRole::OPEN; }
   constexpr bool isCloser(const BlockDelimRole t) { return +t & +BlockDelimRole::CLOSE; }

   enum class FundTypeID : uint8 {
      null,
      NULLPTR_T,
      STR,
      FUNCTION_SIGNATURE,
      
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