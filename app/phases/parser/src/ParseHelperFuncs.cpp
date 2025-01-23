#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"

/*inline*/ bool clef::Parser::consumeKeyword(const KeywordID keywordID, const char* errStr) {
   Identifier* keyword = parseIdentifier();
   if (!keyword || keyword->keywordID() != keywordID) {
      logError(ErrCode::BAD_KEYWORD, errStr);
      return false;
   }
   return true;
}
/*inline*/ bool clef::Parser::tryConsumeKeyword(const KeywordID keywordID) {
   auto oldTokIt = tokIt;
   Identifier* keyword = parseIdentifier();
   if (keyword && keyword->keywordID() == keywordID) {
      return true;
   } else {
      tokIt = oldTokIt;
      return false;
   }
}

/*inline*/ bool clef::Parser::consumeOperator(const OpID id, const char* errStr) {
   if (tokIt->type() != TokenType::OP || tokIt->opID() != id) {
      logError(ErrCode::BAD_KEYWORD, errStr);
      return false;
   }
   ++tokIt;
   return true;
}
/*inline*/ bool clef::Parser::tryConsumeOperator(const OpID id) {
   if (tokIt->type() != TokenType::OP || tokIt->opID() != id) {
      return false;
   }
   ++tokIt;
   return true;
}

/*inline*/ bool clef::Parser::consumeEOS(const char* errStr) {
   if (tokIt->type() == TokenType::EOS) {
      ++tokIt;
      return true;
   }
   logError(ErrCode::MISSING_EOS, errStr);
   return false;
}
/*inline*/ bool clef::Parser::tryConsumeEOS() {
   if (tokIt->type() == TokenType::EOS) {
      ++tokIt;
      return true;
   }
   return false;
}



clef::Scope* clef::Parser::parseProcedure() {
   Scope* scope = (Scope*)tree.allocNode(NodeType::SCOPE);

   while (tokIt < endtok) {
      if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      scope->push_back(parseStmt());
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}



clef::Type* clef::Parser::parseTypename(Identifier* scopeName) {
   Type* name = (Type*)parseIdentifier(scopeName);
   ((astNode*)name)->upCast(NodeType::TYPE);
   return name;
}

clef::Decl* clef::Parser::parseDecl(Type* type, Identifier* scopeName) {
   Identifier* name = parseIdentifier(scopeName);
   return new (tree.allocNode(NodeType::DECL)) Decl{type, name};
}
clef::Decl* clef::Parser::parseDecl(Identifier* scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) {
      Function* func = parseFuncDecl(scopeName);
      return new (tree.allocNode(NodeType::DECL)) Decl{func->signature(), func};
   }

   Type* type = parseTypename(scopeName);

   Identifier* name = parseIdentifier(scopeName);
   return new (tree.allocNode(NodeType::DECL)) Decl{type, name};
}

clef::Function* clef::Parser::parseFuncDecl(Identifier* scopeName) {
   Identifier* name = parseIdentifier(scopeName);
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "function definition must include parameter list");
   ParamList* params = parseParamList(BlockType::CALL);
   consumeOperator(OpID::ARROW, "function definitions must provide a return type");
   Type* returnType = parseTypename(scopeName);

   FuncSig* sig = new (tree.allocNode(NodeType::FUNC_SIG)) FuncSig{returnType, params};
   return new (tree.allocNode(NodeType::FUNC)) Function{sig, nullptr, name};
}

clef::Variable* clef::Parser::parseVariable(Identifier* scopeName) {
   Decl* decl = parseDecl(scopeName);
   if (tryConsumeEOS()) { //forward declaration
      Variable* var = new (decl) Variable{decl->type(), decl->name()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeOperator(OpID::ASSIGN)) {
      Variable* var = new (decl) Variable{decl->type(), decl->name(), parseExpr()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::INIT_LIST);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::LIST_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::CALL);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::CALL_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   logError(ErrCode::BAD_DECL, "bad variable definition");
}

mcsl::pair<clef::Variable*,clef::Decl*> clef::Parser::parseVarDecl(Identifier* scopeName) {
   Decl* decl = parseDecl(scopeName);
   if (tryConsumeEOS()) {
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name()};
      return {var,decl};
   }
   if (tryConsumeOperator(OpID::ASSIGN)) {
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), parseExpr()};
      return {var,decl};
   }
   if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::INIT_LIST);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::LIST_INVOKE, decl->type(), args};
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), invoke};
      return {var,decl};
   }
   if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::CALL);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::CALL_INVOKE, decl->type(), args};
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), invoke};
      return {var,decl};
   }
   logError(ErrCode::BAD_DECL, "bad variable definition");
}


#endif //PARSER_HELPERS_CPP