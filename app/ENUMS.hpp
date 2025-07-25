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
      BAD_TYPE_DECL,

      BAD_PREPROC,

      BAD_ATTR,
      //ast/ast node errors
      BAD_LIT_CAST,
      BAD_NODE_CAST,

      //typechecking errors
      TYPECHECK_ERR,
   };
   constexpr auto      operator+(const ErrCode t) noexcept { return std::to_underlying(t); }
   
   //!enum of symbol type codes
   enum class SymbolType : uint8 {
      null = 0,

      EXTERN_IDEN,

      VAR,
      FUNC,
      MACRO,

      ATTRIBUTE,

      LABEL,
      
      __TYPE_BIT = 8_m,
         EXTERN_TYPE,
         FUND_TYPE,
         CLASS,
         STRUCT,
         TRAIT,
         UNION,
         ENUM,
         ENUM_UNION,
         MASK,
         NAMESPACE,

         GENERIC,

         INDIR,
   };
   constexpr auto operator+(const SymbolType t) noexcept { return std::to_underlying(t); }
   constexpr bool isType(SymbolType t) noexcept { return +t & +SymbolType::__TYPE_BIT; }
   constexpr bool isNonType(SymbolType t) noexcept { return t != SymbolType::null && !isType(t); }

   //!enum of AST node type codes
   enum class NodeType : uint8 {
      ERROR = 0xFF,  //CLEF INTERNAL ERROR
      NONE = 0x00,   //to-be determined

      RAW_IDEN,
      IDEN,
      SCOPE,
      LITERAL,
      EXPR,
         STMT,
            DECL,
            FUNC_DEF,
            MACRO_DEF,
            MAKE_TYPE,
            FOR_LOOP,
            FOREACH_LOOP,
            WHILE_LOOP,
            DO_WHILE_LOOP,
            IF,
            SWITCH,
            MATCH,
            ASM,
      SWITCH_CASES,
      MATCH_CASES,
      STMT_SEQ,
      ARG_LIST,
   };
   constexpr auto operator+(const NodeType t) noexcept { return std::to_underlying(t); }
   constexpr bool canDownCastTo(const NodeType from, const NodeType to) {
      switch (to) {
         case NodeType::EXPR:
            return from >= NodeType::EXPR && from <= NodeType::ASM;
         case NodeType::STMT:
            return from >= NodeType::STMT && from <= NodeType::ASM;
         
         default:
            return from == to;
      };
   }
   
   //!token types bitmask
   enum class TokenType : uint8 {
      NONE = 0,
      
      IDEN,
      MACRO_INVOKE,
      KEYWORD,
      UINT_NUM,
      SINT_NUM,
      REAL_NUM,

      __OPLIKE = 0x80,
      OP = 0x81,
      PREPROC_INIT,
      PREPROC_EOS,
      EOS,
      ESC,
      BLOCK_DELIM,
      PTXT_SEG,
   };
   constexpr auto      operator+(const TokenType t) noexcept { return std::to_underlying(t); }
   constexpr TokenType operator&(const TokenType lhs, const TokenType rhs) noexcept { return (TokenType)((+lhs) & (+rhs)); }

   constexpr bool isOperator(const TokenType t) { return t == TokenType::OP; }
   constexpr bool isBlockLike(const TokenType t) { return t == TokenType::BLOCK_DELIM; }
   constexpr bool isOperand(const TokenType t) { return !+(t & TokenType::__OPLIKE); }
      constexpr bool isNumber(const TokenType t) { return t == TokenType::UINT_NUM || t == TokenType::SINT_NUM || t == TokenType::REAL_NUM; }
   #pragma region ops
   //!NOTE: OpDefTable relies on the order of many of the enumerators
   enum class OpID : uint8 {
      NULL = 0, //nop or not an operator
      
      CALL_INVOKE, //parens
      SUBSCRIPT_INVOKE, //square brackets

      INC, //increment
      DEC, //decrement

      RANGE,
      
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
      

      THREE_WAY_COMP, //AKA spaceship
      LESSER, //less than
      GREATER, //greater than
      LESSER_OR_EQ, //less than or equal to
      GREATER_OR_EQ, //greater than or equal to

      IS_EQUAL, //equality comparison
      IS_UNEQUAL, //inequality comparison

      COALESCE, //null coalescing

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

      ESCAPE, //escape character
      EOS,

      STRING,
      CHAR,
      INTERP_STRING,

      ATTRIBUTE, //@

      LINE_CMNT,
      BLOCK_CMNT,
         BLOCK_CMNT_OPEN,
         BLOCK_CMNT_CLOSE,

      //parens
         CALL_OPEN,
         CALL_CLOSE,
      //square brackets
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

      MEMBER_ACCESS, // .
      PTR_MEMBER_ACCESS, // ->
      METHOD_PTR, // .*
      ARROW_METHOD_PTR, // ->*

      SPREAD,

      INLINE_IF, //ternary operator opener
      INLINE_ELSE, //ternary operator closer

      ASSIGN,
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

      ASM,

      BREAK,
      CONTINUE,
      
      THROW,
      ASSERT,
      DEBUG_ASSERT,
      STATIC_ASSERT,
      ASSUME,
      RETURN,

      ALIAS,

      CAST,
      UP_CAST,
      DYN_CAST,
      BIT_CAST,
      CONST_CAST,

      LET,
      MAKE_TYPE,

      DEF_FUNC_PARAMS,
      DEF_MACRO_PARAMS,

      //preprocessor directive operators
      PREPROC_IMPORT,
      PREPROC_LINK,
      PREPROC_EMBED,

      //aliases
      LABEL_DELIM = INLINE_ELSE,
      EACH_OF = LABEL_DELIM,
      EXTENDS = LABEL_DELIM,
      ADDRESS_OF = BIT_AND,
      REFERENCE = ADDRESS_OF,
      RAW_PTR = MUL,
      DEREF = RAW_PTR,
      SLICE = SUBSCRIPT_INVOKE,
      VARIADIC_PARAM = SPREAD,
      ARROW = PTR_MEMBER_ACCESS,
      UNARY_PLUS = ADD,
      UNARY_MINUS = SUB,
      RADIX_POINT = MEMBER_ACCESS,

      //helpers
      __FIRST_PSEUDO_OP = FOR,
      __LAST_PSEUDO_OP = PREPROC_EMBED,
      __FIRST_CAST = CAST,
      __LAST_CAST = CONST_CAST,
      __FIRST_PREPROC = PREPROC_IMPORT,
      __LAST_PREPROC = PREPROC_EMBED,
      __FIRST_LOOP = FOR,
      __LAST_LOOP = DO_WHILE,
   };

   constexpr auto operator+(const OpID x) { return std::to_underlying(x); }
   constexpr OpID operator&(const OpID lhs, const OpID rhs) { return (OpID)(+lhs & +rhs); }
   constexpr OpID& operator&=(OpID& lhs, const OpID rhs) { lhs = lhs & rhs; return lhs; }
   constexpr bool isOperator(const OpID x) { return x < OpID::__FIRST_PSEUDO_OP; }

   constexpr bool isDecl(const OpID op) { return op == OpID::LET; }

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

   constexpr bool isPreproc(const OpID op) { return op >= OpID::__FIRST_PREPROC && op <= OpID::__LAST_PREPROC; }

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
      ASSUME,

      __FIRST_ASSERT = ASSERT,
      __LAST_ASSERT = ASSUME,

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
   constexpr bool isQualLike(const KeywordID id) noexcept { return __BETWEEN(QUAL_LIKE); }
      constexpr bool isQualifier(const KeywordID id) noexcept { return __BETWEEN(QUAL); }
      constexpr bool isScopeMod(const KeywordID id) noexcept { return __BETWEEN(SCOPE_MOD); }
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
         case ASSUME:
         
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
         case KeywordID::ASSUME        : return OpID::ASSUME;
         case KeywordID::STATIC_ASSERT : return OpID::STATIC_ASSERT;
         case KeywordID::RETURN        : return OpID::RETURN;

         case KeywordID::ALIAS         : return OpID::ALIAS;
         
         case KeywordID::TYPEOF        : TODO;
         case KeywordID::SIZEOF        : TODO;
         case KeywordID::ALIGNAS       : TODO;
         case KeywordID::ALIGNOF       : TODO;

         case KeywordID::CAST          : return OpID::CAST;
         case KeywordID::UP_CAST       : return OpID::UP_CAST;
         case KeywordID::DYN_CAST      : return OpID::DYN_CAST;
         case KeywordID::BIT_CAST      : return OpID::BIT_CAST;
         case KeywordID::CONST_CAST    : return OpID::CONST_CAST;

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
      SUBSCRIPT,        //SQUARE BRACKETS
      LIST,        //CURLY BRACKETS
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
         case LIST         : return OpID::LIST_INVOKE;
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
      null = 0,
      OPEN = 1_m,
      CLOSE = 2_m,
      BOTH = OPEN | CLOSE,
   };
   constexpr auto operator+(const BlockDelimRole t) noexcept { return std::to_underlying(t); } 
   constexpr bool isOpener(const BlockDelimRole t) { return +t & +BlockDelimRole::OPEN; }
   constexpr bool isCloser(const BlockDelimRole t) { return +t & +BlockDelimRole::CLOSE; }

   //plaintext segment type
   enum class PtxtType : uint8 {
      null = 0,

      CHAR = 1,
      WCHAR,
      CHAR8,
      CHAR16,
      CHAR32,

      STR = 9,
      WSTR,
      STR8,
      STR16,
      STR32,

      UNPROCESSED_STR = 25,
      UNPROCESSED_WSTR,
      UNPROCESSED_STR8,
      UNPROCESSED_STR16,
      UNPROCESSED_STR32,

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

      BOOL,
      CHAR,
      STRING,
      INTERP_STR,
      FORMAT,
      REGEX,

      TYPEID,
   };
   constexpr auto operator+(const LitType t) noexcept { return std::to_underlying(t); }


   enum class FundTypeID : uint8 {
      null,
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