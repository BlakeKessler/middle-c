#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SourceTokens.hpp"
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

      //!TODO: use these
      index<Identifier> currScopeIden;
      SymbolNode* currScopeSymbol;

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

      //parsing functions
      index<Expr> parseExpr(index<astNode> initOperand = 0);
      index<Expr> parseExprNoPrimaryComma(index<astNode> initOperand = 0); //expression that cannot have the comma operator as its principal connective
      index<Stmt> parseStmtImpl();
      index<Stmt> parseStmt();
      index<Stmt> parsePreprocStmt();
      index<Scope> parseProcedure();
      index<ArgList> parseArgList(const BlockType closer);
      index<ParamList> parseParamList(const BlockType closer);
      template<bool isDecl = false> index<SpecList> parseSpecList(const BlockType closer);
      index<SpecList> parseSpecList(const BlockType closer, bool isDecl) {
         if (isDecl) { return parseSpecList<true>(closer); }
         else { return parseSpecList<false>(closer); }
      }

      index<Expr> parseCast(const KeywordID);

      void skipBlockComment();
      void skipLineComment();

      QualMask parseQuals();
      index<Identifier> tryParseIdentifier(SymbolType symbolType, SymbolNode* type); //(type == nullptr) == isDecl
      index<Identifier> parseIdentifier(SymbolType symbolType, SymbolNode* type); //(type == nullptr) == isDecl
      index<Type> parseTypename(index<Identifier> scopeName = 0, bool isDecl);
      index<Decl> parseLetStmt(index<Identifier> scopeName = 0);
      index<Variable> parseParam(index<Identifier> scopeName = 0);
      index<Variable> parseDefaultableParam(index<Identifier> scopeName = 0);

      index<ForLoop> parseForLoop();
      index<ForeachLoop> parseForeachLoop();
      index<WhileLoop> parseWhileLoop();
      index<DoWhileLoop> parseDoWhileLoop();

      index<If> parseIf(); //parse if statement (including following else/else if blocks)
      index<Switch> parseSwitch();
      index<Match> parseMatch();

      index<TryCatch> parseTryCatch();

      index<Function> parseFunction();
      index<Macro> parseMacro();
      index<Asm> parseASM();
      
      index<TypeDecl> parseClass();
      index<TypeDecl> parseStruct();
      index<TypeDecl> parseInterface();
      index<TypeDecl> parseUnion();
      index<TypeDecl> parseEnum();
      index<TypeDecl> parseEnumUnion();
      index<TypeDecl> parseMask();
      index<TypeDecl> parseNamespace();

      //error logging
      void logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args);
      
      //get next token
      void getNextToken() { currTok = src.nextToken(); }

      //constructors
      Parser(Lexer& s, SyntaxTree& t):tree{t},src{s},currTok{src.nextToken()},_errno{} {}
   public:
      //parse tokenized source code
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