#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "Lexer.hpp"

#include "OperatorData.hpp"
#include "KeywordData.hpp"

#include "alloc.hpp"

class clef::Parser {
   private:
      //data members
      SyntaxTree& tree;
      Lexer& src;
      Token currTok;

      index<Identifier> scopeName;
      SymbolNode* currScope;
      #define SAVE_SCOPE index<Identifier> __prevScopeName = scopeName; SymbolNode* __prevScope = currScope
      #define PUSH_SCOPE2(name, symbol) SAVE_SCOPE; scopeName = name; currScope = symbol
      #define PUSH_SCOPE PUSH_SCOPE2(name, symbol)
      #define POP_SCOPE scopeName = __prevScopeName; currScope = __prevScope

      ErrCode _errno;

      //utils
      template<typename T> T& operator*(index<T> i) { return tree + i; } //should be a dereferencing operator

      //helper functions

      /*inline*/ bool consumeKeyword(const KeywordID, const char* errStr);
      /*inline*/ bool consumeOperator(const OpID, const char* errStr);
      /*inline*/ bool consumeBlockDelim(const BlockType, const BlockDelimRole, const char* errStr);
      /*inline*/ bool consumeEOS(const char* errStr);

      /*inline*/ bool tryConsumeKeyword(const KeywordID);
      /*inline*/ bool tryConsumeOperator(const OpID);
      /*inline*/ bool tryConsumeBlockDelim(const BlockType, const BlockDelimRole);
      /*inline*/ bool tryConsumeEOS();

      index<Expr> toExpr(index<astNode>);
      
      index<Stmt> makeStmt(index<Expr>);

      //parsing functions
      index<Expr> parseExpr(index<astNode> initOperand = 0);
      index<Expr> parseExprNoPrimaryComma(index<astNode> initOperand = 0); //expression that cannot have the comma operator as its principal connective
      index<Stmt> parseStmtImpl();
      index<Stmt> parseStmt();
      index<Stmt> parsePreprocStmt();
      index<Scope> parseProcedure();
      index<ArgList> parseArgList(const BlockType closer, bool isDecl);
      index<ArgList> parseSpecList(index<Identifier> target, bool isDecl);
      index<ArgList> parseSpecList(index<RawIdentifier> target);

      mcsl::str_slice parseStrLit();

      index<Expr> parseAttr(index<Expr> prevAttrs = 0);
      index<Expr> parseAttrs();
      index<Expr> tryParseAttrs();

      index<Stmt> addAttrs(index<Stmt> stmt, index<Expr> attrs);

      index<Expr> parseCast(const KeywordID);

      void skipBlockComment();
      void skipLineComment();

      QualMask parseQuals();
      index<RawIdentifier> tryParseIdentifier();
      index<RawIdentifier> parseIdentifier();
      index<Identifier> tryParseSymbol(SymbolType symbolType, SymbolNode* type, bool isDecl);
      index<Identifier> parseSymbol(SymbolType symbolType, SymbolNode* type, bool isDecl);
      index<Identifier> parseTypename(SymbolType symbolType, bool isDecl);
      index<Decl> parseDecl(index<Expr> attrs);
      index<Decl> parseParam(index<Expr> attrs);
      index<Decl> parseDefaultableParam(index<Expr> attrs);

      index<ForLoop> parseForLoop();
      index<ForeachLoop> parseForeachLoop();
      index<WhileLoop> parseWhileLoop();
      index<DoWhileLoop> parseDoWhileLoop();

      index<If> parseIf(); //parse if statement (including following else/else if blocks)
      index<Switch> parseSwitch();
      index<Match> parseMatch();

      index<FuncDef> parseFunction(index<Expr> attrs, bool isMethod); //function, signature
      index<MacroDef> parseMacro(index<Expr> attrs); //macro, signature
      mcsl::tuple<index<void>, mcsl::dyn_arr<index<Expr>>*, index<Identifier>> parseFuncSig(SymbolNode* target, bool isMethod);
      index<Asm> parseASM(index<Expr> attrs);
      
   private:
      bool overrideCanBeBinary(OpID op) { return op == OpID::SUBSCRIPT_INVOKE; }
      index<TypeDecl> __parseObjTypeImpl(index<Expr> attrs, SymbolType symbolType, const mcsl::str_slice metatypeName);
   public:
      index<TypeDecl> parseClass(index<Expr> attrs) { return __parseObjTypeImpl(attrs, SymbolType::CLASS, FMT("class")); }
      index<TypeDecl> parseStruct(index<Expr> attrs) { return __parseObjTypeImpl(attrs, SymbolType::STRUCT, FMT("struct")); }
      index<TypeDecl> parseTrait(index<Expr> attrs);
      index<TypeDecl> parseUnion(index<Expr> attrs);
   private:
      index<TypeDecl> __parseEnumlikeImpl(index<Expr> attrs, SymbolType symbolType, const mcsl::str_slice metatypeName);
   public:
      index<TypeDecl> parseMask(index<Expr> attrs) { return __parseEnumlikeImpl(attrs, SymbolType::MASK, FMT("mask")); }
      index<TypeDecl> parseEnum(index<Expr> attrs) { return __parseEnumlikeImpl(attrs, SymbolType::ENUM, FMT("enum")); }
      index<TypeDecl> parseEnumUnion(index<Expr> attrs);
      index<TypeDecl> parseNamespace(index<Expr> attrs);

      //error logging
      void logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, const mcsl::Printable auto&... args) {
         src.logError(code, formatStr, args...);
      }
      template<typename T> T unwrap(res<T> r) {
         if (r.is_ok()) { return r.ok(); }
         else { logError(r.err(), "failed unwrap"); }
      }
      template<typename T> T unwrap(res<T> r, ErrHandler onerr) {
         if (r.is_ok()) { return r.ok(); }
         else { onerr(r.err()); UNREACHABLE; }
      }
      
      //get next token
      void getNextToken() { currTok = src.nextToken(); }

      //make an AST node
      template<astNode_ptr_t asT, astNode_ptr_t T, typename... Argv_t> asT make(Argv_t... argv) requires requires { tree.make<asT, T>(std::forward<Argv_t>(argv)...); };
      template<astNode_t asT, astNode_t T = asT, typename... Argv_t> index<asT> make(Argv_t... argv) requires requires { tree.make<asT, T>(std::forward<Argv_t>(argv)...); } { index<asT> index = tree.nodeCount(); make<asT*,T*>(std::forward<Argv_t>(argv)...); return index; }

      template<astNode_ptr_t newT, astNode_t oldT, typename... Argv_t> newT remake(index<oldT> i, Argv_t... argv) requires requires { tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...); };
      template<astNode_t newT, astNode_t oldT, typename... Argv_t> index<newT> remake(index<oldT> i, Argv_t... argv) requires requires { tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...); } { remake<newT*>(i, std::forward<Argv_t>(argv)...); return +i; }

      template<astNode_ptr_t asT, astNode_ptr_t T, typename... Argv_t> asT make(ErrHandler onerr, Argv_t... argv) requires requires { tree.make<asT, T>(std::forward<Argv_t>(argv)...); };
      template<astNode_t asT, astNode_t T = asT, typename... Argv_t> index<asT> make(ErrHandler onerr, Argv_t... argv) requires requires { tree.make<asT, T>(std::forward<Argv_t>(argv)...); } { index<asT> index = tree.nodeCount(); make<asT*,T*>(onerr, std::forward<Argv_t>(argv)...); return index; }

      template<astNode_ptr_t newT, astNode_t oldT, typename... Argv_t> newT remake(ErrHandler onerr, index<oldT> i, Argv_t... argv) requires requires { tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...); };
      template<astNode_t newT, astNode_t oldT, typename... Argv_t> index<newT> remake(ErrHandler onerr, index<oldT> i, Argv_t... argv) requires requires { tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...); } { remake<newT*>(onerr, i, std::forward<Argv_t>(argv)...); return +i; }

      index<Expr> makeExpr(const OpID op, index<astNode> lhs) {
         index<Expr> expr = tree.makeExpr(op, lhs);
         res<void> r = tree.updateEvalType(expr, currScope);
         if (r.is_err()) {
            logError(r.err(), "error calculating type of expression");
         }
         return expr;
      }
      index<Expr> makeExpr(const OpID op, index<astNode> lhs, index<astNode> rhs) {
         index<Expr> expr = tree.makeExpr(op, lhs, rhs);
         res<void> r = tree.updateEvalType(expr, currScope);
         if (r.is_err()) {
            logError(r.err(), "error calculating type of expression");
         }
         return expr;
      }
      index<Identifier> toIden(index<RawIdentifier>);
      inline index<Identifier> tryToIden(index<astNode> i) {
         if (i && tree[i].nodeType() == NodeType::RAW_IDEN) {
            return toIden(+i);
         }
         return 0;
      }

      //constructors
      Parser(Lexer& s, SyntaxTree& t):tree{t},src{s},currTok{src.nextToken()},scopeName{0},currScope{tree.globalScope()},_errno{} {}
   public:
      //parse tokenized source code
      static void parse(const mcsl::str_slice filePath, SyntaxTree& tree);
      static void parse(Lexer& src, SyntaxTree& tree);
};

#pragma region inlinesrc

template<clef::astNode_ptr_t asT, clef::astNode_ptr_t T = asT, typename... Argv_t>
asT clef::Parser::make(Argv_t... argv)
requires requires {
   tree.make<asT, T>(std::forward<Argv_t>(argv)...);
} {
   uint index = tree.nodeCount();
   asT tmp = tree.make<asT, T>(std::forward<Argv_t>(argv)...);
   if constexpr (mcsl::is_t<mcsl::remove_ptr<T>, Expr>) {
      res<void> r = tree.updateEvalType(index, currScope);
      if (r.is_err()) {
         logError(r.err(), "error calculating type of expression");
      }
   }
   return tmp;
}

template<clef::astNode_ptr_t newT, clef::astNode_t oldT, typename... Argv_t>
newT clef::Parser::remake(index<oldT> i, Argv_t... argv)
requires requires {
   tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...);
} {
   assume(i);
   newT tmp = tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...);
   if constexpr (mcsl::is_t<mcsl::remove_ptr<newT>, Expr>) {
      res<void> r = tree.updateEvalType(i);
      if (r.is_err()) {
         logError(r.err(), "error calculating type of expression");
      }
   }
   return tmp;
}

template<clef::astNode_ptr_t asT, clef::astNode_ptr_t T = asT, typename... Argv_t>
asT clef::Parser::make(ErrHandler onerr, Argv_t... argv)
requires requires {
   tree.make<asT, T>(std::forward<Argv_t>(argv)...);
} {
   uint index = tree.nodeCount();
   asT tmp = tree.make<asT, T>(std::forward<Argv_t>(argv)...);
   if constexpr (mcsl::is_t<mcsl::remove_ptr<T>, Expr>) {
      res<void> r = tree.updateEvalType(index, currScope);
      if (r.is_err()) {
         onerr(r.err());
         UNREACHABLE;
      }
   }
   return tmp;
}

template<clef::astNode_ptr_t newT, clef::astNode_t oldT, typename... Argv_t>
newT clef::Parser::remake(ErrHandler onerr, index<oldT> i, Argv_t... argv)
requires requires {
   tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...);
} {
   assume(i);
   newT tmp = tree.remake<newT, oldT>(i, std::forward<Argv_t>(argv)...);
   if constexpr (mcsl::is_t<mcsl::remove_ptr<newT>, Expr>) {
      res<void> r = tree.updateEvalType(i);
      if (r.is_err()) {
         onerr(r.err());
         UNREACHABLE;
      }
   }
   return tmp;
}

#pragma endregion inlinesrc

#endif //PARSER_HPP