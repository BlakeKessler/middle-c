#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"

/*inline*/ bool clef::Parser::consumeKeyword(const KeywordID keywordID, const char* errStr) {
   if (tryConsumeKeyword(keywordID)) {
      return true;
   }
   logError(ErrCode::MISSING_KEYWORD, errStr);
}
/*inline*/ bool clef::Parser::tryConsumeKeyword(const KeywordID keywordID) {
   if (tokIt->type() != TokenType::KEYWORD || tokIt->keywordID() != keywordID) {
      return false;
   }
   ++tokIt;
   return true;
}

/*inline*/ bool clef::Parser::consumeOperator(const OpID id, const char* errStr) {
   if (tokIt->type() != TokenType::OP || tokIt->opID() != id) {
      logError(ErrCode::MISSING_OPERATOR, errStr);
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
   if (tryConsumeEOS()) {
      return true;
   }
   logError(ErrCode::MISSING_EOS, errStr);
}
/*inline*/ bool clef::Parser::tryConsumeEOS() {
   if (tokIt->type() == TokenType::EOS) {
      ++tokIt;
      return true;
   }
   return false;
}

/*inline*/ bool clef::Parser::consumeBlockDelim(BlockType type, BlockDelimRole role, const char* errStr) {
   if (tryConsumeBlockDelim(type, role)) {
      return true;
   }
   logError(ErrCode::BAD_BLOCK_DELIM, errStr);
}
/*inline*/ bool clef::Parser::tryConsumeBlockDelim(BlockType type, BlockDelimRole role) {
   if (tokIt->type() == TokenType::BLOCK_DELIM) {
      if (tokIt->blockType() == type && tokIt->blockDelimRole() == role) {
         ++tokIt;
         return true;
      }
   }
   return false;
}


clef::index<clef::Scope> clef::Parser::parseProcedure() {
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



clef::index<clef::Type> clef::Parser::parseTypename(index<Identifier> scopeName) {
   index<Type> name = (index<Type>)parseIdentifier(scopeName);
   tree[(index<astNode>)name].upCast(NodeType::TYPE);
   return name;
}

clef::index<clef::Decl> clef::Parser::parseDecl(Type* type, index<Identifier> scopeName) {
   index<Identifier> name = parseIdentifier(scopeName);
   return tree.make<Decl>(type, name);
}
clef::index<clef::Decl> clef::Parser::parseDecl(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) {
      Function* func = parseFuncDecl(scopeName);
      return new (tree.allocNode(NodeType::DECL)) Decl{func->signature(), func};
   }

   index<Type> type = parseTypename(scopeName);

   index<Identifier> name = parseIdentifier(scopeName);
   return tree.make<Decl>(type, name);
}

clef::index<clef::Function> clef::Parser::parseFuncDecl(index<Identifier> scopeName) {
   index<Identifier> name = parseIdentifier(scopeName);
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "function definition must include parameter list");
   index<ParamList> params = parseParamList(BlockType::CALL);
   consumeOperator(OpID::ARROW, "function definitions must provide a return type");
   index<Type> returnType = parseTypename(scopeName);

   index<FuncSig> sig = tree.make<FuncSig>(returnType, params);
   return tree.make<Function>(sig, name);
}

clef::index<clef::Variable> clef::Parser::parseVariable(index<Identifier> scopeName) {
   Decl* decl = parseDecl(scopeName);
   if (tryConsumeEOS()) { //forward declaration
      Variable* var = new (decl) Variable{decl->type(), decl->name()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   else if (tryConsumeOperator(OpID::ASSIGN)) {
      Variable* var = new (decl) Variable{decl->type(), decl->name(), parseExpr()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::INIT_LIST);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::LIST_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::CALL);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::CALL_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   else {
      logError(ErrCode::BAD_DECL, "bad variable definition");
   }
}

mcsl::pair<clef::index<clef::Variable>,clef::index<clef::Decl>> clef::Parser::parseVarDecl(index<Identifier> scopeName) {
   Decl* decl = parseDecl(scopeName);
   if (tryConsumeEOS()) {
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name()};
      return {var,decl};
   }
   else if (tryConsumeOperator(OpID::ASSIGN)) {
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), parseExpr()};
      return {var,decl};
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::INIT_LIST);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::LIST_INVOKE, decl->type(), args};
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), invoke};
      return {var,decl};
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::CALL);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::CALL_INVOKE, decl->type(), args};
      Variable* var = new (tree.allocNode(NodeType::VAR)) Variable{decl->type(), decl->name(), invoke};
      return {var,decl};
   }
   else {
      logError(ErrCode::BAD_DECL, "bad variable definition");
   }
}



clef::index<clef::ArgList> clef::Parser::parseArgList(const BlockType closer) {
   ArgList* args = new (tree.allocNode(NodeType::ARG_LIST)) ArgList{tree.allocBuf<Expr*>()};
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      args->push_back(parseExpr());
      if (tryConsumeOperator(OpID::COMMA)) {
         continue;
      }
      consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
      break;
   } while (true);
   return args;
}
clef::index<clef::ParamList> clef::Parser::parseParamList(const BlockType closer) {
   ParamList* args = new (tree.allocNode(NodeType::PARAM_LIST)) ParamList{tree.allocBuf<Variable*>()};
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      args->push_back(parseVariable());
      if (tryConsumeOperator(OpID::COMMA)) {
         continue;
      }
      consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
      break;
   } while (true);
   return args;
}



clef::TypeQualMask clef::Parser::parseQuals(const TypeQualMask illegalQuals) {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "type qualifiers are not yet supported");
}
clef::index<clef::Stmt> clef::Parser::parsePreprocStmt() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "preprocessor statements are not yet supported");
}
clef::index<clef::Expr> clef::Parser::parseCast(KeywordID castID) {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "typecasting is not yet supported");
}

#endif //PARSER_HELPERS_CPP