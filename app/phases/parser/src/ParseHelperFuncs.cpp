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
   index<Scope> scope = tree.make<Scope>(&tree.allocBuf<index<Stmt>>());

   while (tokIt < endtok) {
      if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      index<Stmt> tmp = parseStmt();
      tree[scope].push_back(tmp);
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}


clef::index<clef::Type> clef::Parser::parseTypename(index<Identifier> scopeName) {
   index<astNode> name = +parseIdentifier(scopeName);
   tree[name].upCast(NodeType::TYPE);
   while (tokIt->type() == TokenType::OP && +(tokIt->op().props() & OpProps::TYPE_MOD)) {
      //!TODO: actually do something with the type modifiers
      ++tokIt;
   }
   return +name;
}

mcsl::pair<clef::index<clef::Stmt>, clef::index<clef::Variable>> clef::Parser::parseLetStmt(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]]; //handle functions separately
      index<Function> funcptr = parseFunction();
      index<Variable> funcvar = tree.make<Variable>(tree[funcptr].signature(), tree[funcptr]); //!TODO: figure out how to represent function bodies as expressions?
      return {tree.make<Stmt, Decl>(tree[funcptr].signature(), funcptr), funcvar};
   }
   
   //declaration
   index<Variable> var = parseParam(scopeName);
   //definition (optional)
   index<Expr> val;
   if (tryConsumeOperator(OpID::ASSIGN)) {
      val = parseExpr();
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::INIT_LIST);
      val = tree.make<Expr>(OpID::LIST_INVOKE, tree[var].type(), args);
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::CALL);
      val = tree.make<Expr>(OpID::CALL_INVOKE, tree[var].type(), args);
   } else {
      val = 0;
   }
   tree[var].val() = val;

   consumeEOS("LET statement must end with EOS token");
   return {tree.make<Stmt, Decl>(tree[var].type(), var), var};
}

clef::index<clef::Variable> clef::Parser::parseParam(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]];
      index<Identifier> name = parseIdentifier(scopeName);
      
      consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
      index<ParamList> params = parseParamList(BlockType::CALL);
      
      consumeOperator(OpID::ARROW, "FUNC without trailing return type");
      // TypeQualMask returnTypeQuals = parseQuals();
      index<Type> returnType = parseTypename();

      index<FuncSig> sig = tree.make<FuncSig>(returnType, params);
      return tree.remake<Variable>(name, sig, tree[name]);
   }

   index<Type> typeName = parseTypename(scopeName);
   index<Identifier> varName = parseIdentifier(scopeName);
   return tree.remake<Variable>(varName, typeName, tree[varName]);
}
//!NOTE: does not support inline function definitions
clef::index<clef::Variable> clef::Parser::parseDefaultableParam(index<Identifier> scopeName) {
   index<Variable> var = parseParam(scopeName);
   if (tryConsumeOperator(OpID::ASSIGN)) {
      index<Expr> val = parseExpr();
      tree[var].val() = val;
   }
   return var;
}

clef::index<clef::ArgList> clef::Parser::parseArgList(const BlockType closer) {
   index<ArgList> args = tree.make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      auto tmp = parseExpr();
      tree[args].push_back(tmp);
      if (tryConsumeOperator(OpID::COMMA)) {
         continue;
      }
      consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
      break;
   } while (true);
   return args;
}
clef::index<clef::ParamList> clef::Parser::parseParamList(const BlockType closer) {
   index<ParamList> args = tree.make<ParamList>(&tree.allocBuf<index<Variable>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   do {
      index<Variable> tmp = parseDefaultableParam();
      tree[args].push_back(tmp);
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