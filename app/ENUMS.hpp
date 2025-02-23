#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

// #include "CLEF.hpp"
#include "CLEF_DEFINES.hpp"
#include <utility>
// #include <bit>

#include "assert.hpp"

// //for some reason clang and gcc don't like enumerators named FOR_LOOP_PARAMS?
// //and for some reason it tries thinks it is redefining the macro (even though it is in a guarded file)?
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wmacro-redefined"
// #ifndef FOR_LOOP_PARAMS
// #define FOR_LOOP_PARAMS __FOR_LOOP_PARAMS__
// #endif
// #pragma GCC diagnostic pop

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

      MISSING_KEYWORD,
      MISSING_OPERATOR,
      MISSING_BLOCK_DELIM,
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
   enum class NodeType : uint8 {
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
      EXPR,
         STMT,
            DECL,
            FOR_LOOP,
            FOREACH_LOOP,
            WHILE_LOOP,
            DO_WHILE_LOOP,
            IF,
            SWITCH,
            MATCH,
            ASM,
            TRY_CATCH,
      FOR_LOOP_PARAMS_,
      FOREACH_LOOP_PARAMS,
      SWITCH_CASES,
      MATCH_CASES,
      STMT_SEQ,
      ARG_LIST,
      PARAM_LIST,
      SPEC_LIST,
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
      INTERP_STRING,

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

      CHAR_INVOKE,
      STR_INVOKE,
      INTERP_STR_INVOKE,
      TERNARY_INVOKE,


      PREPROCESSOR,

      SCOPE_RESOLUTION,

      INC, //increment
      DEC, //decrement

      MEMBER_ACCESS, // .
      PTR_MEMBER_ACCESS, // ->
      METHOD_PTR, // .*
      ARROW_METHOD_PTR, // ->*

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

      GOTO,
      GOTO_CASE,

      IF,

      SWITCH,
      MATCH,

      TRY_CATCH,
      ASM,

      BREAK,
      CONTINUE,
      
      THROW,
      ASSERT,
      DEBUG_ASSERT,
      STATIC_ASSERT,
      RETURN,

      ALIAS,

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
      ARROW = PTR_MEMBER_ACCESS,
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

   //operator properties bitmask
   enum class [[clang::flag_enum]] OpProps : uint8 {
      NULL = 0,

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
   constexpr OpProps makeOpProps(const bool canBePostfix, const bool canBePrefix, const bool canBeBinary, const bool canBeTypeMod, const bool canBeOpen, const bool canBeClose, const bool isLeftAssoc) {
      OpProps post  = canBePostfix ? OpProps::CAN_BE_POSTFIX : OpProps::NULL;
      OpProps pre   = canBePrefix  ? OpProps::CAN_BE_PREFIX  : OpProps::NULL;
      OpProps bin   = canBeBinary  ? OpProps::CAN_BE_BINARY  : OpProps::NULL;
      OpProps mod   = canBeTypeMod ? OpProps::TYPE_MOD       : OpProps::NULL;
      OpProps open  = canBeOpen    ? OpProps::OPEN_DELIM     : OpProps::NULL;
      OpProps close = canBeClose   ? OpProps::CLOSE_DELIM    : OpProps::NULL;
      OpProps left  = (isLeftAssoc && canBeBinary) ? OpProps::IS_LEFT_ASSOC : OpProps::NULL;
      return post | pre | bin | mod | open | close | left;
   }
   #pragma endregion ops
   #pragma region keyword
   enum class KeywordID : uint8 {
      _NOT_A_KEYWORD = 0,


      VOID,
      AUTO,


      CHAR,
      CHAR_UTF_8,
      CHAR_UTF_16,
      CHAR_UTF_32,
      

      BOOL,
      UBYTE,
      USHORT,
      UINT,
      ULONG,
      UINT_PTR,
      UWORD,

      UINT_8,
      UINT_16,
      UINT_32,
      UINT_64,
      UINT_128,
      UINT_256,


      SIGN_T,
      SBYTE,
      SSHORT,
      SINT,
      SLONG,
      SINT_PTR,
      SWORD,

      SINT_8,
      SINT_16,
      SINT_32,
      SINT_64,
      SINT_128,
      SINT_256,

      FLOAT,

      FLOAT_16,
      FLOAT_32,
      FLOAT_64,
      FLOAT_128,
      FLOAT_256,


      __SIGN_TYPE_OFFSET = SBYTE - UBYTE,

      __FIRST_TEXT_TYPE = CHAR,
      __LAST_TEXT_TYPE = CHAR_UTF_32,
      __FIRST_INT_TYPE = BOOL,
      __LAST_INT_TYPE = SINT_256,
      __FIRST_UINT_TYPE = BOOL,
      __LAST_UINT_TYPE = UINT_256,
      __FIRST_SINT_TYPE = SIGN_T,
      __LAST_SINT_TYPE = SINT_256,
      __FIRST_FLOAT_TYPE = FLOAT,
      __LAST_FLOAT_TYPE = FLOAT_256,
      __FIRST_NUM_TYPE = __FIRST_INT_TYPE,
      __LAST_NUM_TYPE = __LAST_FLOAT_TYPE,
      __FIRST_TYPE = VOID,
      __LAST_TYPE = FLOAT_256,







      CLASS,
      STRUCT,
      INTERFACE,
      UNION,
      ENUM,
      ENUM_UNION,
      MASK,
      NAMESPACE,
      FUNC,

      __FIRST_OBJ_TYPE = CLASS,
      __LAST_OBJ_TYPE = FUNC,

      CONST,
      CONSTEXPR,
      IMMEDIATE,
      VIEW,
      NON_OWNING,
      MUTABLE,
      VOLATILE,
      ATOMIC,
      EXPLICIT,

      __FIRST_TYPE_QUAL = CONST,
      __LAST_TYPE_QUAL = EXPLICIT,

      CAST,
      UP_CAST,
      DYN_CAST,
      BIT_CAST,
      CONST_CAST,

      __FIRST_CAST = CAST,
      __LAST_CAST = CONST_CAST,

      TYPEOF,
      SIZEOF,
      ALIGNAS,
      ALIGNOF,

      __FIRST_TYPE_ASSESSMENT = TYPEOF,
      __LAST_TYPE_ASSESSMENT = ALIGNOF,
      

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
      TRY,
      CATCH,
      THROW,

      __FIRST_CONTROL_FLOW = GOTO,
      __LAST_CONTROL_FLOW = THROW,




      THIS,
      SELF,

      TRUE,
      FALSE,

      NULLPTR,

      __FIRST_VALUE = THIS,
      __LAST_VALUE = NULLPTR,

      ASSERT,
      STATIC_ASSERT,
      DEBUG_ASSERT,
      SAFE_MODE_ASSERT,

      __FIRST_ASSERT = ASSERT,
      __LAST_ASSERT = SAFE_MODE_ASSERT,

      LET,
      ALIAS,

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
   constexpr bool isQualifier(const KeywordID id) noexcept { return __BETWEEN(TYPE_QUAL); }
   constexpr bool isCast(const KeywordID id) noexcept { return __BETWEEN(CAST); }
   constexpr bool isControlFlow(const KeywordID id) noexcept { return __BETWEEN(CONTROL_FLOW); }
   constexpr bool isUnspec(const KeywordID id) noexcept { return __BETWEEN(UNSPEC); }
   constexpr bool isValue(const KeywordID id) noexcept { return __BETWEEN(VALUE); }
   #undef __BETWEEN

   constexpr bool isC_FunctionLike(const KeywordID id) noexcept {//no specializer parameters
      using enum KeywordID;
      switch (id) {
         case ASSERT:
         case DEBUG_ASSERT:
         case STATIC_ASSERT:
         case SAFE_MODE_ASSERT:
         
         case RETURN:
         case THROW:
         
         case TYPEOF:
         case SIZEOF:
         case ALIGNAS:
         case ALIGNOF:
            return true;
         
         default:
            return false;
      }
   }
   constexpr OpID toOpID(const KeywordID id) {
      switch (id) {
         case KeywordID::BREAK         : return OpID::BREAK;
         case KeywordID::CONTINUE      : return OpID::CONTINUE;
         
         case KeywordID::THROW         : return OpID::THROW;
         case KeywordID::ASSERT        : return OpID::ASSERT;
         case KeywordID::DEBUG_ASSERT  : return OpID::DEBUG_ASSERT;
         case KeywordID::STATIC_ASSERT : return OpID::STATIC_ASSERT;
         case KeywordID::RETURN        : return OpID::RETURN;

         case KeywordID::ALIAS         : return OpID::ALIAS;

         default: UNREACHABLE;
      }
   }
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
   constexpr OpID getInvoker(const BlockType t) {
      using enum BlockType;
      switch (t) {
         case CALL         : return OpID::CALL_INVOKE;
         case SUBSCRIPT    : return OpID::SUBSCRIPT_INVOKE;
         case INIT_LIST    : return OpID::LIST_INVOKE;
         case SPECIALIZER  : return OpID::SPECIALIZER_INVOKE;

         case QUOTES_CHAR  : return OpID::CHAR_INVOKE;
         case QUOTES_STR   : return OpID::STR_INVOKE;
         case QUOTES_INTERP: return OpID::INTERP_STR_INVOKE;

         case TERNARY      : return OpID::TERNARY_INVOKE;

         default: UNREACHABLE;
      }
   }
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
      
      POINTER,

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
      
      VOID,
      AUTO,

      CHAR,
      CHAR8,
      CHAR16,
      CHAR32,

      BOOL,
      UINT8,
      UINT16,
      UINT32,
      UINT64,
      UINT128,
      UINT256,

      SIGN_T,
      SINT8,
      SINT16,
      SINT32,
      SINT64,
      SINT128,
      SINT256,

      FLOAT16,
      FLOAT32,
      FLOAT64,
      FLOAT128,
      FLOAT256,


      ENUM,
      MASK,
      UNION,
      NAMESPACE,
      INTERFACE,
      STRUCT,
      CLASS,
   };
}

#endif //ENUMS_HPP