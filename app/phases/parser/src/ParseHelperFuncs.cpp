#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"
#include "pretty-print.hpp"

/*inline*/ bool clef::Parser::consumeKeyword(const KeywordID keywordID, const char* errStr) {
   if (tryConsumeKeyword(keywordID)) {
      return true;
   }
   logError(ErrCode::MISSING_KEYWORD, errStr);
}
/*inline*/ bool clef::Parser::tryConsumeKeyword(const KeywordID keywordID) {
   if (currTok.type() != TokenType::KEYWORD || currTok.keywordID() != keywordID) {
      return false;
   }
   getNextToken();
   return true;
}

/*inline*/ bool clef::Parser::consumeOperator(const OpID id, const char* errStr) {
   if (currTok.type() != TokenType::OP || currTok.opID() != id) {
      logError(ErrCode::MISSING_OPERATOR, errStr);
   }
   getNextToken();
   return true;
}
/*inline*/ bool clef::Parser::tryConsumeOperator(const OpID id) {
   if (currTok.type() != TokenType::OP || currTok.opID() != id) {
      return false;
   }
   getNextToken();
   return true;
}

/*inline*/ bool clef::Parser::consumeEOS(const char* errStr) {
   if (tryConsumeEOS()) {
      return true;
   }
   logError(ErrCode::MISSING_EOS, errStr);
}
/*inline*/ bool clef::Parser::tryConsumeEOS() {
   if (currTok.type() == TokenType::EOS) {
      getNextToken();
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
   if (currTok.type() == TokenType::BLOCK_DELIM) {
      if (currTok.blockType() == type && currTok.blockDelimRole() == role) {
         getNextToken();
         return true;
      }
   }
   return false;
}


clef::index<clef::Scope> clef::Parser::parseProcedure() {
   index<Scope> scope = tree.make<Scope>(&tree.allocBuf<index<Stmt>>());

   while (!src.done()) {
      if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      index<Stmt> tmp = parseStmt();
      tree[scope].push_back(tmp);
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}


clef::index<clef::Identifier> clef::Parser::parseTypename(SymbolType symbolType, bool isDecl) {
   if (isDecl) {
      TODO;
   }
   index<Identifier> name = parseIdentifier(symbolType, nullptr);
   Identifier& iden = tree[name];
   if (SymbolNode* symbol = iden.symbol(); !symbol || !isType(symbol->symbolType())) {
      logError(ErrCode::BAD_IDEN, "`%s` does not name a type", symbol && symbol->name().size() ? symbol->name() : FMT("(anonymous)"));
   }

   //pointers and references
   IndirTable::Entry entry;
   QualMask ptrquals;
   auto qualsAndMods = [&]() -> bool {
      new (&entry) IndirTable::Entry();
      ptrquals = parseQuals();
      if (entry.setQuals(ptrquals)) {
         logError(ErrCode::BAD_IDEN, "illegal qualifiers for typename");
      }
      if (tryConsumeOperator(OpID::RAW_PTR)) { //pointer
         entry._type = IndirTable::Entry::PTR;
         return true;
      } else if (tryConsumeOperator(OpID::REFERENCE)) { //reference
         entry._type = IndirTable::Entry::REF;
         return true;
      } else if (tryConsumeOperator(OpID::SLICE)) { //slice
         entry._type = IndirTable::Entry::SLICE;
         return true;
      } else if (TODO, false) { //array literal
         entry._type = IndirTable::Entry::ARR;
         TODO;
         return true;
      }
      //neither -> break
      if (+ptrquals) { //trailing qualifiers (error)
         logError(ErrCode::BAD_IDEN, "type qualifiers must precede the type name");
      }
      return false;
   };
   if (qualsAndMods()) {
      iden.setQualMask(ptrquals);
      TypeSpec* typeDef = tree.makeIndirType(name, iden.symbol()->type(), iden.quals(), entry);
      IndirTable& indirTable = typeDef->indirTable();
      while (qualsAndMods()) {
         if (+(ptrquals & QualMask::VIEW)) {
            indirTable.appendView(entry);
         } else {
            indirTable.append(entry);
         }
      }
   }
   return +name;
}

clef::index<clef::Decl> clef::Parser::parseDecl() {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]]; //handle functions separately
      TODO;
      // index<Function> funcptr = parseFunction();
      // return tree.make<Decl>(tree[funcptr].signature(), funcptr, toExpr(+funcptr));
   }
   
   //declaration
   index<Identifier> type = parseTypename(SymbolType::null, true);
   index<Identifier> varName = parseIdentifier(SymbolType::VAR, tree[type].symbol());
   //definition (optional)
   index<Expr> val;
   if (tryConsumeOperator(OpID::ASSIGN)) {
      val = parseExpr();
   }
   else if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::INIT_LIST, false);
      val = tree.make<Expr>(OpID::LIST_INVOKE, type, args);
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::CALL, false);
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

clef::index<clef::Decl> clef::Parser::parseParam() {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]];
      TODO;
      // index<Identifier> name = parseIdentifier(SymbolType::FUNC, nullptr);
      
      // consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
      // index<ArgList> params = parseArgList(BlockType::CALL, true);
      
      // consumeOperator(OpID::ARROW, "FUNC without trailing return type");
      // index<Identifier> returnType = parseTypename(SymbolType::null, true);

      // index<FuncSig> sig = tree.make<FuncSig>(returnType, params);
      // return tree.remake<Variable>(name, sig, tree[name]);
   }

   index<Identifier> typeName = parseTypename(SymbolType::null, true);
   index<Identifier> varName = parseIdentifier(SymbolType::VAR, tree[typeName].symbol());
   return tree.make<Decl>(typeName, varName);
}
clef::index<clef::Decl> clef::Parser::parseDefaultableParam() {
   index<Decl> var = parseParam();
   if (tryConsumeOperator(OpID::ASSIGN)) {
      index<Expr> val = parseExpr();
      tree[var].value() = val;
   }
   return var;
}

clef::index<clef::ArgList> clef::Parser::parseArgList(const BlockType closer, bool isDecl) {
   index<ArgList> args = tree.make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   if (isDecl) {
      do {
         auto tmp = parseDefaultableParam();
         tree[args].push_back(tmp);
         if (tryConsumeOperator(OpID::COMMA)) {
            continue;
         }
         consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
         break;
      } while (true);
   } else {
      do {
         auto tmp = parseExpr();
         tree[args].push_back(tmp);
         if (tryConsumeOperator(OpID::COMMA)) {
            continue;
         }
         consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
         break;
      } while (true);
   }
   return args;
}

clef::index<clef::Stmt> clef::Parser::parsePreprocStmt() {
   //deduce directive
   if (currTok.type() != TokenType::IDEN) {
      logError(ErrCode::BAD_PREPROC, "invalid directive");
   }
   OpID op;
   index<Identifier> name = 0;
   if (currTok.name() == FMT("import")) {
      getNextToken();
      op = OpID::PREPROC_IMPORT;
   }
   else if (currTok.name() == FMT("link")) {
      getNextToken();
      op = OpID::PREPROC_LINK;
   }
   else if (currTok.name() == FMT("embed")) {
      getNextToken();
      op = OpID::PREPROC_EMBED;
      SymbolNode* byteType = tree.globalScope()->get(toString(KeywordID::UBYTE));
      index<Identifier> byteSpan = tree.make<Identifier>(KeywordID::UBYTE);
      tree.makeIndirType(byteSpan, byteType->type(), QualMask::CONST, IndirTable::Entry(IndirTable::Entry::SLICE, true, false, false));
      name = parseIdentifier(SymbolType::VAR, tree[byteSpan].symbol());
   } else {
      logError(ErrCode::BAD_PREPROC, "unrecognized directive");
      op = OpID::NULL;
   }

   //get path //!TODO: standard library paths
   if (currTok.type() != TokenType::PTXT_SEG || !isString(currTok.ptxtType())) {
      logError(ErrCode::BAD_PREPROC, "invalid `%s` directive", toString(op));
   }
   const mcsl::str_slice path = currTok.unprocessedStrVal();
   getNextToken();
   index<Literal> pathLit = tree.make<Literal>(path);
   consumeEOS("missing EOS token");

   //create statement node
   index<Expr> stmt = tree.makeExpr(op, +name, +pathLit);
   tree[(index<astNode>)stmt].upCast(NodeType::STMT);
   return +stmt;
}
clef::index<clef::Expr> clef::Parser::parseCast(KeywordID castID) {
   debug_assert(isCast(castID));
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN, "must specify type of cast");
   index<ArgList> typeptr = parseArgList(BlockType::SPECIALIZER, false);
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
      case Literal::nodeType():
         return tree.makeExpr(OpID::NULL, index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case FunctionDefinition::nodeType():
      case MacroDefinition::nodeType():
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
      case NodeType::MAKE_TYPE:
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

#endif //PARSER_HELPERS_CPP