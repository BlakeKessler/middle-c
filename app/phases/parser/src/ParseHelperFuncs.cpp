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
   index<Identifier> name = parseIdentifier(symbolType, nullptr, isDecl);
   Identifier& iden = tree[name];
   {
      SymbolNode* symbol = iden.symbol();
      if (!symbol || isNonType(symbol->symbolType())) {
         logError(ErrCode::BAD_IDEN, "`%s` does not name a type", symbol && symbol->name().size() ? symbol->name() : FMT("(anonymous)"));
      }
      debug_assert(symbol);
      if (!symbol->type()) {
         symbol->setType(tree.registerType(symbol, TypeSpec::null, SymbolType::EXTERN_TYPE));
      }
      else if (SymbolNode*& canon = symbol->type()->canonName(); !canon) {
         canon = symbol;
      }
      debug_assert(symbol->type()->canonName());
   }

   //pointers and references
   IndirTable::Entry entry;
   QualMask ptrquals;
   const auto qualsAndMods = [&]() -> bool {
      bool isDone = false;
      new (&entry) IndirTable::Entry();
      if (tryConsumeOperator(OpID::RAW_PTR)) { //pointer
         entry._type = IndirTable::Entry::PTR;
         isDone = true;
      } else if (tryConsumeOperator(OpID::REFERENCE)) { //reference
         entry._type = IndirTable::Entry::REF;
         isDone = true;
      } else if (tryConsumeOperator(OpID::SLICE)) { //slice
         entry._type = IndirTable::Entry::SLICE;
         isDone = true;
      } else if (tryConsumeBlockDelim(BlockType::SUBSCRIPT, BlockDelimRole::OPEN)) { //array literal
         entry._type = IndirTable::Entry::ARR;
         index<Expr> arrSize = parseExprNoPrimaryComma();
         consumeBlockDelim(BlockType::SUBSCRIPT, BlockDelimRole::CLOSE, "array bounds must be a single integer constant expression");
         TODO;
         isDone = true;
      }
      ptrquals = parseQuals();
      if (entry.setQuals(ptrquals)) {
         logError(ErrCode::BAD_IDEN, "illegal qualifiers for typename");
      }
      return isDone;
   };
   if (qualsAndMods()) {
      QualMask targetQuals = iden.quals() | ptrquals;
      IndirTable indirTable{entry};
      while (qualsAndMods()) {
         if (+(ptrquals & QualMask::VIEW)) {
            indirTable.appendView(entry);
         } else {
            indirTable.append(entry);
         }
      }
      tree.makeIndirType(name, iden.symbol()->type(), targetQuals, std::move(indirTable));
      iden.setQualMask(ptrquals);
   }
   return +name;
}

clef::index<clef::Decl> clef::Parser::parseDecl(index<Expr> attrs) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]]; //handle functions separately
      index<FuncDef> funcDef = parseFunction(attrs);
      index<Expr> val;
      if (tree[funcDef].procedure()) {
         val = toExpr(+tree[funcDef].name());
      } else {
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExpr();
         } else {
            val = 0;
         }
      }
      return tree.make<Decl>(tree[funcDef].name(), tree[funcDef].name(), val);
   }
   
   //declaration
   index<Identifier> type = parseTypename(SymbolType::EXTERN_TYPE, true);
   index<Identifier> varName = parseIdentifier(SymbolType::VAR, tree[type].symbol(), true);
   SymbolNode* varSymbol = tree[varName].symbol();
   varSymbol->setSymbolType(SymbolType::VAR);
   varSymbol->setType(tree[type].symbol()->type());
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

clef::index<clef::Decl> clef::Parser::parseParam(index<Expr> attrs) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]];
      index<FuncDef> funcDef = parseFunction(attrs);
      if (tree[funcDef].procedure()) {
         logError(ErrCode::BAD_DECL, "cannot inline define parameters of type func");
      }
      return tree.make<Decl>(tree[funcDef].name(), tree[funcDef].name());
   }

   index<Identifier> typeName = parseTypename(SymbolType::EXTERN_TYPE, true);
   index<Identifier> varName = tryParseIdentifier(SymbolType::VAR, tree[typeName].symbol(), true);
   if (varName) {
      SymbolNode* varSymbol = tree[varName].symbol();
      varSymbol->setSymbolType(SymbolType::VAR);
      varSymbol->setType(tree[typeName].symbol()->type());
   }
   if (attrs) { TODO; }
   return tree.make<Decl>(typeName, varName);
}
clef::index<clef::Decl> clef::Parser::parseDefaultableParam(index<Expr> attrs) {
   index<Decl> var = parseParam(attrs);
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
         auto tmp = parseDefaultableParam(tryParseAttrs());
         tree[args].push_back(tmp);
         if (!tryConsumeOperator(OpID::COMMA)) {
            break;
         }
      } while (true);
   } else {
      do {
         auto tmp = parseExpr();
         tree[args].push_back(tmp);
         if (!tryConsumeOperator(OpID::COMMA)) {
            break;
         }
      } while (true);
   }
   consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
   return args;
}

clef::index<clef::ArgList> clef::Parser::parseSpecList(index<Identifier> target, bool isDecl) {
   index<ArgList> args = tree.make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE)) {
      return args;
   }
   if (isDecl) {
      do {
         if (tryConsumeKeyword(KeywordID::TYPE)) {
            index<Identifier> typeName = parseTypename(SymbolType::EXTERN_TYPE, true);
            index<TypeDecl> typeDecl;
            if (tryConsumeOperator(OpID::ASSIGN)) {
               index<Identifier> defaultType = parseTypename(SymbolType::EXTERN_TYPE, false);
               typeDecl = tree.make<TypeDecl>(typeName, defaultType);
            } else {
               typeDecl = tree.make<TypeDecl>(typeName);
            }
            tree[args].push_back(typeDecl);
         } else {
            index<Decl> param = parseDefaultableParam(tryParseAttrs());
            tree[args].push_back(param);
         }
         if (!tryConsumeOperator(OpID::COMMA)) {
            break;
         }
      } while (true);
   } else {
      do {
         index<Expr> arg = parseExpr();
         tree[args].push_back(arg);
         if (!tryConsumeOperator(OpID::COMMA)) {
            break;
         }
      } while (true);
   }
         consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
   tree[target].specializer() = args;
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
      SymbolNode* byteType = tree.getFundType(KeywordID::UBYTE);
      index<Identifier> byteSpan = tree.make<Identifier>(KeywordID::UBYTE, byteType);
      tree.makeIndirType(byteSpan, byteType->type(), QualMask::CONST, IndirTable(IndirTable::Entry(IndirTable::Entry::SLICE, true, false, false)));
      name = parseIdentifier(SymbolType::VAR, tree[byteSpan].symbol(), true);
   } else {
      logError(ErrCode::BAD_PREPROC, "unrecognized directive");
      op = OpID::NULL;
   }

   //get path //!TODO: standard library paths
   if (currTok.type() != TokenType::PTXT_SEG || !isString(currTok.ptxtType())) {
      logError(ErrCode::BAD_PREPROC, "invalid `%s` directive", toString(op));
   }
   const mcsl::str_slice path = parseStrLit();
   index<Literal> pathLit = tree.make<Literal>(path);
   consumeEOS("missing EOS token");

#if !PARALLEL_COMPILE_FILES
   if (op == OpID::PREPROC_IMPORT) {
      //!TODO: make this more robust
      mcsl::string fullPath = src.path();
      while (fullPath.size() && fullPath.back() != '/') {
         fullPath.pop_back();
      }
      fullPath += path;
      Lexer importedToks = clef::Lexer::fromFile(fullPath);
      Parser::parse(importedToks, tree);
      return 0;
   }
#endif

   //create statement node
   index<Expr> stmt = tree.makeExpr(op, +name, +pathLit);
   tree[(index<astNode>)stmt].upCast(NodeType::STMT);
   return +stmt;
}
clef::index<clef::Expr> clef::Parser::parseCast(KeywordID castID) {
   debug_assert(isCast(castID));
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN, "must specify type of cast");
   index<ArgList> typeptr = parseArgList(BlockType::SPECIALIZER, false);
   index<Identifier> castptr = tree.make<Identifier>(castID, nullptr, typeptr);
   //!TODO: validate cast specializer
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "typecasting uses function call syntax");
   index<Expr> contents = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "typecasting uses function call syntax");
   return tree.makeExpr(OpID::CALL_INVOKE, +castptr, +contents);
}

mcsl::str_slice clef::Parser::parseStrLit() {
   mcsl::string str;
   debug_assert(currTok.type() == TokenType::PTXT_SEG && isString(currTok.ptxtType()));
   do {
      switch (currTok.ptxtType()) {
         case PtxtType::STR: str += currTok.strVal();
         case PtxtType::WSTR: TODO;
         case PtxtType::STR8: TODO;
         case PtxtType::STR16: TODO;
         case PtxtType::STR32: TODO;
         case PtxtType::UNPROCESSED_STR: {
            mcsl::str_slice raw = currTok.unprocessedStrVal();
            str.reserve(str.size() + raw.size());
            for (uint i = 0; i < raw.size(); ++i) {
               if (raw[i] == '\\') {
                  ++i;
                  if (i >= raw.size()) {
                     TODO;
                  }
                  switch (raw[i]) {
                     // case 'a': str.push_back('\a'); break;
                     case 'b': str.push_back('\b'); break;
                     case 'f': str.push_back('\f'); break;
                     case 'n': str.push_back('\n'); break;
                     case 'r': str.push_back('\r'); break;
                     case 't': str.push_back('\t'); break;
                     case 'v': str.push_back('\v'); break;

                     default:
                        TODO; //!TODO: warning for invalid escape sequence
                        [[fallthrough]];
                     case '\"':
                     case '\'':
                        str.push_back(raw[i]);
                        break;
                  }
               } else {
                  str.push_back(raw[i]);
               }
            }
         }
         break;
         case PtxtType::UNPROCESSED_WSTR: TODO;
         case PtxtType::UNPROCESSED_STR8: TODO;
         case PtxtType::UNPROCESSED_STR16: TODO;
         case PtxtType::UNPROCESSED_STR32: TODO;

         default: UNREACHABLE;
      }
      getNextToken();
   } while (currTok.type() == TokenType::PTXT_SEG && isString(currTok.ptxtType()));
   return tree.storeString(std::move(str));
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