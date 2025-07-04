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

      mcsl::str_slice parseStrLit();

      index<Expr> parseAttr(index<Expr> prevAttrs = 0);
      index<Expr> parseAttrs();
      index<Expr> tryParseAttrs();

      index<Stmt> addAttrs(index<Stmt> stmt, index<Expr> attrs);

      index<Expr> parseCast(const KeywordID);

      void skipBlockComment();
      void skipLineComment();

      QualMask parseQuals();
      index<Identifier> tryParseIdentifier(SymbolType symbolType, SymbolNode* type, bool isDecl);
      index<Identifier> parseIdentifier(SymbolType symbolType, SymbolNode* type, bool isDecl);
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

      index<TryCatch> parseTryCatch();

      index<FuncDef> parseFunction(index<Expr> attrs); //function, signature
      index<MacroDef> parseMacro(index<Expr> attrs); //macro, signature
      mcsl::tuple<index<void>, mcsl::dyn_arr<index<Expr>>*, index<Identifier>> parseFuncSig(SymbolNode* target);
      index<Asm> parseASM(index<Expr> attrs);
      
   private:
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
      void logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args);
      
      //get next token
      void getNextToken() { currTok = src.nextToken(); }

      //constructors
      Parser(Lexer& s, SyntaxTree& t):tree{t},src{s},currTok{src.nextToken()},scopeName{0},currScope{tree.globalScope()},_errno{} {}
   public:
      //parse tokenized source code
      static void parse(const mcsl::str_slice filePath, SyntaxTree& tree);
      static void parse(Lexer& src, SyntaxTree& tree);
};


#pragma region inlinesrc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
void clef::Parser::logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args) {
   _errno = code;
   // mcsl::write(mcsl::stderr, currTok);
   // mcsl::write(mcsl::stderr, tree);
   clef::throwError(code, src.lineNum(), src.currLine(), src.prevTokStr(), src.path(), mcsl::FMT(formatStr), std::forward<decltype(args)>(args)...);
}
#pragma GCC diagnostic pop
#pragma endregion inlinesrc

#endif //PARSER_HPP