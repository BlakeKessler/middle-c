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
      const Token* tokIt;
      const Token* endtok;
      const SourceTokens src;

      ErrCode _errno;

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
      Expr* parseExpr(astNode* initOperand = {});
      Expr* parseExprNoPrimaryComma(astNode* initOperand = {}); //expression that cannot have the comma operator as its principal connective
      Stmt* parseStmt();
      Stmt* parsePreprocStmt();
      Scope* parseProcedure();
      ArgList* parseArgList(const BlockType closer);
      ParamList* parseParamList(const BlockType closer);

      Expr* parseCast(const KeywordID);

      void skipBlockComment();
      void skipLineComment();

      TypeQualMask parseQuals(const TypeQualMask illegalQuals = {});

      Identifier* tryParseIdentifier(Identifier* scopeName = {});
      Identifier* parseIdentifier(Identifier* scopeName = {});
      Decl* parseDecl(Type* type, Identifier* scopeName = {});
      Decl* parseDecl(Identifier* scopeName = {});
      Function* parseFuncDecl(Identifier* scopeName = {});
      Stmt* parseForwardDecl(Identifier* scopeName = {});
      Stmt* parseDeclStmt(Identifier* scopeName = {});
      Type* parseTypename(Identifier* scopeName = {});
      Variable* parseVariable(Identifier* scopeName = {});
      mcsl::pair<Variable*,Decl*> parseVarDecl(Identifier* scopeName = {});

      Loop* parseForLoop();
      Loop* parseForeachLoop();
      Loop* parseWhileLoop();
      Loop* parseDoWhileLoop();

      If* parseIf(); //parse if statement (including following else/else if blocks)
      Switch* parseSwitch();
      Match* parseMatch();

      TryCatch* parseTryCatch();

      Function* parseFunction();
      // Asm* parseASM();
      
      Class* parseClass();
      Struct* parseStruct();
      Interface* parseInterface();
      Union* parseUnion();
      Enum* parseEnum();
      Mask* parseMask();
      Namespace* parseNamespace();

      //error logging
      void logError [[noreturn]] (const clef::ErrCode code, const char* formatStr, auto&&... args);
      
      
      
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