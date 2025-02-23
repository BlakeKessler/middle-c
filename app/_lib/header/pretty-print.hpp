#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include "CLEF.hpp"

namespace clef {
   constexpr const char* toString(const NodeType);
   constexpr const char* toString(const TokenType);
   constexpr const char* toString(const TokenType);
   constexpr const char* toString(const OpID);
   constexpr const char* toString(const OpProps);
   constexpr const char* toString(const KeywordID);
   constexpr const char* toString(const BlockType);
   constexpr const char* toString(const BlockDelimRole);
   constexpr const char* toString(const PtxtType);
   constexpr const char* toString(const LitType);
   // constexpr const char* toString(const TypeQualMask);
   // constexpr const char* toString(const SymbolProp);
   constexpr const char* toString(const FundTypeID);
};



#pragma region inlinesrc

#define CASE(enumerator, name) case enumerator: return name

constexpr const char* clef::toString(const NodeType type) {
   using enum NodeType;
   switch (type) {
      CASE(ERROR, "error");
      CASE(NONE, "none");

      CASE(IDEN, "IDENTIFIER");
      CASE(VAR, "VARIABLE");
      CASE(FUNC, "FUNCTION");
      CASE(TYPE, "TYPE");
      CASE(VAR_PARAM, "VARIADIC_PARAM");
      CASE(FUND_TYPE, "FUNDEMENTAL_TYPE");
      CASE(FUNC_SIG, "FUNCTION_SIGNATURE");
      CASE(ENUM, "ENUM");
      CASE(MASK, "MASK");
      CASE(UNION, "UNION");
      CASE(NAMESPACE, "NAMESPACE");
      CASE(INTERFACE, "INTERFACE");
      CASE(STRUCT, "STRUCT");
      CASE(CLASS, "CLASS");
      CASE(GENERIC, "GENERIC");
      CASE(SCOPE, "SCOPE");
      CASE(LITERAL, "LITERAL");
      CASE(EXPR, "EXPRESSION");
      CASE(STMT, "STATEMENT");
      CASE(DECL, "DECLARATION");
      CASE(FOR_LOOP, "FOR_LOOP");
      CASE(FOREACH_LOOP, "FOREACH_LOOP");
      CASE(WHILE_LOOP, "WHILE_LOOP");
      CASE(DO_WHILE_LOOP, "DO_WHILE_LOOP");
      CASE(IF, "IF");
      CASE(SWITCH, "SWITCH");
      CASE(MATCH, "MATCH");
      CASE(ASM, "ASM");
      CASE(TRY_CATCH, "TRY_CATCH");
      CASE(FOR_LOOP_PARAMS_, "FOR_LOOP_PARAMS");
      CASE(FOREACH_LOOP_PARAMS, "FOREACH_LOOP_PARAMS");
      CASE(SWITCH_CASES, "SWITCH_CASES");
      CASE(MATCH_CASES, "MATCH_CASES");
      CASE(STMT_SEQ, "STATEMENT_SEQUENCE");
      CASE(ARG_LIST, "ARGUMENT_LIST");
      CASE(PARAM_LIST, "PARAMETER_LIST");
      CASE(SPEC_LIST, "SPECIALIZER_LIST");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const TokenType type) {
   using enum TokenType;
   switch (type) {
      CASE(NONE, "none");

      CASE(IDEN, "IDENTIFIER");
      CASE(KEYWORD, "KEYWORD");
      CASE(INT_NUM, "INTEGER");
      CASE(REAL_NUM, "REAL_NUMBER");
      
      CASE(OP, "OPERATOR");
      CASE(PREPROC_INIT, "PREPROCESSOR_OPEN");
      CASE(PREPROC_EOS, "PREPROCESSOR_CLOSE");
      CASE(EOS, "END OF STATEMENT");
      CASE(ESC, "ESCAPE_CHARACTER");
      CASE(BLOCK_DELIM, "BLOCK_DELIMITER");
      CASE(PTXT_SEG, "PLAINTEXT_SEGMENT");

      case __OPLIKE: UNREACHABLE;
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const OpID op) {
   using enum OpID;
   switch (op) {
      CASE(NULL, "null");

      CASE(ESCAPE, "\\");
      CASE(EOS, ";");

      CASE(CHAR, "\'");
      CASE(STRING, "\"");
      CASE(INTERP_STRING, "`");

      CASE(LINE_CMNT, "//");
      CASE(BLOCK_CMNT, "/**/");
      CASE(BLOCK_CMNT_OPEN, "/*");
      CASE(BLOCK_CMNT_CLOSE, "*/");

      CASE(CALL_INVOKE, "()");
      CASE(CALL_OPEN, "(");
      CASE(CALL_CLOSE, ")");
      CASE(SUBSCRIPT_INVOKE, "[]");
      CASE(SUBSCRIPT_OPEN, "[");
      CASE(SUBSCRIPT_CLOSE, "]");
      CASE(LIST_INVOKE, "{}");
      CASE(LIST_OPEN, "{");
      CASE(LIST_CLOSE, "}");
      CASE(SPECIALIZER_INVOKE, "<::>");
      CASE(SPECIALIZER_OPEN, "<:");
      CASE(SPECIALIZER_CLOSE, ":>");

      CASE(CHAR_INVOKE, "\'\'");
      CASE(STR_INVOKE, "\"\"");
      CASE(INTERP_STR_INVOKE, "``");
      CASE(TERNARY_INVOKE, "?:");


      CASE(PREPROCESSOR, "#");

      CASE(SCOPE_RESOLUTION, "::");

      CASE(INC, "++");
      CASE(DEC, "--");

      CASE(MEMBER_ACCESS, ".");
      CASE(PTR_MEMBER_ACCESS, "->");
      CASE(METHOD_PTR, ".*");
      CASE(ARROW_METHOD_PTR, "->*");

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
      CASE(SHIFT_LEFT, "<<");
      CASE(SHIFT_RIGHT, ">>");


      CASE(UNIQUE_PTR, "@");
      CASE(SHARED_PTR, "$");
      CASE(WEAK_PTR, "`");
      CASE(ITERATOR, "%");


      CASE(THREE_WAY_COMP, "<=>");
      CASE(LESSER, "<");
      CASE(GREATER, ">");
      CASE(LESSER_OR_EQ, "<=");
      CASE(GREATER_OR_EQ, ">=");

      CASE(IS_EQUAL, "==");
      CASE(IS_UNEQUAL, "!=");

      CASE(COALESCE, "??");

      CASE(INLINE_IF, "?");
      CASE(INLINE_ELSE, ":");

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

      CASE(TRY_CATCH, "TRY_CATCH");
      CASE(ASM, "ASM");

      CASE(BREAK, "BREAK");
      CASE(CONTINUE, "CONTINUE");

      CASE(THROW, "THROW");
      CASE(ASSERT, "ASSERT");
      CASE(DEBUG_ASSERT, "DEBUG_ASSERT");
      CASE(STATIC_ASSERT, "STATIC_ASSERT");
      CASE(RETURN, "RETURN");

      CASE(ALIAS, "ALIAS");


      CASE(DECL, "DECLARATION");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const OpProps props) {
   using enum OpProps;
   switch (props) {
      CASE(NULL, "null");

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

constexpr const char* clef::toString(const KeywordID kw) {
   using enum KeywordID;
   switch (kw) {
      CASE(_NOT_A_KEYWORD, "not_a_keyword");


      CASE(VOID, "VOID");
      CASE(AUTO, "AUTO");


      CASE(CHAR, "CHAR");
      CASE(CHAR_UTF_8, "CHAR_UTF_8");
      CASE(CHAR_UTF_16, "CHAR_UTF_16");
      CASE(CHAR_UTF_32, "CHAR_UTF_32");


      CASE(BOOL, "BOOL");
      CASE(UBYTE, "UBYTE");
      CASE(USHORT, "USHORT");
      CASE(UINT, "UINT");
      CASE(ULONG, "ULONG");
      CASE(UINT_PTR, "UINT_PTR");
      CASE(UWORD, "UWORD");

      CASE(SIGN_T, "SIGN_T");
      CASE(SBYTE, "SBYTE");
      CASE(SSHORT, "SSHORT");
      CASE(SINT, "SINT");
      CASE(SLONG, "SLONG");
      CASE(SINT_PTR, "SINT_PTR");
      CASE(SWORD, "SWORD");


      CASE(UINT_8, "UINT_8");
      CASE(UINT_16, "UINT_16");
      CASE(UINT_32, "UINT_32");
      CASE(UINT_64, "UINT_64");
      CASE(UINT_128, "UINT_128");
      CASE(UINT_256, "UINT_256");

      CASE(SINT_8, "SINT_8");
      CASE(SINT_16, "SINT_16");
      CASE(SINT_32, "SINT_32");
      CASE(SINT_64, "SINT_64");
      CASE(SINT_128, "SINT_128");
      CASE(SINT_256, "SINT_256");

      CASE(FLOAT, "FLOAT");

      CASE(FLOAT_16, "FLOAT_16");
      CASE(FLOAT_32, "FLOAT_32");
      CASE(FLOAT_64, "FLOAT_64");
      CASE(FLOAT_128, "FLOAT_128");
      CASE(FLOAT_256, "FLOAT_256");







      CASE(CLASS, "CLASS");
      CASE(STRUCT, "STRUCT");
      CASE(INTERFACE, "INTERFACE");
      CASE(UNION, "UNION");
      CASE(ENUM, "ENUM");
      CASE(ENUM_UNION, "ENUM_UNION");
      CASE(MASK, "MASK");
      CASE(NAMESPACE, "NAMESPACE");
      CASE(FUNC, "FUNC");

      CASE(CONST, "CONST");
      CASE(CONSTEXPR, "CONSTEXPR");
      CASE(IMMEDIATE, "IMMEDIATE");
      CASE(VIEW, "VIEW");
      CASE(NON_OWNING, "NON_OWNING");
      CASE(MUTABLE, "MUTABLE");
      CASE(VOLATILE, "VOLATILE");
      CASE(ATOMIC, "ATOMIC");
      CASE(EXPLICIT, "EXPLICIT");

      CASE(CAST, "CAST");
      CASE(UP_CAST, "UP_CAST");
      CASE(DYN_CAST, "DYN_CAST");
      CASE(BIT_CAST, "BIT_CAST");
      CASE(CONST_CAST, "CONST_CAST");

      CASE(TYPEOF, "TYPEOF");
      CASE(SIZEOF, "SIZEOF");
      CASE(ALIGNAS, "ALIGNAS");
      CASE(ALIGNOF, "ALIGNOF");


      CASE(GOTO, "GOTO");
      CASE(IF, "IF");
      CASE(ELSE, "ELSE");
      CASE(FOR, "FOR");
      CASE(FOREACH, "FOREACH");
      CASE(WHILE, "WHILE");
      CASE(DO, "DO");
      CASE(BREAK, "BREAK");
      CASE(CONTINUE, "CONTINUE");
      CASE(SWITCH, "SWITCH");
      CASE(MATCH, "MATCH");
      CASE(CASE, "CASE");
      CASE(DEFAULT, "DEFAULT");
      CASE(TRY, "TRY");
      CASE(CATCH, "CATCH");
      CASE(THROW, "THROW");




      CASE(THIS, "THIS");
      CASE(SELF, "SELF");

      CASE(ASSERT, "ASSERT");
      CASE(STATIC_ASSERT, "STATIC_ASSERT");
      CASE(DEBUG_ASSERT, "DEBUG_ASSERT");
      CASE(SAFE_MODE_ASSERT, "SAFE_MODE_ASSERT");

      CASE(TRUE, "TRUE");
      CASE(FALSE, "FALSE");

      CASE(NULLPTR, "NULL");

      CASE(RETURN, "RETURN");

      CASE(LET, "LET");
      CASE(ALIAS, "ALIAS");

      CASE(ASM, "ASM");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const BlockType type) {
   using enum BlockType;
   switch (type) {
      CASE(NONE, "none");

      CASE(CALL, "CALL");
      CASE(SUBSCRIPT, "SUBSCRIPT");
      CASE(INIT_LIST, "INITIALIZER_LIST");
      CASE(SPECIALIZER, "SPECIALIZER");

      CASE(QUOTES_CHAR, "CHARACTER_LITERAL");
      CASE(QUOTES_STR, "STRING_LITERAL");
      CASE(QUOTES_INTERP, "INTERPOLATED_STRING_LITERAL");

      CASE(TERNARY, "TERNARY_STATEMENT");

      CASE(COMMENT_BLOCK, "BLOCK_COMMENT");
      CASE(COMMENT_LINE, "LINE_COMMENT");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const BlockDelimRole role) {
   using enum BlockDelimRole;
   switch (role) {
      CASE(NULL, "null");
      CASE(OPEN, "OPEN");
      CASE(CLOSE, "CLOSE");
      CASE(BOTH, "BOTH");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const PtxtType type) {
   using enum PtxtType;
   switch (type) {
      CASE(NULL, "NULL");

      CASE(CHAR, "CHAR");
      CASE(WCHAR, "WCHAR");
      CASE(CHAR8, "CHAR8");
      CASE(CHAR16, "CHAR16");
      CASE(CHAR32, "CHAR32");

      CASE(STR, "STR");
      CASE(WSTR, "WSTR");
      CASE(STR8, "STR8");
      CASE(STR16, "STR16");
      CASE(STR32, "STR32");

      CASE(UNPROCESSED_STR, "UNPROCESSED_STR");
      CASE(UNPROCESSED_WSTR, "UNPROCESSED_WSTR");
      CASE(UNPROCESSED_STR8, "UNPROCESSED_STR8");
      CASE(UNPROCESSED_STR16, "UNPROCESSED_STR16");
      CASE(UNPROCESSED_STR32, "UNPROCESSED_STR32");

      case __STR_TYPE: UNREACHABLE;
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const LitType type) {
   using enum LitType;
   switch (type) {
      CASE(NONE, "none");

      CASE(POINTER, "POINTER");

      CASE(UINT, "UNSIGNED_INT");
      CASE(SINT, "SIGNED_INT");
      CASE(FLOAT, "FLOATING_POINT");

      CASE(CHAR, "CHARACTER");
      CASE(STRING, "STRING");
      CASE(INTERP_STR, "INTERPOLATED_STRING");
      CASE(FORMAT, "FORMAT");
      CASE(REGEX, "REGEX");

      CASE(TYPEID, "TYPEID");
   }
   UNREACHABLE;
}

constexpr const char* clef::toString(const FundTypeID type) {
   using enum FundTypeID;
   switch (type) {
      CASE(NULL, "null");
      CASE(FUNCTION_SIGNATURE, "FUNCTION_SIGNATURE");

      CASE(VOID, "VOID");
      CASE(AUTO, "AUTO");

      CASE(CHAR, "CHAR");
      CASE(CHAR8, "CHAR8");
      CASE(CHAR16, "CHAR16");
      CASE(CHAR32, "CHAR32");

      CASE(BOOL, "BOOL");
      CASE(UINT8, "UINT8");
      CASE(UINT16, "UINT16");
      CASE(UINT32, "UINT32");
      CASE(UINT64, "UINT64");
      CASE(UINT128, "UINT128");
      CASE(UINT256, "UINT256");

      CASE(SIGN_T, "SIGN_T");
      CASE(SINT8, "SINT8");
      CASE(SINT16, "SINT16");
      CASE(SINT32, "SINT32");
      CASE(SINT64, "SINT64");
      CASE(SINT128, "SINT128");
      CASE(SINT256, "SINT256");

      CASE(FLOAT16, "FLOAT16");
      CASE(FLOAT32, "FLOAT32");
      CASE(FLOAT64, "FLOAT64");
      CASE(FLOAT128, "FLOAT128");
      CASE(FLOAT256, "FLOAT256");
      

      CASE(ENUM, "ENUM");
      CASE(MASK, "MASK");
      CASE(UNION, "UNION");
      CASE(NAMESPACE, "NAMESPACE");
      CASE(INTERFACE, "INTERFACE");
      CASE(STRUCT, "STRUCT");
      CASE(CLASS, "CLASS");
   }
   UNREACHABLE;
}

#undef CASE

#pragma endregion inlinesrc

#endif //PRETTY_PRINT_HPP