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
   index<Scope> scope = tree.make<Scope>(tree.allocBuf<index<Stmt>>());

   while (tokIt < endtok) {
      if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      tree[scope].push_back(parseStmt());
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}



clef::index<clef::Type> clef::Parser::parseTypename(index<Identifier> scopeName) {
   index<astNode> name = +parseIdentifier(scopeName);
   tree[name].upCast(NodeType::TYPE);
   return +name;
}

clef::index<clef::Decl> clef::Parser::parseDecl(index<Type> type, index<Identifier> scopeName) {
   index<Identifier> name = parseIdentifier(scopeName);
   return tree.make<Decl>(type, name);
}
clef::index<clef::Decl> clef::Parser::parseDecl(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) {
      index<Function> func = parseFuncDecl(scopeName);
      return tree.make<Decl>(tree[func].signature(), func);
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
   return tree.make<Function>(sig, tree[name]);
}

clef::index<clef::Variable> clef::Parser::parseVariable(index<Identifier> scopeName) {
   auto [var, decl] = parseVarDecl(scopeName);
   if (tryConsumeEOS()) { //forward declaration
      return var;
   }
   else if (tryConsumeOperator(OpID::ASSIGN)) {
      tree[var].val() = parseExpr();
      return var;
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::INIT_LIST);
      index<Expr> invoke = tree.make<Expr>(OpID::LIST_INVOKE, tree[decl].type(), args);
      tree[var].val() = invoke;
      return var;
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::CALL);
      index<Expr> invoke = tree.make<Expr>(OpID::CALL_INVOKE, tree[decl].type(), args);
      tree[var].val() = invoke;
      return var;
   }
   else {
      logError(ErrCode::BAD_DECL, "bad variable definition");
   }
}

mcsl::pair<clef::index<clef::Variable>,clef::index<clef::Decl>> clef::Parser::parseVarDecl(index<Identifier> scopeName) {
   index<Decl> decl = parseDecl(scopeName);
   index<Variable> var = tree.remake<Variable>(tree[decl].name(), tree[decl].type(), tree[tree[decl].name()]);
   return {var,decl};
}



clef::index<clef::ArgList> clef::Parser::parseArgList(const BlockType closer) {
   index<ArgList> args = tree.make<ArgList>(tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      tree[args].push_back(parseExpr());
      if (tryConsumeOperator(OpID::COMMA)) {
         continue;
      }
      consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
      break;
   } while (true);
   return args;
}
clef::index<clef::ParamList> clef::Parser::parseParamList(const BlockType closer) {
   index<ParamList> args = tree.make<ParamList>(tree.allocBuf<index<Variable>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      tree[args].push_back(parseVarDecl().first);
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



clef::index<clef::Expr> clef::Parser::toExpr(index<astNode> index) {
   astNode& node = tree[index];
   switch (node.nodeType()) {
      case Identifier::nodeType():
      case Variable::nodeType():
      case Function::nodeType():
      case Type::nodeType():
      case VariadicParameter::nodeType():
      case FundamentalType::nodeType():
      case FunctionSignature::nodeType():
      case Enum::nodeType():
      case Mask::nodeType():
      case Union::nodeType():
      case Namespace::nodeType():
      case Interface::nodeType():
      case Struct::nodeType():
      case Class::nodeType():
      case GenericType::nodeType():
      case Literal::nodeType():
         return tree.makeExpr(OpID::NULL, index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case Statement::nodeType():
      case ForLoop::nodeType():
      case ForeachLoop::nodeType():
      case WhileLoop::nodeType():
      case DoWhileLoop::nodeType():
      case If::nodeType():
      case Switch::nodeType():
      case Match::nodeType():
      case TryCatch::nodeType():
      case Asm::nodeType():
         return +index;


      case Scope::nodeType():
      case ForLoopParams::nodeType():
      case ForeachLoopParams::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StatementSequence::nodeType():
      case ArgumentList::nodeType():
      case ParameterList::nodeType():
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

#endif //PARSER_HELPERS_CPP