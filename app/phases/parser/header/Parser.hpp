#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SourceTokens.hpp"
#include "SyntaxTree.hpp"

#include "OperatorData.hpp"
#include "KeywordData.hpp"

#include "alloc.hpp"

class clef::Parser {
   private:
      //data members
      SyntaxTree tree;
      const SourceTokens& src;
      const Token* tokIt;
      const Token* endtok;

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

      //parsing functions
      index<Expr> parseExpr(index<astNode> initOperand = 0);
      index<Expr> parseExprNoPrimaryComma(index<astNode> initOperand = 0); //expression that cannot have the comma operator as its principal connective
      index<Stmt> parseStmt();
      index<Stmt> parsePreprocStmt();
      index<Scope> parseProcedure();
      index<ArgList> parseArgList(const BlockType closer);
      index<ParamList> parseParamList(const BlockType closer);

      index<Expr> parseCast(const KeywordID);

      void skipBlockComment();
      void skipLineComment();

      TypeQualMask parseQuals(const TypeQualMask illegalQuals = {});

      index<Identifier> tryParseIdentifier(index<Identifier> scopeName = 0);
      index<Identifier> parseIdentifier(index<Identifier> scopeName = 0);
      index<Decl> parseDecl(index<Type> type, index<Identifier> scopeName = 0);
      index<Decl> parseDecl(index<Identifier> scopeName = 0);
      index<Function> parseFuncDecl(index<Identifier> scopeName = 0);
      index<Stmt> parseForwardDecl(index<Identifier> scopeName = 0);
      index<Stmt> parseDeclStmt(index<Identifier> scopeName = 0);
      index<Type> parseTypename(index<Identifier> scopeName = 0);
      index<Variable> parseVariable(index<Identifier> scopeName = 0);
      mcsl::pair<index<Variable>,index<Decl>> parseVarDecl(index<Identifier> scopeName = 0);

      index<ForLoop> parseForLoop();
      index<ForeachLoop> parseForeachLoop();
      index<WhileLoop> parseWhileLoop();
      index<DoWhileLoop> parseDoWhileLoop();

      index<If> parseIf(); //parse if statement (including following else/else if blocks)
      index<Switch> parseSwitch();
      index<Match> parseMatch();

      index<TryCatch> parseTryCatch();

      index<Function> parseFunction();
      index<Asm> parseASM();
      
      index<Class> parseClass();
      index<Struct> parseStruct();
      index<Interface> parseInterface();
      index<Union> parseUnion();
      index<Enum> parseEnum();
      index<Mask> parseMask();
      index<Namespace> parseNamespace();

      //error logging
      void logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args);
      
      //constructors
      Parser(const SourceTokens& s):tree{},src{s},tokIt{s.begin()},endtok{s.end()},_errno{} {}
      
      static constexpr const mcsl::raw_str _nameof = "Parser";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      //parse tokenized source code
      static SyntaxTree parse(const SourceTokens& src);
};


#pragma region src
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
void clef::Parser::logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args) {
   _errno = code;
   clef::throwError(code, formatStr, std::forward<decltype(args)>(args)...);
}
#pragma GCC diagnostic pop
#pragma endregion src

#endif //PARSER_HPP