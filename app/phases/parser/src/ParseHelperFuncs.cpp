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

   //pointers and references
   while (true) {
      QualMask ptrquals = parseQuals();
      FundTypeID id;
      if (tryConsumeOperator(OpID::DEREF)) { //pointer
         id = FundTypeID::PTR;
      } else if (tryConsumeOperator(OpID::REFERENCE)) { //reference
         id = FundTypeID::REF;
      } else { //neither -> break
         if (+ptrquals) { //trailing qualifiers
            logError(ErrCode::BAD_IDEN, "type qualifiers must precede the type name");
         }
         break;
      }
      name = +tree.remake<SpecList>((index<Type>)name, tree[(index<Type>)name]);
      name = +tree.make<FundType>(id, (index<SpecList>)name, ptrquals);
   }
   return +name;
}

clef::index<clef::Decl> clef::Parser::parseLetStmt(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]]; //handle functions separately
      index<Function> funcptr = parseFunction();
      return tree.make<Decl>(tree[funcptr].signature(), funcptr, toExpr(+funcptr));
   }
   
   //declaration
   index<Type> type = parseTypename(scopeName);
   index<Identifier> varName = parseIdentifier(scopeName);
   //definition (optional)
   index<Expr> val;
   if (tryConsumeOperator(OpID::ASSIGN)) {
      val = parseExpr();
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::INIT_LIST);
      val = tree.make<Expr>(OpID::LIST_INVOKE, type, args);
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::CALL);
      val = tree.make<Expr>(OpID::CALL_INVOKE, type, args);
   } else {
      val = 0;
   }

   consumeEOS("LET statement must end with EOS token");
   if (val) {
      return tree.make<Decl>(type, varName, val);
   }
   return tree.make<Decl>(type, varName);
}

clef::index<clef::Variable> clef::Parser::parseParam(index<Identifier> scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]];
      index<Identifier> name = parseIdentifier(scopeName);
      
      consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
      index<ParamList> params = parseParamList(BlockType::CALL);
      
      consumeOperator(OpID::ARROW, "FUNC without trailing return type");
      index<Type> returnType = parseTypename();

      index<FuncSig> sig = tree.make<FuncSig>(returnType, params);
      return tree.remake<Variable>(name, sig, tree[name]);
   }

   index<Type> typeName = parseTypename(scopeName);
   index<Identifier> varName = parseIdentifier(scopeName);
   return tree.remake<Variable>(varName, typeName, tree[varName]);
}
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
template<bool isDecl> clef::index<clef::SpecList> clef::Parser::parseSpecList(const BlockType closer) {
   //!TODO: use GenericType?
   if constexpr (isDecl) {
      index<ParamList> params = parseParamList(closer);
      return tree.remake<SpecList>(params, tree[params]);
   } else {
      index<ArgList> args = parseArgList(closer);
      return tree.remake<SpecList>(args, tree[args]);
   }
}
template clef::index<clef::SpecList> clef::Parser::parseSpecList<true>(const BlockType closer);
template clef::index<clef::SpecList> clef::Parser::parseSpecList<false>(const BlockType closer);


clef::index<clef::Stmt> clef::Parser::parsePreprocStmt() {
   TODO;
}
clef::index<clef::Expr> clef::Parser::parseCast(KeywordID castID) {
   debug_assert(isCast(castID));
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN, "must specify type of cast");
   index<SpecList> typeptr = parseSpecList(BlockType::SPECIALIZER);
   index<Identifier> castptr = tree.make<Identifier>(castID, typeptr);
   //!TODO: validate cast specializer
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "typecasting uses function call syntax");
   index<Expr> contents = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "typecasting uses function call syntax");
   return tree.makeExpr(OpID::CALL_INVOKE, +castptr, +contents);
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
      case SpecializerList::nodeType():
      case NodeType::MAKE_TYPE:
      case NodeType::OBJ_TYPE_SPEC:
      case NodeType::INTERFACE_SPEC:
      case NodeType::NAMESPACE_SPEC:
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

#endif //PARSER_HELPERS_CPP