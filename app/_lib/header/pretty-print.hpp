#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include "CLEF.hpp"

#include "str_slice.hpp"

#undef NULL

namespace clef {
   constexpr const mcsl::str_slice toString(const TokenType);
   constexpr const mcsl::str_slice toString(const OpID);
   constexpr const mcsl::str_slice toString(const OpProps);
   constexpr const mcsl::str_slice toString(const KeywordID);
   constexpr const mcsl::str_slice toString(const BlockType);
   constexpr const mcsl::str_slice toString(const BlockDelimRole);
   constexpr const mcsl::str_slice toString(const FundTypeID);
   constexpr const mcsl::str_slice toString(const QualMask);

   constexpr const mcsl::str_slice getSuffix(const FundTypeID);
};



#pragma region inlinesrc

#define CASE(enumerator, name) case enumerator: return mcsl::str_slice::make_from_cstr(name)

constexpr const mcsl::str_slice clef::toString(const TokenType type) {
   using enum TokenType;
   switch (type) {
      CASE(NONE, "none");

      CASE(IDEN, "IDENTIFIER");
      CASE(MACRO_INVOKE, "MACRO_INVOKE");
      CASE(KEYWORD, "KEYWORD");
      CASE(UINT_NUM, "UNSIGNED_INTEGER");
      CASE(SINT_NUM, "SIGNED_INTEGER");
      CASE(REAL_NUM, "REAL_NUMBER");
      CASE(BOOL_LIT, "BOOLEAN");
      CASE(CHAR_LIT, "CHAR");
      CASE(STR_LIT, "STRING");
      
      CASE(OP, "OPERATOR");
      CASE(EOS, "END OF STATEMENT");
      CASE(BLOCK_DELIM, "BLOCK_DELIMITER");

      CASE(ATTR, "ATTRIBUTE");

      CASE(PREPROC_INIT, "PREPROCESSOR");

      case __OPLIKE: UNREACHABLE;
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const OpID op) {
   using enum OpID;
   switch (op) {
      CASE(null, "null");

      CASE(CALL, "()");
      CASE(INDEX, "[]");
      CASE(INIT_LIST, "{}");
      CASE(SPECIALIZER, "<::>");

      CASE(INLINE_IF, "?");
      CASE(INLINE_ELSE, ":");

      CASE(SCOPE_RESOLUTION, "::");

      CASE(INC, "++");
      CASE(DEC, "--");

      CASE(MEMBER_ACCESS, ".");
      CASE(PTR_MEMBER_ACCESS, "->");
      CASE(METHOD_PTR, ".*");
      CASE(ARROW_METHOD_PTR, "->*");

      CASE(SLICE, "[]");

      CASE(RANGE, "..");
      CASE(SPREAD, "...");

      CASE(ADD, "+");
      CASE(SUB, "-");
      CASE(MUL, "*");
      CASE(DIV, "/");
      CASE(MOD, "%");
      CASE(EXP, "^^");

      CASE(LOGICAL_NOT, "!");
      CASE(LOGICAL_AND, "&&");
      CASE(LOGICAL_OR, "||");

      CASE(BIT_NOT, "~");
      CASE(BIT_AND, "&");
      CASE(BIT_OR, "|");
      CASE(BIT_XOR, "^");
      CASE(SHL, "<<");
      CASE(SHR, ">>");


      CASE(THREE_WAY_COMP, "<=>");
      CASE(LESSER, "<");
      CASE(GREATER, ">");
      CASE(LESSER_OR_EQ, "<=");
      CASE(GREATER_OR_EQ, ">=");

      CASE(IS_EQUAL, "==");
      CASE(IS_UNEQUAL, "!=");

      CASE(COALESCE, "??");

      CASE(ASSIGN, "=");
      CASE(ADD_ASSIGN, "+=");
      CASE(SUB_ASSIGN, "-=");
      CASE(MUL_ASSIGN, "*=");
      CASE(DIV_ASSIGN, "/=");
      CASE(MOD_ASSIGN, "%=");
      CASE(EXP_ASSIGN, "^^=");
      CASE(SHL_ASSIGN, "<<=");
      CASE(SHR_ASSIGN, ">>=");
      CASE(AND_ASSIGN, "&=");
      CASE(XOR_ASSIGN, "^=");
      CASE(OR_ASSIGN, "|=");
      CASE(COALESCE_ASSIGN, "??=");

      CASE(COMMA, ",");


      CASE(FOR, "FOR");
      CASE(FOREACH, "FOREACH");
      CASE(WHILE, "WHILE");
      CASE(DO_WHILE, "DO_WHILE");

      CASE(GOTO, "GOTO");
      CASE(GOTO_CASE, "GOTO_CASE");

      CASE(IF, "IF");

      CASE(SWITCH, "SWITCH");
      CASE(MATCH, "MATCH");

      CASE(ASM, "ASM");

      CASE(BREAK, "BREAK");
      CASE(CONTINUE, "CONTINUE");

      CASE(ASSERT, "ASSERT");
      CASE(STATIC_ASSERT, "STATIC_ASSERT");
      CASE(ASSUME, "ASSUME");

      CASE(RETURN, "RETURN");

      CASE(CAST, "CAST");
      CASE(UP_CAST, "UP_CAST");
      CASE(BIT_CAST, "BIT_CAST");
      CASE(CONST_CAST, "CONST_CAST");

      CASE(TYPEOF, "TYPEOF");
      CASE(SIZEOF, "SIZEOF");
      CASE(ALIGNOF, "ALIGNOF");
      CASE(STRIDEOF, "STRIDEOF");
      CASE(ALIGNAS, "ALIGNAS");
      CASE(STRIDEAS, "STRIDEAS");


      CASE(LET, "LET");
      CASE(MAKE_TYPE, "MAKE_TYPE");
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const OpProps props) {
   using enum OpProps;
   switch (props) {
      CASE(null, "null");

      CASE(POSTFIX, "POSTFIX");
      CASE(PREFIX, "PREFIX");
      CASE(POSTFIX | PREFIX, "UNARY");
      CASE(INFIX_LEFT, "INFIX_LEFT");
      CASE(INFIX_RIGHT, "INFIX_RIGHT");
      CASE(PREFIX | INFIX_LEFT, "PREFIX or INFIX_LEFT");
      CASE(PREFIX | INFIX_RIGHT, "PREFIX or INFIX_RIGHT");

      CASE(DELIM, "DELIMITER");
      CASE(OPEN_DELIM, "OPENING DELIMITER");
      CASE(CLOSE_DELIM, "CLOSING DELIMITER");

      CASE(TYPE_MOD, "TYPE MODIFIER");
      CASE(TYPE_MOD | INFIX_LEFT, "TYPE MODIFIER or INFIX_LEFT");
      CASE(TYPE_MOD | INFIX_RIGHT, "TYPE MODIFIER or INFIX_RIGHT");
      CASE(TYPE_MOD | PREFIX | INFIX_LEFT, "TYPE MODIFIER or PREFIX or INFIX_LEFT");
      CASE(TYPE_MOD | PREFIX | INFIX_RIGHT, "TYPE MODIFIER or PREFIX or INFIX_RIGHT");

      default: UNREACHABLE;
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const KeywordID kw) {
   using enum KeywordID;
   switch (kw) {
      CASE(_NOT_A_KEYWORD, "not_a_keyword");


      CASE(VOID, "void");
      CASE(AUTO, "auto");

      CASE(CHAR, "char");
      CASE(CHAR_8, "char8");
      CASE(CHAR_16, "char16");
      CASE(CHAR_32, "char32");
      
      CASE(BOOL, "bool");
      CASE(UBYTE, "ubyte");
      CASE(USHORT, "ushort");
      CASE(UINT, "uint");
      CASE(ULONG, "ulong");
      CASE(UOVERLONG, "uoverlong");
      CASE(UPTR, "uptr");
      CASE(UWORD, "uword");
      CASE(UINT_8, "uint8");
      CASE(UINT_16, "uint16");
      CASE(UINT_32, "uint32");
      CASE(UINT_64, "uint64");
      CASE(UINT_128, "uint128");
      CASE(UINT_256, "uint256");

      CASE(SIGN_T, "sign_t");
      CASE(SBYTE, "sbyte");
      CASE(SSHORT, "sshort");
      CASE(SINT, "sint");
      CASE(SLONG, "slong");
      CASE(SOVERLONG, "soverlong");
      CASE(SPTR, "sptr");
      CASE(SWORD, "sword");
      CASE(SINT_8, "sint8");
      CASE(SINT_16, "sint16");
      CASE(SINT_32, "sint32");
      CASE(SINT_64, "sint64");
      CASE(SINT_128, "sint128");
      CASE(SINT_256, "sint256");

      CASE(FP3, "fp3");
      CASE(FBYTE, "fbyte");
      CASE(FSHORT, "fshort");
      CASE(FLOAT, "float");
      CASE(FLONG, "flong");
      CASE(FLEXT, "flext");
      CASE(FPTR, "fptr");
      CASE(FWORD, "fword");
      CASE(FLOAT_8, "float8");
      CASE(FLOAT_16, "float16");
      CASE(FLOAT_32, "float32");
      CASE(FLOAT_64, "float64");
      CASE(FLOAT_80, "float80");
      CASE(FLOAT_128, "float128");
      CASE(FLOAT_256, "float256");


      CASE(TYPE, "type");

      CASE(CLASS, "class");
      CASE(STRUCT, "struct");
      CASE(TRAIT, "trait");
      CASE(UNION, "union");
      CASE(ENUM, "enum");
      CASE(ENUM_UNION, "enumunion");
      CASE(MASK, "mask");
      CASE(NAMESPACE, "namespace");
      CASE(TUPLE, "tuple");
      CASE(FUNC, "func");
      CASE(MACRO, "macro");


      
      CASE(CONST, "const");
      CASE(CONSTEXPR, "constexpr");
      CASE(IMMEDIATE, "immediate");
      CASE(VIEW, "view");
      CASE(NON_OWNING, "notown");
      CASE(MUTABLE, "mutable");
      CASE(VOLATILE, "volatile");
      CASE(ATOMIC, "atomic");
      CASE(EXTERN, "extern");
      CASE(INLINE, "inline");
      CASE(NOEXCEPT, "noexcept");
      CASE(VIRTUAL, "virtual");

      CASE(PUBLIC, "public");
      CASE(PRIVATE, "private");
      CASE(PROTECTED, "protected");

      CASE(STATIC, "static");

      CASE(CAST, "cast");
      CASE(UP_CAST, "up_cast");
      CASE(BIT_CAST, "bit_cast");
      CASE(CONST_CAST, "const_cast");

      CASE(TYPEOF, "typeof");
      CASE(SIZEOF, "sizeof");
      CASE(ALIGNAS, "alignas");
      CASE(ALIGNOF, "alignof");
      CASE(STRIDEAS, "strideas");
      CASE(STRIDEOF, "strideof");

      CASE(GOTO, "goto");
      CASE(RETURN, "return");
      CASE(IF, "if");
      CASE(ELSE, "else");
      CASE(FOR, "for");
      CASE(FOREACH, "foreach");
      CASE(WHILE, "while");
      CASE(DO, "do");
      CASE(BREAK, "break");
      CASE(CONTINUE, "continue");
      CASE(SWITCH, "switch");
      CASE(MATCH, "match");
      CASE(CASE, "case");
      CASE(DEFAULT, "default");

      CASE(CATCH, "catch");
      CASE(THROW, "throw");

      CASE(THIS, "this");
      CASE(SELF, "self");

      CASE(TRUE, "true");
      CASE(FALSE, "false");
      CASE(NULLPTR, "null");

      CASE(ASSERT, "assert");
      CASE(STATIC_ASSERT, "static_assert");
      CASE(ASSUME, "assume");

      CASE(LET, "let");
      CASE(USING, "using");
      CASE(ASM, "asm");
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const BlockType type) {
   using enum BlockType;
   switch (type) {
      CASE(NONE, "none");

      CASE(CALL, "CALL");
      CASE(INDEX, "INDEX");
      CASE(LIST, "INITIALIZER_LIST");
      CASE(SPECIALIZER, "SPECIALIZER");

      CASE(TERNARY, "TERNARY_STATEMENT");
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const BlockDelimRole role) {
   using enum BlockDelimRole;
   switch (role) {
      CASE(null, "null");
      CASE(OPEN, "OPEN");
      CASE(CLOSE, "CLOSE");
      CASE(BOTH, "BOTH");
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const FundTypeID type) {
   using enum FundTypeID;
   switch (type) {
      CASE(null, "\033[31mNULL TYPE\033[39m");
      CASE(FUNCTION_SIGNATURE, "FUNCTION_SIGNATURE");

      CASE(VOID, "void");
      CASE(AUTO, "auto");

      CASE(CHAR, "char");
      CASE(CHAR8, "char8");
      CASE(CHAR16, "char16");
      CASE(CHAR32, "char32");

      CASE(BOOL, "bool");
      CASE(UINT8, "uint8");
      CASE(UINT16, "uint16");
      CASE(UINT32, "uint32");
      CASE(UINT64, "uint64");
      CASE(UINT128, "uint128");
      CASE(UINT256, "uint256");

      CASE(SIGN_T, "sign_t");
      CASE(SINT8, "sint8");
      CASE(SINT16, "sint16");
      CASE(SINT32, "sint32");
      CASE(SINT64, "sint64");
      CASE(SINT128, "sint128");
      CASE(SINT256, "sint256");

      CASE(FLOAT16, "float16");
      CASE(FLOAT32, "float32");
      CASE(FLOAT64, "float64");
      CASE(FLOAT80, "float80");
      CASE(FLOAT128, "float128");
      CASE(FLOAT256, "float256");

      case FundTypeID::__CHARS:
      case FundTypeID::__UINTS:
      case FundTypeID::__SINTS:
      case FundTypeID::__FLOATS:
      case FundTypeID::__INTS:
      case FundTypeID::__NUMS:
         UNREACHABLE;
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::toString(const QualMask quals) {
   using enum QualMask;
   switch (quals) {
      CASE(CONST, "const");
      CASE(CONSTEXPR, "constexpr");
      CASE(IMMEDIATE, "immediate");
      CASE(VIEW, "view");
      CASE(NON_OWNING, "notown");
      CASE(MUTABLE, "mutable");
      CASE(VOLATILE, "volatile");
      CASE(ATOMIC, "atomic");
      CASE(EXTERN, "extern");
      CASE(INLINE, "inline");
      CASE(NOEXCEPT, "noexcept");
      CASE(VIRTUAL, "virtual");
      CASE(VARIADIC, "...");

      CASE(PUBLIC, "public");
      CASE(PRIVATE, "private");
      CASE(PROTECTED, "protected");

      CASE(_no_quals, "");

      default: TODO;
   }
   UNREACHABLE;
}

constexpr const mcsl::str_slice clef::getSuffix(const FundTypeID id) {
   using enum FundTypeID;
   switch (id) {
      CASE(UINT8,   "_u8");
      CASE(UINT16,  "_u16");
      CASE(UINT32,  "_u32");
      CASE(UINT64,  "_u64");
      CASE(UINT128, "_u128");
      CASE(UINT256, "_u256");

      CASE(SINT8,   "_s8");
      CASE(SINT16,  "_s16");
      CASE(SINT32,  "_s32");
      CASE(SINT64,  "_s64");
      CASE(SINT128, "_s128");
      CASE(SINT256, "_s256");

      // CASE(FLOAT8,   "_f8");
      CASE(FLOAT16,  "_f16");
      CASE(FLOAT32,  "_f32");
      CASE(FLOAT64,  "_f64");
      CASE(FLOAT128, "_f128");
      CASE(FLOAT256, "_f256");

      default: UNREACHABLE;
   }
}

#undef CASE

#pragma endregion inlinesrc

#endif //PRETTY_PRINT_HPP