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
      if (currTok.block().type == type && currTok.block().role == role) {
         getNextToken();
         return true;
      }
   }
   return false;
}


clef::index<clef::Scope> clef::Parser::parseProcedure() {
   index<Scope> scope = make<Scope>(&tree.allocBuf<index<Stmt>>());

   while (!src.done()) {
      if (tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      index<Stmt> tmp = parseStmt();
      tree[scope].push_back(tmp);
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}

//parse a typename
//assumed to already be declared if !isDecl
//!TODO: allow inline type definitions (EX: `let struct{let uint n;} var;`)
clef::index<clef::Identifier> clef::Parser::parseTypename(SymbolType symbolType, bool isDecl) {
   index<Identifier> name = parseSymbol(symbolType, nullptr, isDecl);
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

   //pointer and reference types
   IndirTable::Entry entry;
   QualMask ptrquals;
   index<Expr> arrSize;
   const auto qualsAndMods = [&]() -> bool { //local function to parse qualifiers
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
      } else if (tryConsumeBlockDelim(BlockType::SUBSCRIPT, BlockDelimRole::OPEN)) { //in-place array
         entry._type = IndirTable::Entry::ARR;
         arrSize = parseExprNoPrimaryComma();
         consumeBlockDelim(BlockType::SUBSCRIPT, BlockDelimRole::CLOSE, "array bounds must be a single integer constant expression");
         isDone = true;

         //special handling for in-place arrays
         ptrquals = parseQuals();
         if (+(ptrquals & QualMask::CONST)) {
            logError(ErrCode::BAD_IDEN, "in-place arrays cannot be marked `const`");
         }
         ptrquals |= QualMask::CONST;
         goto AFTER_PARSE_QUALS;
      }
      ptrquals = parseQuals(); AFTER_PARSE_QUALS:
      if (entry.setQuals(ptrquals)) {
         logError(ErrCode::BAD_IDEN, "illegal qualifiers for typename");
      }
      return isDone;
   };
   if (qualsAndMods()) {
      QualMask targetQuals = iden.quals() | ptrquals;
      IndirTable indirTable{entry};
      if (entry.isArr()) {
         indirTable.appendArrExtent(arrSize, sizeof(arrSize));
      }
      while (qualsAndMods()) {
         if (+(ptrquals & QualMask::VIEW)) {
            indirTable.appendView(entry);
         } else {
            indirTable.append(entry);
         }
         if (entry.isArr()) {
            indirTable.appendArrExtent(arrSize, sizeof(arrSize));
         }
      }
      tree.makeIndirType(name, iden.symbol()->type(), targetQuals, std::move(indirTable));
      iden.setQualMask(ptrquals);
   }
   //variadic parameters
   if (tryConsumeOperator(OpID::VARIADIC_PARAM)) {
      iden.addQuals(QualMask::VARIADIC);
   }
   //return
   return name;
}

//parse a variable declaration (where required, it is assumed that the `let` keyword has already been consumed)
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
      return make<Decl>(tree[funcDef].name(), tree[funcDef].name(), val);
   }
   
   //declaration
   index<Identifier> type = parseTypename(SymbolType::EXTERN_TYPE, true);
   index<Identifier> varName = parseSymbol(SymbolType::VAR, tree[type].symbol(), true);
   SymbolNode* varSymbol = tree[varName].symbol();
   varSymbol->setSymbolType(SymbolType::VAR);
   varSymbol->setType(tree[type].symbol()->type());
   //definition (optional)
   index<Expr> val;
   if (tryConsumeOperator(OpID::ASSIGN)) {
      val = parseExpr();
   }
   else if (tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::LIST, false);
      val = make<Expr>(OpID::LIST_INVOKE, type, args);
   }
   else if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      index<ArgList> args = parseArgList(BlockType::CALL, false);
      val = make<Expr>(OpID::CALL_INVOKE, type, args);
   } else {
      val = 0;
   }

   consumeEOS("LET statement must end with EOS token");
   if (val) {
      return make<Decl>(type, varName, val);
   }
   return make<Decl>(type, varName);
}

//parse a parameter
clef::index<clef::Decl> clef::Parser::parseParam(index<Expr> attrs) {
   if (tryConsumeKeyword(KeywordID::FUNC)) { [[unlikely]];
      index<FuncDef> funcDef = parseFunction(attrs);
      if (tree[funcDef].procedure()) {
         logError(ErrCode::BAD_DECL, "cannot inline define parameters of type func");
      }
      return make<Decl>(tree[funcDef].name(), tree[funcDef].name());
   }

   index<Identifier> typeName = parseTypename(SymbolType::EXTERN_TYPE, true);
   index<Identifier> varName = tryParseSymbol(SymbolType::VAR, tree[typeName].symbol(), true);
   if (varName) {
      SymbolNode* varSymbol = tree[varName].symbol();
      varSymbol->setSymbolType(SymbolType::VAR);
      varSymbol->setType(tree[typeName].symbol()->type());
   }
   if (attrs) { TODO; }
   return make<Decl>(typeName, varName);
}
//parse a parameter and (optional) default value
clef::index<clef::Decl> clef::Parser::parseDefaultableParam(index<Expr> attrs) {
   index<Decl> var = parseParam(attrs);
   if (tryConsumeOperator(OpID::ASSIGN)) {
      index<Expr> val = parseExpr();
      tree[var].value() = val;
   }
   return var;
}

//parse a comma-separated list of arguments (if !isDecl) or parameters (if isDecl)
clef::index<clef::ArgList> clef::Parser::parseArgList(const BlockType closer, bool isDecl) {
   index<ArgList> args = make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(closer, BlockDelimRole::CLOSE)) {
      return args;
   }
   if (isDecl) { //parameter list
      do {
         auto tmp = parseDefaultableParam(tryParseAttrs());
         tree[args].push_back(tmp);
      } while (tryConsumeOperator(OpID::COMMA));
   } else { //argument list
      do {
         auto tmp = parseExpr();
         tree[args].push_back(tmp);
      } while (tryConsumeOperator(OpID::COMMA));
   }
   consumeBlockDelim(closer, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
   return args;
}

//parse a specializer list
clef::index<clef::ArgList> clef::Parser::parseSpecList(index<Identifier> target, bool isDecl) {
   index<ArgList> args = make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE)) {
      return args;
   }
   if (isDecl) { //template declaration
      do {
         if (tryConsumeKeyword(KeywordID::TYPE)) { //typename template parameter
            index<Identifier> typeName = parseTypename(SymbolType::GENERIC, true);
            index<TypeDecl> typeDecl;
            if (tryConsumeOperator(OpID::ASSIGN)) {
               index<Identifier> defaultType = parseTypename(SymbolType::EXTERN_TYPE, false);
               typeDecl = make<TypeDecl>(typeName, defaultType);
            } else {
               typeDecl = make<TypeDecl>(typeName);
            }
            tree[args].push_back(typeDecl);
         } else { //value template parameter
            index<Decl> param = parseDefaultableParam(tryParseAttrs());
            tree[args].push_back(param);
         }
      } while (tryConsumeOperator(OpID::COMMA));
   } else { //templated entity
      do {
         index<Expr> arg = parseExpr();
         tree[args].push_back(arg);
      } while (tryConsumeOperator(OpID::COMMA));
   }
   //closing delimiter
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
   //update AST and return
   tree[target].specializer() = args;
   return args;
}
//parse a specializer list
clef::index<clef::ArgList> clef::Parser::parseSpecList(index<RawIdentifier> target) {
   index<ArgList> args = make<ArgList>(&tree.allocBuf<index<Expr>>());
   if (tryConsumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE)) {
      return args;
   }
   //templated entity
   do {
      index<Expr> arg = parseExpr();
      tree[args].push_back(arg);
   } while (tryConsumeOperator(OpID::COMMA));
   //closing delimiter
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE, "argument list must end with the correct closing delimiter");
   //update AST and return
   tree[target].specializer() = args;
   return args;
}

//parse a preprocessor statement
//!NOTE: currently supported directives: `import`, `link`, and `embed`
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
      index<Identifier> byteSpan = make<Identifier>(KeywordID::UBYTE, byteType);
      tree.makeIndirType(byteSpan, byteType->type(), QualMask::CONST, IndirTable(IndirTable::Entry(IndirTable::Entry::SLICE, true, false, false)));
      name = parseSymbol(SymbolType::VAR, tree[byteSpan].symbol(), true);
   } else {
      logError(ErrCode::BAD_PREPROC, "unrecognized directive");
      op = OpID::NULL;
   }

   //get path //!TODO: standard library paths
   if (currTok.type() != TokenType::PTXT_SEG || !isString(currTok.ptxtType())) {
      logError(ErrCode::BAD_PREPROC, "invalid `%s` directive", toString(op));
   }
   const mcsl::str_slice path = parseStrLit();
   index<Literal> pathLit = make<Literal>(path, FundTypeID::CHAR);
   consumeEOS("missing EOS token");

#if !PARALLEL_COMPILE_FILES
   if (op == OpID::PREPROC_IMPORT) {
      //!HACK: make this more robust
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
   return makeStmt(makeExpr(op, +name, +pathLit));
}
clef::index<clef::Expr> clef::Parser::parseCast(KeywordID castID) {
   debug_assert(isCast(castID));
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN, "must specify type of cast");
   index<Identifier> targetType = parseTypename(SymbolType::EXTERN_IDEN, false);
   consumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::CLOSE, "invalid cast target type");
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "typecasting uses function call syntax");
   index<Expr> expr = parseExprNoPrimaryComma(0);
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "bad typecast argument");
   return tree.make<Expr>(toOpID(castID), targetType, expr);
}

//parse a string literal (adjacent string literals are concatenated, like in C)
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
               if (raw[i] == '\\') { //escape sequence
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
               } else { //standard char
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


//get an expression AST node encoding of another node
clef::index<clef::Expr> clef::Parser::toExpr(index<astNode> index) {
   astNode& node = tree[index];
   switch (node.nodeType()) {
      //value nodes - return an expression with no operator and the node as the only argument
      case RawIdentifier::nodeType():
         toIden(+index);
         [[fallthrough]];
      case Identifier::nodeType():
      case Literal::nodeType():
         return makeExpr(OpID::NULL, index);

      //expressions - return the node unaltered
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
      case Asm::nodeType():
         return +index;

      //non-expression types
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

//make an expression into a statement (in-place)
clef::index<clef::Stmt> clef::Parser::makeStmt(index<Expr> expr) {
   tree[(index<astNode>)expr].anyCast(NodeType::STMT);
   return +expr;
}

clef::index<clef::Identifier> clef::Parser::toIden(index<RawIdentifier> i) {
   RawIdentifier& asRaw = tree[(index<RawIdentifier>)i];
   SymbolNode* symbol;
   if (+asRaw.keywordID()) { //keyword
      KeywordID kwid = asRaw.keywordID();
      if (isType(kwid)) {
         return remake<Identifier>((index<RawIdentifier>)i, tree.toFundTypeID(kwid), kwid, tree.getFundType(kwid), index<ArgList>{});
      }
      else if (kwid == KeywordID::THIS || kwid == KeywordID::SELF) {
         return remake<Identifier>((index<RawIdentifier>)i, kwid, currScope); //!NOTE: this breaks in subscopes
      }
      logError(ErrCode::BAD_IDEN, "keyword `%s` may not name a symbol", currTok.keywordID());
   } else { //standard identifier
      symbol = tree.findSymbol(asRaw.name(), currScope);
      return remake<Identifier>((index<RawIdentifier>)i, symbol, 0, asRaw.specializer());
   }
}

#endif //PARSER_HELPERS_CPP