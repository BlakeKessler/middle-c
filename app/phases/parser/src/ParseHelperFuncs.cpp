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
      iden.addQuals(ptrquals);
      TypeSpec* typeDef = tree.makeIndirType(name, iden.symbol()->type(), iden.quals(), entry);
      IndirTable& indirTable = typeDef->indirTable();
      while (qualsAndMods()) {
         if (+(ptrquals & QualMask::VIEW)) {
            indirTable.appendView(entry);
         } else {
            indirTable.append(entry);
         }
      }
      iden.setQualMask(ptrquals);
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

   index<Identifier> typeName = parseTypename(SymbolType::EXTERN_TYPE, true);
   index<Identifier> varName = tryParseIdentifier(SymbolType::VAR, tree[typeName].symbol(), true);
   if (varName) {
      SymbolNode* varSymbol = tree[varName].symbol();
      varSymbol->setSymbolType(SymbolType::VAR);
      varSymbol->setType(tree[typeName].symbol()->type());
   }
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
            index<Decl> param = parseDefaultableParam();
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
      tree.makeIndirType(byteSpan, byteType->type(), QualMask::CONST, IndirTable::Entry(IndirTable::Entry::SLICE, true, false, false));
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

clef::TypeSpec* clef::Parser::evalType(index<astNode> i) {
   if (!i) {
      return nullptr;
   }
   switch (tree[i].nodeType()) {
      case NodeType::NONE: [[fallthrough]];
      case NodeType::ERROR:
         return nullptr;
         
      case NodeType::IDEN:
         return tree[(index<Identifier>)i].symbol()->type();
         
      case NodeType::SCOPE: TODO;

      case NodeType::LITERAL: {
         Literal& lit = tree[(index<Literal>)i];
         switch (lit.type()) {
            case LitType::NONE: TODO;
            case LitType::POINTER: TODO;

            case LitType::UINT: return tree.getFundType(KeywordID::UINT)->type();
            case LitType::SINT: return tree.getFundType(KeywordID::SINT)->type();
            case LitType::FLOAT: return tree.getFundType(KeywordID::FLONG)->type();

            case LitType::BOOL: return tree.getFundType(KeywordID::BOOL)->type();
            case LitType::CHAR: return tree.getFundType(KeywordID::CHAR)->type();

            case LitType::STRING: TODO; //const char[]
            case LitType::INTERP_STR: TODO;
            case LitType::FORMAT: TODO;
            case LitType::REGEX: TODO;

            case LitType::TYPEID: return static_cast<TypeSpec*>(lit);
         }
         UNREACHABLE;
      }

      case NodeType::EXPR: [[fallthrough]];
      case NodeType::STMT: [[fallthrough]];
      case NodeType::DECL: [[fallthrough]];
      case NodeType::FUNC_DEF: [[fallthrough]];
      case NodeType::MACRO_DEF: [[fallthrough]];
      case NodeType::MAKE_TYPE: [[fallthrough]];
      case NodeType::FOR_LOOP: [[fallthrough]];
      case NodeType::FOREACH_LOOP: [[fallthrough]];
      case NodeType::WHILE_LOOP: [[fallthrough]];
      case NodeType::DO_WHILE_LOOP: [[fallthrough]];
      case NodeType::IF: [[fallthrough]];
      case NodeType::SWITCH: [[fallthrough]];
      case NodeType::MATCH: [[fallthrough]];
      case NodeType::ASM: [[fallthrough]];
      case NodeType::TRY_CATCH:
         return tree[(index<Expr>)i].evalType();

      case NodeType::SWITCH_CASES: UNREACHABLE;
      case NodeType::MATCH_CASES: UNREACHABLE;
      case NodeType::STMT_SEQ: UNREACHABLE;
      case NodeType::ARG_LIST: UNREACHABLE;
   }
}
void clef::Parser::updateEvalType(index<Expr> i) {
   debug_assert(i);

   Expr& expr = tree[i];

   bool isAllFundNoIndir = 
      (expr.lhs() || evalType(+expr.lhs())->metaType() == TypeSpec::FUND_TYPE) &&
      (expr.rhs() || evalType(+expr.rhs())->metaType() == TypeSpec::FUND_TYPE) &&
      (expr.extra() || evalType(+expr.extra())->metaType() == TypeSpec::FUND_TYPE) &&
      (expr.extra2() || evalType(+expr.extra2())->metaType() == TypeSpec::FUND_TYPE) &&
      (expr.lhs() || evalType(+expr.lhs())->metaType() == TypeSpec::FUND_TYPE);

   if (isAllFundNoIndir) {
      switch (expr.opID()) {
            case OpID::TERNARY_INVOKE: 
               if (!(expr.evalType() = commonType(+expr.rhs(), +expr.extra()))) {
                  logError(ErrCode::TYPECHECK_ERR, "operands of ternary expression must have a common type");
               }
               return;


            case OpID::PREPROCESSOR: TODO;

            case OpID::SCOPE_RESOLUTION: UNREACHABLE;

               TODO;

            case OpID::RANGE: TODO;
            case OpID::SPREAD: TODO;

            //unary identity - common type
            case OpID::INC: [[fallthrough]];
            case OpID::DEC: [[fallthrough]];
            case OpID::BIT_NOT: [[fallthrough]];
            //bitwise binary identity - common type
            case OpID::BIT_AND: [[fallthrough]];
            case OpID::BIT_OR: [[fallthrough]];
            case OpID::BIT_XOR: [[fallthrough]];
            case OpID::SHIFT_LEFT: [[fallthrough]];
            case OpID::SHIFT_RIGHT: [[fallthrough]];
            //binary identity - common type
            case OpID::ADD: [[fallthrough]];
            case OpID::SUB: [[fallthrough]];
            case OpID::MUL: [[fallthrough]];
            case OpID::DIV: [[fallthrough]];
            case OpID::MOD: [[fallthrough]];
            case OpID::EXP: [[fallthrough]];
            case OpID::COALESCE:
               expr.evalType() = commonTypeOfOperands(i);
               return;

            //unary boolean - bool
            case OpID::LOGICAL_NOT: [[fallthrough]];
            //binary boolean - bool
            case OpID::LOGICAL_AND: [[fallthrough]];
            case OpID::LOGICAL_OR: [[fallthrough]];
            case OpID::LESSER: [[fallthrough]];
            case OpID::GREATER: [[fallthrough]];
            case OpID::LESSER_OR_EQ: [[fallthrough]];
            case OpID::GREATER_OR_EQ: [[fallthrough]];
            case OpID::IS_EQUAL: [[fallthrough]];
            case OpID::IS_UNEQUAL: [[fallthrough]];
            //case OpID::IS_EQUAL_STRICT: [[fallthrough]];
            //case OpID::IS_UNEQUAL_STRICT: [[fallthrough]];
            case OpID::THREE_WAY_COMP:
               expr.evalType() = tree.getFundType(KeywordID::BOOL)->type();
               return;

            //null - lhs
            case OpID::NULL:
            //assignments - lhs
            case OpID::ASSIGN: [[fallthrough]];
            //case OpID::CONST_ASSIGN: [[fallthrough]];
            case OpID::ADD_ASSIGN: [[fallthrough]];
            case OpID::SUB_ASSIGN: [[fallthrough]];
            case OpID::MUL_ASSIGN: [[fallthrough]];
            case OpID::DIV_ASSIGN: [[fallthrough]];
            case OpID::MOD_ASSIGN: [[fallthrough]];
            case OpID::EXP_ASSIGN: [[fallthrough]];
            case OpID::SHL_ASSIGN: [[fallthrough]];
            case OpID::SHR_ASSIGN: [[fallthrough]];
            case OpID::AND_ASSIGN: [[fallthrough]];
            case OpID::XOR_ASSIGN: [[fallthrough]];
            case OpID::OR_ASSIGN: [[fallthrough]];
            case OpID::COALESCE_ASSIGN:
               expr.evalType() = evalType(+expr.lhs());
               return;

            //misc - rhs
            case OpID::COMMA: [[fallthrough]];
            case OpID::LET: [[fallthrough]];
            case OpID::MAKE_TYPE:
               expr.evalType() = evalType(+expr.rhs());
               return;

            //pseudo-operators - typeless
            case OpID::FOR: [[fallthrough]];
            case OpID::FOREACH: [[fallthrough]];
            case OpID::WHILE: [[fallthrough]];
            case OpID::DO_WHILE: [[fallthrough]];
            case OpID::GOTO: [[fallthrough]];
            case OpID::GOTO_CASE: [[fallthrough]];
            case OpID::IF: [[fallthrough]];
            case OpID::SWITCH: [[fallthrough]];
            case OpID::MATCH: [[fallthrough]];
            case OpID::TRY_CATCH: [[fallthrough]];
            case OpID::ASM: [[fallthrough]];
            case OpID::BREAK: [[fallthrough]];
            case OpID::CONTINUE: [[fallthrough]];
            case OpID::THROW: [[fallthrough]];
            case OpID::ASSERT: [[fallthrough]];
            case OpID::DEBUG_ASSERT: [[fallthrough]];
            case OpID::STATIC_ASSERT: [[fallthrough]];
            case OpID::ASSUME: [[fallthrough]];
            case OpID::RETURN: [[fallthrough]];
            case OpID::ALIAS: [[fallthrough]];
            case OpID::DEF_FUNC_PARAMS: [[fallthrough]];
            case OpID::DEF_MACRO_PARAMS: [[fallthrough]];
            //labels - typeless
            case OpID::LABEL_DELIM:
               expr.evalType() = nullptr;
               return;

            //errors
            case OpID::MEMBER_ACCESS: [[fallthrough]];
            case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
            case OpID::METHOD_PTR: [[fallthrough]];
            case OpID::ARROW_METHOD_PTR: [[fallthrough]];
            case OpID::CALL_INVOKE: [[fallthrough]];
            case OpID::SUBSCRIPT_INVOKE: [[fallthrough]];
            case OpID::LIST_INVOKE: [[fallthrough]];
            case OpID::SPECIALIZER_INVOKE: [[fallthrough]];
            case OpID::INTERP_STR_INVOKE: logError(ErrCode::BAD_EXPR, "invalid operator for types");

            //unreachables
            case OpID::INLINE_IF: [[fallthrough]];
            case OpID::UNIQUE_PTR: [[fallthrough]];
            case OpID::SHARED_PTR: [[fallthrough]];
            case OpID::WEAK_PTR: [[fallthrough]];
            case OpID::ITERATOR: [[fallthrough]];
            case OpID::PREPROC_IMPORT: [[fallthrough]];
            case OpID::PREPROC_LINK: [[fallthrough]];
            case OpID::PREPROC_EMBED: [[fallthrough]];
            case OpID::ESCAPE: [[fallthrough]];
            case OpID::EOS: [[fallthrough]];
            case OpID::STRING: [[fallthrough]];
            case OpID::CHAR: [[fallthrough]];
            case OpID::INTERP_STRING: [[fallthrough]];
            case OpID::LINE_CMNT: [[fallthrough]];
            case OpID::BLOCK_CMNT: [[fallthrough]];
            case OpID::BLOCK_CMNT_OPEN: [[fallthrough]];
            case OpID::BLOCK_CMNT_CLOSE: [[fallthrough]];
            case OpID::CALL_OPEN: [[fallthrough]];
            case OpID::CALL_CLOSE: [[fallthrough]];
            case OpID::SUBSCRIPT_OPEN: [[fallthrough]];
            case OpID::SUBSCRIPT_CLOSE: [[fallthrough]];
            case OpID::LIST_OPEN: [[fallthrough]];
            case OpID::LIST_CLOSE: [[fallthrough]];
            case OpID::SPECIALIZER_OPEN: [[fallthrough]];
            case OpID::SPECIALIZER_CLOSE: [[fallthrough]];
            case OpID::CHAR_INVOKE: [[fallthrough]];
            case OpID::STR_INVOKE: UNREACHABLE;
         }
   }

   switch (expr.opID()) {
      #define __TYPELESS expr.evalType() = nullptr; return
      case OpID::NULL:
         debug_assert(!expr.rhs() && !expr.extra() && !expr.extra2());
         expr.evalType() = evalType(+expr.lhs());
         return;

      case OpID::ESCAPE: UNREACHABLE;
      case OpID::EOS: UNREACHABLE;

      case OpID::STRING: UNREACHABLE;
      case OpID::CHAR: UNREACHABLE;
      case OpID::INTERP_STRING: UNREACHABLE;

      case OpID::LINE_CMNT: UNREACHABLE;
      case OpID::BLOCK_CMNT: UNREACHABLE;
      case OpID::BLOCK_CMNT_OPEN: UNREACHABLE;
      case OpID::BLOCK_CMNT_CLOSE: UNREACHABLE;

      case OpID::CALL_OPEN: UNREACHABLE;
      case OpID::CALL_CLOSE: UNREACHABLE;
      case OpID::SUBSCRIPT_OPEN: UNREACHABLE;
      case OpID::SUBSCRIPT_CLOSE: UNREACHABLE;
      case OpID::LIST_OPEN: UNREACHABLE;
      case OpID::LIST_CLOSE: UNREACHABLE;
      case OpID::SPECIALIZER_OPEN: UNREACHABLE;
      case OpID::SPECIALIZER_CLOSE: UNREACHABLE;

      case OpID::CALL_INVOKE:
      case OpID::SUBSCRIPT_INVOKE:
      case OpID::LIST_INVOKE:
      case OpID::SPECIALIZER_INVOKE:

         TODO;

      case OpID::CHAR_INVOKE: UNREACHABLE;
      case OpID::STR_INVOKE: UNREACHABLE;
      case OpID::INTERP_STR_INVOKE: TODO;
      case OpID::TERNARY_INVOKE: 
         if (!(expr.evalType() = commonType(+expr.rhs(), +expr.extra()))) {
            logError(ErrCode::TYPECHECK_ERR, "operands of ternary expression must have a common type");
         }
         return;


      case OpID::PREPROCESSOR: TODO;

      case OpID::SCOPE_RESOLUTION: UNREACHABLE;

         TODO;

      case OpID::MEMBER_ACCESS: [[fallthrough]];
      case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
      case OpID::METHOD_PTR: [[fallthrough]];
      case OpID::ARROW_METHOD_PTR:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::RANGE: TODO;
      case OpID::SPREAD: TODO;

      //unary identity
      case OpID::INC:
      case OpID::DEC:
      case OpID::BIT_NOT:

      //bitwise binary identity
      case OpID::BIT_AND:
      case OpID::BIT_OR:
      case OpID::BIT_XOR:
      case OpID::SHIFT_LEFT:
      case OpID::SHIFT_RIGHT:

      //bitwise binary identity
      case OpID::ADD:
      case OpID::SUB:
      case OpID::MUL:
      case OpID::DIV:
      case OpID::MOD:
      case OpID::EXP:

      case OpID::THREE_WAY_COMP: TODO;

      //unary boolean
      case OpID::LOGICAL_NOT:

      //binary boolean
      case OpID::LOGICAL_AND:
      case OpID::LOGICAL_OR:
      case OpID::LESSER:
      case OpID::GREATER:
      case OpID::LESSER_OR_EQ:
      case OpID::GREATER_OR_EQ:
      case OpID::IS_EQUAL:
      case OpID::IS_UNEQUAL:
      //case OpID::IS_EQUAL_STRICT:
      //case OpID::IS_UNEQUAL_STRICT:


      case OpID::UNIQUE_PTR: TODO;
      case OpID::SHARED_PTR: TODO;
      case OpID::WEAK_PTR: TODO;
      case OpID::ITERATOR: TODO;



      case OpID::COALESCE: TODO;

      case OpID::INLINE_IF: UNREACHABLE;
      case OpID::LABEL_DELIM: __TYPELESS;

      case OpID::ASSIGN:
         expr.evalType() = evalType(+expr.lhs());
         return;
      //case OpID::CONST_ASSIGN: TODO;
      case OpID::ADD_ASSIGN:
      case OpID::SUB_ASSIGN:
      case OpID::MUL_ASSIGN:
      case OpID::DIV_ASSIGN:
      case OpID::MOD_ASSIGN:
      case OpID::EXP_ASSIGN:
      case OpID::SHL_ASSIGN:
      case OpID::SHR_ASSIGN:
      case OpID::AND_ASSIGN:
      case OpID::XOR_ASSIGN:
      case OpID::OR_ASSIGN:
      case OpID::COALESCE_ASSIGN:


         TODO;

      case OpID::COMMA:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::FOR: [[fallthrough]];
      case OpID::FOREACH: [[fallthrough]];
      case OpID::WHILE: [[fallthrough]];
      case OpID::DO_WHILE: [[fallthrough]];

      case OpID::GOTO: [[fallthrough]];
      case OpID::GOTO_CASE: [[fallthrough]];

      case OpID::IF: [[fallthrough]];

      case OpID::SWITCH: [[fallthrough]];
      case OpID::MATCH: [[fallthrough]];

      case OpID::TRY_CATCH: [[fallthrough]];
      case OpID::ASM: [[fallthrough]];

      case OpID::BREAK: [[fallthrough]];
      case OpID::CONTINUE: [[fallthrough]];

      case OpID::THROW: [[fallthrough]];
      case OpID::ASSERT: [[fallthrough]];
      case OpID::DEBUG_ASSERT: [[fallthrough]];
      case OpID::STATIC_ASSERT: [[fallthrough]];
      case OpID::ASSUME: [[fallthrough]];
      case OpID::RETURN: [[fallthrough]];

      case OpID::ALIAS:
         __TYPELESS;

      case OpID::LET: [[fallthrough]];
      case OpID::MAKE_TYPE:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::DEF_FUNC_PARAMS: [[fallthrough]];
      case OpID::DEF_MACRO_PARAMS: __TYPELESS;


      case OpID::PREPROC_IMPORT: TODO;
      case OpID::PREPROC_LINK: TODO;
      case OpID::PREPROC_EMBED: TODO;
      #undef __TYPELESS
   }
}
void clef::Parser::updateEvalType_r(index<Expr> i) {
   debug_assert(i);

   Expr& expr = tree[i];

   //recursively update types
   if (expr.lhs() && canDownCastTo(expr.lhsType(), NodeType::EXPR)) {
      updateEvalType_r(+expr.lhs());
   }
   if (expr.rhs() && canDownCastTo(expr.rhsType(), NodeType::EXPR)) {
      updateEvalType_r(+expr.rhs());
   }
   if (expr.extra() && canDownCastTo(expr.extraType(), NodeType::EXPR)) {
      updateEvalType_r(+expr.extra());
   }
   if (expr.extra2() && canDownCastTo(expr.extraType2(), NodeType::EXPR)) {
      updateEvalType_r(+expr.extra2());
   }

   //update own type
   updateEvalType(i);
}

#endif //PARSER_HELPERS_CPP