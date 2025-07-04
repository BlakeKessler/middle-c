#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

void clef::Parser::parse(const mcsl::str_slice filePath, SyntaxTree& tree) {
   Lexer src = Lexer::fromFile(filePath);
   return parse(src, tree);
}
void clef::Parser::parse(Lexer& src, SyntaxTree& tree) {
   Parser parser{src, tree};
   index<StmtSeq> global = 1;
   debug_assert(tree[index<astNode>(global)].nodeType() == NodeType::STMT_SEQ);
   while (!src.done()) {
      index<Stmt> stmt = parser.parseStmt();
      tree[global].push_back(stmt);
   }
   // return parser.tree;
}
clef::index<clef::Stmt> clef::Parser::parseStmt() {
   clef::index<clef::Stmt> tmp = parseStmtImpl();
   if (tryConsumeOperator(OpID::COMMA)) {
      logError(ErrCode::BAD_EXPR, "`;,` is illegal");
   }
   return tmp;
}
clef::index<clef::Stmt> clef::Parser::parseStmtImpl() {
   index<Expr> attrs = tryParseAttrs();
START_PARSE_STMT:
   switch (currTok.type()) {
      case TokenType::NONE        : UNREACHABLE;
      
      case TokenType::KEYWORD     :
         switch (currTok.keywordID()) {
            case KeywordID::VOID    : [[fallthrough]];
            case KeywordID::AUTO    : [[fallthrough]];
            default:
               debug_assert(isType(currTok.keywordID()));
               logError(ErrCode::BAD_KEYWORD, "floating typename (declarations must use the `let` keyword)");
               break;
            case KeywordID::LET     : {
               getNextToken();
               return makeStmt(parseDecl(attrs));
            } break;

            case KeywordID::NULLPTR : [[fallthrough]];
            case KeywordID::THIS    : [[fallthrough]];
            case KeywordID::SELF    : [[fallthrough]];
            case KeywordID::TRUE    : [[fallthrough]];
            case KeywordID::FALSE   :
               goto STMT_STARTS_WITH_VALUE;

            case KeywordID::ASM           : getNextToken(); return parseASM(attrs); break;
            
            case KeywordID::CLASS         : getNextToken(); return parseClass(attrs);
            case KeywordID::STRUCT        : getNextToken(); return parseStruct(attrs);
            case KeywordID::TRAIT         : getNextToken(); return parseTrait(attrs);
            case KeywordID::UNION         : getNextToken(); return parseUnion(attrs);
            case KeywordID::ENUM          : getNextToken(); return parseEnum(attrs);
            case KeywordID::MASK          : getNextToken(); return parseMask(attrs);
            case KeywordID::NAMESPACE     : getNextToken(); return parseNamespace(attrs);
            case KeywordID::FUNC          : getNextToken(); return makeStmt(parseFunction(attrs));
            case KeywordID::MACRO         : getNextToken(); return makeStmt(parseMacro(attrs));

            
            case KeywordID::IF            : getNextToken(); return addAttrs(parseIf(), attrs); break;
            case KeywordID::ELSE          : logError(ErrCode::BAD_KEYWORD, "floating ELSE");

            case KeywordID::FOR           : getNextToken(); return addAttrs(parseForLoop(), attrs); break;
            case KeywordID::FOREACH       : getNextToken(); return addAttrs(parseForeachLoop(), attrs); break;
            case KeywordID::WHILE         : getNextToken(); return addAttrs(parseWhileLoop(), attrs); break;
            case KeywordID::DO            : getNextToken(); return addAttrs(parseDoWhileLoop(), attrs); break;

            case KeywordID::SWITCH        : getNextToken(); return addAttrs(parseSwitch(), attrs); break;
            case KeywordID::MATCH         : getNextToken(); return addAttrs(parseMatch(), attrs); break;
            
            case KeywordID::CASE          : logError(ErrCode::BAD_KEYWORD, "floating CASE");
            case KeywordID::DEFAULT       : logError(ErrCode::BAD_KEYWORD, "floating DEFAULT");

            case KeywordID::GOTO          : {
               getNextToken();
               //goto case
               if (tryConsumeKeyword(KeywordID::CASE)) {
                  index<Expr> caseExpr = parseExpr();
                  consumeEOS("bad GOTO CASE statement (missing EOS)");
                  return addAttrs(tree.make<Stmt>(OpID::GOTO_CASE, caseExpr), attrs);
               }
               //goto default
               if (tryConsumeKeyword(KeywordID::DEFAULT)) {
                  consumeEOS("bad GOTO DEFAULT statement (missing EOS)");
                  return addAttrs(tree.make<Stmt>(OpID::GOTO_CASE), attrs);
               }
               //goto
               index<Identifier> label = parseIdentifier(SymbolType::LABEL, nullptr, false);
               if (tree[label].scopeName()) { logError(ErrCode::BAD_IDEN, "label may not be scoped"); }
               consumeEOS("bad GOTO statement (missing EOS)");
               return addAttrs(tree.make<Stmt>(OpID::GOTO, label), attrs);
            }

            case KeywordID::TRY           : getNextToken(); return addAttrs(parseTryCatch(), attrs); break;
            case KeywordID::CATCH         : logError(ErrCode::BAD_KEYWORD, "floating CATCH");

            case KeywordID::BREAK         :
               getNextToken();
               consumeEOS("bad BREAK");
               return addAttrs(tree.make<Stmt>(KeywordID::BREAK), attrs);
            case KeywordID::CONTINUE      :
               getNextToken();
               consumeEOS("bad CONTINUE");
               return addAttrs(tree.make<Stmt>(KeywordID::CONTINUE), attrs);

            case KeywordID::THROW         : [[fallthrough]];
            case KeywordID::ASSERT        : [[fallthrough]];
            case KeywordID::DEBUG_ASSERT  : [[fallthrough]];
            case KeywordID::STATIC_ASSERT : [[fallthrough]];
            case KeywordID::ASSUME        : {
               KeywordID kw = currTok.keywordID();
               getNextToken();
               index<Expr> expr = parseExpr();
               consumeEOS("bad ASSUME statement");
               return addAttrs(tree.make<Stmt>(kw, expr), attrs);
            }
            case KeywordID::RETURN        : {
               getNextToken();
               if (tryConsumeEOS()) {
                  return addAttrs(tree.make<Stmt>(KeywordID::RETURN), attrs);
               } else {
                  index<Expr> expr = parseExpr();
                  consumeEOS("bad RETURN statement");
                  return addAttrs(tree.make<Stmt>(KeywordID::RETURN, expr), attrs);
               }
            }

            case KeywordID::ALIAS         : {
               getNextToken();
               index<Identifier> alias = parseIdentifier(SymbolType::EXTERN_IDEN, nullptr, true);
               consumeOperator(OpID::ASSIGN, "alias definitions must use the assignment operator (and cannot be forward-declared)");
               index<Expr> valExpr = parseExpr();
               Expr& val = tree[valExpr];
               SymbolNode* symbol = tree[alias].symbol();
               if (val.opID() == OpID::NULL && val.lhsType() == NodeType::IDEN && val.rhsType() == NodeType::NONE && val.extraType() == NodeType::NONE && val.extraType2() == NodeType::NONE) {
                  SymbolNode* target = tree[(index<Identifier>)val.lhs()].symbol();
                  tree.registerAlias(symbol, target);
               } else {
                  TypeSpec* spec = tree.evalType(+valExpr);
                  symbol->setType(spec);
                  symbol->setSymbolType(SymbolType::VAR);
               }
               return addAttrs(tree.make<Stmt>(KeywordID::ALIAS, alias, valExpr), attrs);
            }

            UNREACHABLE;

            case KeywordID::CAST          : [[fallthrough]];
            case KeywordID::UP_CAST       : [[fallthrough]];
            case KeywordID::DYN_CAST      : [[fallthrough]];
            case KeywordID::BIT_CAST      : [[fallthrough]];
            case KeywordID::CONST_CAST    : {
               index<Expr> stmtContents = parseCast(currTok.keywordID());
               getNextToken();
               consumeEOS("bad cast statement");
               return addAttrs(makeStmt(stmtContents), attrs);
            }

            case KeywordID::_NOT_A_KEYWORD: UNREACHABLE;
         }
      case TokenType::IDEN        : PARSE_IDEN: {
         index<Expr> stmtContents = parseExpr();
         if (tryConsumeOperator(OpID::LABEL_DELIM)) { //label
            return addAttrs(tree.make<Stmt>(OpID::LABEL_DELIM, stmtContents), attrs);
         }
         consumeEOS("invalid statement");
         return addAttrs(makeStmt(stmtContents), attrs);
      }
      UNREACHABLE;

      case TokenType::OP          : if (currTok.opID() == OpID::SCOPE_RESOLUTION) { goto PARSE_IDEN; } [[fallthrough]];
      case TokenType::INT_NUM     : [[fallthrough]];
      case TokenType::REAL_NUM    : [[fallthrough]];
      case TokenType::BLOCK_DELIM : [[fallthrough]];
      case TokenType::PTXT_SEG    : STMT_STARTS_WITH_VALUE: {
         index<Expr> stmtContents = parseExpr();
         consumeEOS("statement must end with EOS token");
         return addAttrs(makeStmt(stmtContents), attrs);
      }

      case TokenType::PREPROC_INIT: if (attrs) { TODO; } getNextToken(); return parsePreprocStmt();
      case TokenType::PREPROC_EOS : getNextToken(); goto START_PARSE_STMT;
      case TokenType::EOS         :
         getNextToken();
         if (attrs) {
            return makeStmt(attrs);
         } else {
            return tree.make<Stmt>();
         }
      case TokenType::ESC         : UNREACHABLE;

      default: UNREACHABLE;
   }
   UNREACHABLE;
}

clef::index<clef::Expr> clef::Parser::parseExpr(index<astNode> initOperand) {
   index<Expr> expr = parseExprNoPrimaryComma(initOperand);
   while (tryConsumeOperator(OpID::COMMA)) {
      expr = tree.make<Expr>(OpID::COMMA, expr, parseExprNoPrimaryComma());
   }
   return expr;
}
clef::index<clef::Expr> clef::Parser::parseExprNoPrimaryComma(index<astNode> initOperand) {
   mcsl::dyn_arr<OpData> operatorStack;
   mcsl::dyn_arr<index<astNode>> operandStack;
   bool prevTokIsOperand = false;

   if (initOperand) {
      operandStack.push_back(initOperand);
   }

   const auto eval = [&]() { //evaluate the subexpression on the top of the stacks
      OpData op = operatorStack.pop_back();
      if (!operandStack.size()) { logError(ErrCode::BAD_EXPR, "bad expression (missing RHS on stack)"); }

      index<astNode> rhs = operandStack.pop_back();
      index<astNode> lhs;
      if (op.opID() == OpID::TERNARY_INVOKE) {
         if (operandStack.size() < 2) {
            logError(ErrCode::BAD_EXPR, "bad ternary conditional expression");
         }
         lhs = operandStack.pop_back();
         index<astNode> cond = operandStack.pop_back();
         operandStack.push_back(+tree.make<Expr>(Expr::makeTernary(tree, cond, lhs, rhs)));
      }
      else if (isBinary(op)) { //!NOTE: PRIORITIZES BINARY OVER POSTFIX-UNARY
         if (!operandStack.size()) { logError(ErrCode::BAD_EXPR, "bad expression (missing LHS on stack)"); }
         lhs = operandStack.pop_back();
         operandStack.push_back(+tree.makeExpr(op.opID(), lhs, rhs));
      } else {
         if (+(op & OpProps::CAN_BE_POSTFIX)) {
            operandStack.push_back(+tree.makeExpr(op.opID(), rhs));
         } else {
            debug_assert(+(op & OpProps::CAN_BE_PREFIX));
            operandStack.push_back(+tree.makeExpr(op.opID(), 0, rhs));
         }
      }
   };

   while (!src.done()) {
      switch (currTok.type()) {
         case TokenType::NONE: UNREACHABLE;
         case TokenType::__OPLIKE: UNREACHABLE;

         case TokenType::PREPROC_EOS: getNextToken(); goto PARSE_EXPR_CONTINUE;
         case TokenType::ESC: UNREACHABLE;
         
         case TokenType::KEYWORD:{
            const KeywordID kw = currTok.keywordID();
            if (kw == KeywordID::FUNC) {
               getNextToken();
               operandStack.push_back(+parseFunction(0));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (kw == KeywordID::LET) {
               getNextToken();
               operandStack.push_back(+parseDecl(0));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isValue(kw)) {
               operandStack.push_back(tree.getValueKeyword(kw));
               getNextToken();
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isCast(kw)) {
               getNextToken();
               operandStack.push_back(+parseCast(kw));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isC_FunctionLike(kw)) {
               getNextToken();
               consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "bad function-like keyword use");
               index<ArgList> argList = parseArgList(BlockType::CALL, false);
               operandStack.push_back(+tree.make<Expr>(kw, argList));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isType(kw)) {
               TODO;
               // operandStack.push_back(+tree.getFundType(kw));
               getNextToken();
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            
            logError(ErrCode::BAD_KEYWORD, "bad keyword in expression");
         }

         case TokenType::MACRO_INVOKE: TODO;
         case TokenType::IDEN: operandStack.push_back(+parseIdentifier(SymbolType::EXTERN_IDEN, nullptr, false)); prevTokIsOperand = true; goto PARSE_EXPR_CONTINUE;
         case TokenType::INT_NUM:
            operandStack.push_back(+tree.make<Literal>(currTok.intVal()));
            getNextToken();
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         case TokenType::REAL_NUM:
            operandStack.push_back(+tree.make<Literal>(currTok.realVal()));
            getNextToken();
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         case TokenType::PTXT_SEG:
            prevTokIsOperand = true;
            if (isString(currTok.ptxtType())) {
               operandStack.push_back(+tree.make<Literal>(parseStrLit()));
               goto PARSE_EXPR_CONTINUE;
            } else if (currTok.ptxtType() == PtxtType::CHAR) {
               operandStack.push_back(+tree.make<Literal>(currTok.charVal()));
               getNextToken();
               goto PARSE_EXPR_CONTINUE;
            } else {
               logError(ErrCode::PARSER_NOT_IMPLEMENTED, "cannot currently parse this plaintext segment type (%u)", +currTok.ptxtType());
               //!TODO: support other types of plaintext literal
            }
            UNREACHABLE;

         case TokenType::EOS: goto END_OF_EXPR;
         case TokenType::OP:
            if (currTok.opID() == OpID::SCOPE_RESOLUTION) {
               operandStack.push_back(+parseIdentifier(SymbolType::EXTERN_IDEN, nullptr, false));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            if (currTok.opID() == OpID::COMMA || currTok.opID() == OpID::INLINE_ELSE) {
               goto END_OF_EXPR;
            }
            else if (currTok.opID() == OpID::INLINE_IF) {
               getNextToken();
               operatorStack.emplace_back(FMT(""), OpID::TERNARY_INVOKE, OpProps::null, (ubyte)0, TokenType::OP);
               index<Expr> trueVal = parseExpr();
               operandStack.push_back(+trueVal);
               consumeOperator(OpID::INLINE_ELSE, "bad ternary conditional expression");
               prevTokIsOperand = false;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (prevTokIsOperand) { //binary or postfix unary
               OpData op = currTok.op();
               while (operatorStack.size() && (
                  operatorStack.back().precedence() > op.precedence() || (
                     (+(op.props() & OpProps::IS_LEFT_ASSOC)) &&
                     operatorStack.back().precedence() == op.precedence()))) {
                        eval();
               }
               op.removeProps(OpProps::PREFIX);
               debug_assert(+op.props());
               operatorStack.push_back(op);
            } else { //prefix unary
               OpData op = currTok.op();
               op.removeProps(OpProps::POSTFIX | OpProps::INFIX_LEFT | OpProps::INFIX_RIGHT);
               debug_assert(+(op.props() & OpProps::PREFIX));
               operatorStack.push_back(op);
            }
            prevTokIsOperand = false;
            getNextToken();
            goto PARSE_EXPR_CONTINUE;
         case TokenType::BLOCK_DELIM: {
            BlockType blockType = currTok.blockType();
            BlockDelimRole role = currTok.blockDelimRole();
            if (!isOpener(role)) { goto END_OF_EXPR; }
            getNextToken();
            if (prevTokIsOperand) { //function call, initializer list, subscript, or specializer
               debug_assert(operandStack.size());
               index<ArgList> args = parseArgList(blockType, false);
               operandStack.push_back(+tree.makeExpr(getInvoker(blockType),operandStack.pop_back(),(index<astNode>)args));
            } else if (blockType == BlockType::INIT_LIST) { //tuple
               operandStack.push_back(+parseArgList(blockType, false));
            } else { //block subexpression
               operandStack.push_back(+parseExpr());
               consumeBlockDelim(blockType, BlockDelimRole::CLOSE, "bad block subexpression");
            }
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         }

         case TokenType::PREPROC_INIT: logError(ErrCode::BAD_EXPR, "floating preprocessor statement opener");
      }
      UNREACHABLE;
      PARSE_EXPR_CONTINUE:
   }
   END_OF_EXPR:

   while (operatorStack.size()) { eval(); }
   if (operandStack.size() != 1) {
      mcsl::err_printf(mcsl::FMT("%u excess operand(s)\n"), operandStack.size() - 1);
      logError(ErrCode::BAD_EXPR, "invalid expression");
   }
   return toExpr(operandStack[0]);
}

clef::QualMask clef::Parser::parseQuals() {
   QualMask quals = QualMask::_no_quals;
   while (currTok.type() == TokenType::KEYWORD && isQualifier(currTok.keywordID())) {
      quals |= toQual(currTok.keywordID());
      getNextToken();
   }
   return quals;
}

clef::index<clef::Identifier> clef::Parser::tryParseIdentifier(SymbolType symbolType, SymbolNode* type, bool isDecl) {
   QualMask quals = parseQuals();
   //handle keywords
   if (currTok.type() == TokenType::KEYWORD) {
      index<Identifier> keyword = tree.make<Identifier>(tree.toFundTypeID(currTok.keywordID()), currTok.keywordID(), tree.getFundType(currTok.keywordID()), index<ArgList>{}, quals);
      getNextToken();
      tree[keyword].addQuals(parseQuals());
      if (tryConsumeOperator(OpID::SCOPE_RESOLUTION)) {
         logError(ErrCode::BAD_KEYWORD, "keywords may not name scopes");
      }
      return keyword;
   }

   //handle other identifiers
   if (currTok.type() != TokenType::IDEN) { return 0; }

   SAVE_SCOPE;

   index<Identifier> name = scopeName;
   SymbolNode* symbol = currScope;

   if (tryConsumeOperator(OpID::SCOPE_RESOLUTION)) {
      name = 0;
      symbol = tree.globalScope();
   }

   do {
#if !PARALLEL_COMPILE_FILES
      //!TODO: make this less janky
      if (symbol->symbolType() == SymbolType::EXTERN_IDEN || symbol->symbolType() == SymbolType::EXTERN_TYPE || (symbol->symbolType() == SymbolType::null && symbol != tree.globalScope())) {
         logError(ErrCode::BAD_IDEN, "undeclared identifier `%s`", astTNB(tree, name, 0));
      }
#endif
      symbol = tree.registerSymbol(currTok.name(), symbol); //add symbol to symbol table
      debug_assert(symbol);
      //create AST node for symbol
      if (!name || symbol->parentScope() != tree[name].symbol()) {
         name = tree.make<Identifier>(symbol);
      } else {
         name = tree.make<Identifier>(symbol, name);
      }
      debug_assert(tree[name].symbol() == symbol);
      getNextToken();

      if (tryConsumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN)) {
         parseSpecList(name, isDecl);
      }

      if (!tryConsumeOperator(OpID::SCOPE_RESOLUTION)) { break; }

      if (currTok.type() == TokenType::KEYWORD) { logError(ErrCode::BAD_IDEN, "keywords may not name or be members of scopes"); }
      else if (currTok.type() != TokenType::IDEN) { logError(ErrCode::BAD_IDEN, "only identifiers may name or be members of scopes (%u)", +currTok.type()); }
   } while (true);

#if !PARALLEL_COMPILE_FILES
   //!TODO: make this less janky
   if (!isDecl && (symbol->symbolType() == SymbolType::EXTERN_IDEN || symbol->symbolType() == SymbolType::EXTERN_TYPE)) {
      logError(ErrCode::BAD_IDEN, "undeclared identifier `%s`", astTNB(tree, name, 0));
   }
#endif
   //!TODO: make this less janky
   if (isDecl) {
      if (symbol->symbolType() == SymbolType::null || symbol->symbolType() == SymbolType::EXTERN_IDEN || (isType(symbolType) && symbol->symbolType() == SymbolType::EXTERN_TYPE)) {
         symbol->setSymbolType(symbolType);
      } else {
         TODO;
      }
   }

   if (symbol == tree.globalScope()) {
      //!TODO: unget token
      return 0;
   }

   tree[name].setQualMask(quals);
   if (type) {
      if (tree[name].symbol()->type()) {
         if (tree[name].symbol()->type() != type->type()) {
            logError(ErrCode::BAD_IDEN, "type mismatch");
         }
      } else {
         tree[name].symbol()->setType(type->type());
      }
   }
   
   debug_assert(symbol);
   debug_assert(tree[name].symbol() == symbol);
   
   POP_SCOPE;

   tree[name].addQuals(parseQuals());
   return name;
}
clef::index<clef::Identifier> clef::Parser::parseIdentifier(SymbolType symbolType, SymbolNode* type, bool isDecl) {
   index<Identifier> name = tryParseIdentifier(symbolType, type, isDecl);
   if (!name) {
      logError(ErrCode::BAD_IDEN, "expected an identifier");
   }
   return name;
}

clef::index<clef::If> clef::Parser::parseIf() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "IF statement without opening parens for condition");
   index<Expr> condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "IF statement without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad IF block");
   index<Scope> proc = parseProcedure();

   //EOS
   if (tryConsumeEOS()) {
      return tree.make<If>(condition, proc);
   }

   //ELSE
   consumeKeyword(KeywordID::ELSE, "IF statement without EOS token");
   switch (currTok.type()) {
      case TokenType::BLOCK_DELIM: { //basic ELSE
         consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ELSE block");
         index<Scope> elseProc = parseProcedure();
         //EOS
         consumeEOS("DO WHILE statement without EOS token");

         //return
         index<If> elseStmt = tree.make<If>((index<Expr>)0, elseProc, (index<If>)0);
         index<If> ifStmt = tree.make<If>(condition, proc, elseStmt);
         return ifStmt;
      }
      default: { //ELSE IF
         consumeKeyword(KeywordID::IF, "bad ELSE IF block");
         index<If> elifStmt = parseIf(); //will consume EOS

         //return
         index<If> ifStmt = tree.make<If>(condition, proc, elifStmt);
         return ifStmt;
      }
   }
}

clef::index<clef::Switch> clef::Parser::parseSwitch() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "SWITCH without opening parens for condition");
   index<Expr> condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "SWITCH without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad SWITCH block");
   index<Scope> procedure = tree.make<Scope>(&tree.allocBuf<index<Stmt>>());
   index<SwitchCases> cases = tree.make<SwitchCases>(&tree.allocBuf<mcsl::pair<index<Expr>, index<Stmt>>>(), procedure);

   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         index<Expr> caseExpr = parseExpr();

         consumeOperator(OpID::LABEL_DELIM, "bad CASE in SWITCH statement");
         
         index<Stmt> firstStmt = parseStmt(); //!TODO: handle code in the form of `case expr1: case expr2: stmt;`
         tree[procedure].push_back(firstStmt);
         tree[cases].push_back({caseExpr, firstStmt});

      } else if (tryConsumeKeyword(KeywordID::DEFAULT)) { //DEFAULT
         consumeOperator(OpID::LABEL_DELIM, "bad DEFAULT in SWITCH statement");
         
         index<Stmt> firstStmt = parseStmt();
         tree[procedure].push_back(firstStmt);
         tree[cases].push_back({0, firstStmt});

      } else { //standard statement
         index<Stmt> tmp = parseStmt();
         tree[procedure].push_back(tmp);
      }
   }

   //EOS
   consumeEOS("SWITCH statement without EOS");

   //return
   return tree.make<Switch>(condition, cases);
}

clef::index<clef::Match> clef::Parser::parseMatch() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "MATCH without opening parens for condition");
   index<Expr> condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "MATCH without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad MATCH block");
   index<MatchCases> cases = tree.make<MatchCases>(&tree.allocBuf<mcsl::pair<index<Expr>, index<Scope>>>());

   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      index<Expr> caseExpr;
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         caseExpr = parseExpr();
      } else {
         consumeKeyword(KeywordID::DEFAULT, "bad MATCH block procedure");
         caseExpr = 0;   
      }
      consumeOperator(OpID::LABEL_DELIM, "bad CASE or DEFAULT in MATCH statement");
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CASE or DEFAULT procedure in MATCH statement");
      index<Scope> procedure = parseProcedure();
      
      tree[cases].emplace_back(caseExpr, procedure);
   }

   //EOS
   consumeEOS("MATCH statement without EOS");

   //return
   return tree.make<Match>(condition, cases);
}

clef::index<clef::TryCatch> clef::Parser::parseTryCatch() {
   index<Scope> procedure = parseProcedure();
   consumeKeyword(KeywordID::CATCH, "TRY block without CATCH block");
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "CATCH block without opening parens");
   index<Decl> err = parseParam(tryParseAttrs());
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "CATCH block without closing parens");
   index<Scope> handler = parseProcedure();
   return tree.make<TryCatch>(procedure, err, handler);
}

clef::index<clef::FuncDef> clef::Parser::parseFunction(index<Expr> attrs) {
   index<Identifier> name = parseIdentifier(SymbolType::FUNC, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   symbol->setSymbolType(SymbolType::FUNC);
   if (attrs) { TODO; }
   
   PUSH_SCOPE;

   auto [overloadIndex, params, retType] = parseFuncSig(symbol);
   tree[name].overloadIndex() = overloadIndex;

   if (tryConsumeEOS()) { //forward declaration
      POP_SCOPE;
      return tree.make<FuncDef>(name, tree.make<ArgList>(params, retType));
      // tree.freeBuf(*params);
      // return tree.make<FuncDef>(name, index<ArgList>{});
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FUNC definition");
   index<Scope> procedure = parseProcedure();

   //EOS
   consumeEOS("function definition without EOS");

   //make definition
   index<ArgList> paramNode = tree.make<ArgList>(params, retType);
   index<FuncDef> def = tree.make<FuncDef>(name, paramNode, procedure);
   symbol->defineOverload(overloadIndex, def);

   //return
   POP_SCOPE;
   return def;
}
clef::index<clef::MacroDef> clef::Parser::parseMacro(index<Expr> attrs) {
   TODO;
}

mcsl::tuple<clef::index<void>, mcsl::dyn_arr<clef::index<clef::Expr>>*, clef::index<clef::Identifier>> clef::Parser::parseFuncSig(SymbolNode* target) {
   TypeSpec* overload = tree.registerType(nullptr, TypeSpec::FUNC_SIG, SymbolType::null);
   
   //parameters
   mcsl::dyn_arr<index<Expr>>& params = tree.allocBuf<index<Expr>>();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "function parameters must be enclosed by parentheses");
   if (!tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE)) {
      do {
         index<Decl> param = parseDefaultableParam(tryParseAttrs());
         overload->funcSig().params.push_back(tree[tree[param].type()].symbol()->type());
         params.push_back(param);
      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "missing closing parentheses in function signature");
   }
   //return type
   index<Identifier> retType;
   if (tryConsumeOperator(OpID::ARROW)) { //trailing return type
      retType = parseTypename(SymbolType::EXTERN_TYPE, false);
      overload->funcSig().retType = tree[retType].symbol()->type();
   } else { //no return type - assumed to be auto
      retType = 0;
      overload->funcSig().retType = nullptr;
   }

   //push to overload table
   auto [overloadIndex, isNew] = target->registerOverload(overload);
   if (!isNew) {
      std::destroy_at(overload);
      tree.freeBuf(params);
   }
   //return
   return {overloadIndex, &params, retType};
}

//!TODO: implement parseASM
clef::index<clef::Asm> clef::Parser::parseASM(index<Expr> attrs) {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "inline assembly is not yet supported");
}

clef::index<clef::Expr> clef::Parser::parseAttr(index<Expr> prevAttrs) {
   index<Identifier> name = parseIdentifier(SymbolType::ATTRIBUTE, nullptr, false);
   index<ArgList> args;
   if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      args = parseArgList(BlockType::CALL, false);
   } else { args = 0; }

   return tree.make<Expr>(OpID::ATTRIBUTE, name, args, prevAttrs);
}
clef::index<clef::Expr> clef::Parser::parseAttrs() {
   index<Expr> attr = 0;
   do {
      attr = parseAttr(attr);
   } while (tryConsumeOperator(OpID::ATTRIBUTE));
   return attr;
}
clef::index<clef::Expr> clef::Parser::tryParseAttrs() {
   if (tryConsumeOperator(OpID::ATTRIBUTE)) {
      return parseAttrs();
   }
   return 0;
}

clef::index<clef::Stmt> clef::Parser::addAttrs(index<Stmt> stmt, index<Expr> attrs) {
   if (!attrs) {
      return stmt;
   }
   tree[attrs].setExtra2(stmt);
   return makeStmt(attrs);
}

#endif //PARSER_CPP