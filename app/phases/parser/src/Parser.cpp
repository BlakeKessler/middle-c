#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

void clef::Parser::parse(const SourceTokens& src, SyntaxTree& tree) {
   Parser parser{src, tree};
   index<StmtSeq> global = parser.tree.make<StmtSeq>(&tree.allocBuf<index<Stmt>>());
   while (parser.tokIt < parser.endtok) {
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
START_PARSE_STMT:
   switch (tokIt->type()) {
      case TokenType::NONE        : UNREACHABLE;
      
      case TokenType::KEYWORD     :
         switch (tokIt->keywordID()) {
            case KeywordID::VOID    : [[fallthrough]];
            case KeywordID::AUTO    : [[fallthrough]];
            default:
               debug_assert(isType(tokIt->keywordID()));
               logError(ErrCode::BAD_KEYWORD, "floating typename (declarations must use the `let` keyword)");
               break;
            case KeywordID::LET     : {
               ++tokIt;
               index<Decl> tmp = parseLetStmt();
               tree[(index<astNode>)(+tmp)].anyCast(NodeType::STMT);
               return +tmp;
            } break;

            case KeywordID::NULLPTR : [[fallthrough]];
            case KeywordID::THIS    : [[fallthrough]];
            case KeywordID::SELF    : [[fallthrough]];
            case KeywordID::TRUE    : [[fallthrough]];
            case KeywordID::FALSE   :
               goto STMT_STARTS_WITH_VALUE;

            case KeywordID::ASM           : ++tokIt; return parseASM(); break;
            
            case KeywordID::CLASS         : ++tokIt; { return parseClass(); }
            case KeywordID::STRUCT        : ++tokIt; { return parseStruct(); }
            case KeywordID::INTERFACE     : ++tokIt; { return parseInterface(); }
            case KeywordID::UNION         : ++tokIt; { return parseUnion(); }
            case KeywordID::ENUM          : ++tokIt; { return parseEnum(); }
            case KeywordID::MASK          : ++tokIt; { return parseMask(); }
            case KeywordID::NAMESPACE     : ++tokIt; { return parseNamespace(); }
            case KeywordID::FUNC          : ++tokIt; { auto tmp = parseFunction(); return tree.make<TypeDecl>(tree[tmp].signature(), tmp, tmp); }

            
            case KeywordID::IF            : ++tokIt; return parseIf(); break;
            case KeywordID::ELSE          : logError(ErrCode::BAD_KEYWORD, "floating ELSE");

            case KeywordID::FOR           : ++tokIt; return parseForLoop(); break;
            case KeywordID::FOREACH       : ++tokIt; return parseForeachLoop(); break;
            case KeywordID::WHILE         : ++tokIt; return parseWhileLoop(); break;
            case KeywordID::DO            : ++tokIt; return parseDoWhileLoop(); break;

            case KeywordID::SWITCH        : ++tokIt; return parseSwitch(); break;
            case KeywordID::MATCH         : ++tokIt; return parseMatch(); break;
            
            case KeywordID::CASE          : logError(ErrCode::BAD_KEYWORD, "floating CASE");
            case KeywordID::DEFAULT       : logError(ErrCode::BAD_KEYWORD, "floating DEFAULT");

            case KeywordID::GOTO          : {
               ++tokIt;
               //goto case
               if (tryConsumeKeyword(KeywordID::CASE)) {
                  index<Expr> caseExpr = parseExpr();
                  consumeEOS("bad GOTO CASE statement (missing EOS)");
                  return tree.make<Stmt>(OpID::GOTO_CASE, caseExpr);
               }
               //goto default
               if (tryConsumeKeyword(KeywordID::DEFAULT)) {
                  consumeEOS("bad GOTO DEFAULT statement (missing EOS)");
                  return tree.make<Stmt>(OpID::GOTO_CASE);
               }
               //goto
               index<Identifier> label = parseIdentifier();
               if (tree[label].scopeName()) { logError(ErrCode::BAD_IDEN, "label may not be scoped"); }
               consumeEOS("bad GOTO statement (missing EOS)");
               return tree.make<Stmt>(OpID::GOTO, label);
            }

            case KeywordID::TRY           : ++tokIt; return parseTryCatch(); break;
            case KeywordID::CATCH         : logError(ErrCode::BAD_KEYWORD, "floating CATCH");

            case KeywordID::BREAK         :
               ++tokIt;
               consumeEOS("bad BREAK");
               return tree.make<Stmt>(KeywordID::BREAK);
            case KeywordID::CONTINUE      :
               ++tokIt;
               consumeEOS("bad CONTINUE");
               return tree.make<Stmt>(KeywordID::CONTINUE);

            case KeywordID::THROW         : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad THROW statement");
               return tree.make<Stmt>(KeywordID::THROW, expr);
            }
            case KeywordID::ASSERT        : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad ASSERT statement");
               return tree.make<Stmt>(KeywordID::ASSERT, expr);
            }
            case KeywordID::DEBUG_ASSERT  : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad DEBUG_ASSERT statement");
               return tree.make<Stmt>(KeywordID::DEBUG_ASSERT, expr);
            }
            case KeywordID::STATIC_ASSERT : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad STATIC_ASSERT statement");
               return tree.make<Stmt>(KeywordID::STATIC_ASSERT, expr);
            }
            case KeywordID::RETURN        : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad RETURN statement");
               return tree.make<Stmt>(KeywordID::RETURN, expr);
            }

            case KeywordID::ALIAS         : {
               ++tokIt;
               index<Identifier> alias = parseIdentifier();
               consumeOperator(OpID::ASSIGN, "alias definitions must use the assignment operator (and cannot be forward-declared)");
               index<Expr> valExpr = parseExpr();
               return tree.make<Stmt>(KeywordID::ALIAS, alias, valExpr);
            }

            UNREACHABLE;

            case KeywordID::CAST          : [[fallthrough]];
            case KeywordID::UP_CAST       : [[fallthrough]];
            case KeywordID::DYN_CAST      : [[fallthrough]];
            case KeywordID::BIT_CAST      : [[fallthrough]];
            case KeywordID::CONST_CAST    : {
               index<Expr> stmtContents = parseCast(tokIt++->keywordID());
               consumeEOS("bad cast statement");
               tree.getNode(stmtContents).upCast(NodeType::STMT);
               return (index<Stmt>)stmtContents;
            }

            case KeywordID::_NOT_A_KEYWORD: UNREACHABLE;
         }
      case TokenType::IDEN        : {
         index<Expr> stmtContents = parseExpr();
         if (tryConsumeOperator(OpID::LABEL_DELIM)) { //label
            return tree.make<Stmt>(OpID::LABEL_DELIM, stmtContents);
         }
         consumeEOS("invalid statement");
         tree[(index<astNode>)stmtContents].upCast(NodeType::STMT);
         return (index<Stmt>)stmtContents;
      }
      
      case TokenType::INT_NUM     : [[fallthrough]];
      case TokenType::REAL_NUM    : [[fallthrough]];
      case TokenType::OP          : [[fallthrough]];
      case TokenType::BLOCK_DELIM : [[fallthrough]];
      case TokenType::PTXT_SEG    : STMT_STARTS_WITH_VALUE: {
         index<Expr> stmtContents = parseExpr();
         consumeEOS("statement must end with EOS token");
         tree[(index<astNode>)stmtContents].upCast(NodeType::STMT);
         return (index<Stmt>)stmtContents;
      }

      case TokenType::PREPROC_INIT: ++tokIt; return parsePreprocStmt();
      case TokenType::PREPROC_EOS : ++tokIt; goto START_PARSE_STMT;
      case TokenType::EOS         : ++tokIt; return tree.make<Stmt>();
      case TokenType::ESC         : ++tokIt += 2; goto START_PARSE_STMT;

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

   auto eval = [&]() { //evaluate the subexpression on the top of the stacks
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

   while (tokIt < endtok) {
      switch (tokIt->type()) {
         case TokenType::NONE: UNREACHABLE;
         case TokenType::__OPLIKE: UNREACHABLE;

         case TokenType::PREPROC_EOS: ++tokIt; goto PARSE_EXPR_CONTINUE;
         case TokenType::ESC: tokIt += 2; goto PARSE_EXPR_CONTINUE;
         
         case TokenType::KEYWORD:{
            const KeywordID kw = tokIt->keywordID();
            if (kw == KeywordID::FUNC) {
               ++tokIt;
               operandStack.push_back(+parseFunction());
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (kw == KeywordID::LET) {
               ++tokIt;
               operandStack.push_back(+parseLetStmt());
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isValue(kw)) {
               operandStack.push_back(tree.getValueKeyword(kw));
               ++tokIt;
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isCast(kw)) {
               ++tokIt;
               operandStack.push_back(+parseCast(kw));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isC_FunctionLike(kw)) {
               ++tokIt;
               consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "bad function-like keyword use");
               index<ArgList> argList = parseArgList(BlockType::CALL);
               operandStack.push_back(+tree.make<Expr>(kw, argList));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isType(kw)) {
               operandStack.push_back(+tree.getFundType(kw));
               ++tokIt;
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            
            logError(ErrCode::BAD_KEYWORD, "bad keyword in expression");
         }

         case TokenType::IDEN: operandStack.push_back(+parseIdentifier()); prevTokIsOperand = true; goto PARSE_EXPR_CONTINUE;
         case TokenType::INT_NUM:
            operandStack.push_back(+tree.make<Literal>(tokIt++->intVal()));
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         case TokenType::REAL_NUM:
            operandStack.push_back(+tree.make<Literal>(tokIt++->realVal()));
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         case TokenType::PTXT_SEG:
            prevTokIsOperand = true;
            switch (tokIt->ptxtType()) {
               case PtxtType::CHAR:
                  operandStack.push_back(+tree.make<Literal>(tokIt++->charVal()));
                  goto PARSE_EXPR_CONTINUE;
               case PtxtType::UNPROCESSED_STR:
                  operandStack.push_back(+tree.make<Literal>(tokIt++->unprocessedStrVal()));
                  goto PARSE_EXPR_CONTINUE;

               default: logError(ErrCode::PARSER_NOT_IMPLEMENTED, "cannot currently parse this plaintext segment type (%u)", +tokIt->ptxtType());
               //!TODO: support other types of plaintext literal
            }
            UNREACHABLE;

         case TokenType::EOS: goto END_OF_EXPR;
         case TokenType::OP:
            if (tokIt->opID() == OpID::COMMA || tokIt->opID() == OpID::INLINE_ELSE) {
               goto END_OF_EXPR;
            }
            else if (tokIt->opID() == OpID::INLINE_IF) {
               ++tokIt;
               operatorStack.emplace_back(FMT(""), OpID::TERNARY_INVOKE, OpProps::NULL, (ubyte)0, TokenType::OP);
               index<Expr> trueVal = parseExpr();
               operandStack.push_back(+trueVal);
               consumeOperator(OpID::INLINE_ELSE, "bad ternary conditional expression");
               prevTokIsOperand = false;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (prevTokIsOperand) { //binary or postfix unary
               OpData op = tokIt->op();
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
               OpData op = tokIt->op();
               op.removeProps(OpProps::POSTFIX | OpProps::INFIX_LEFT | OpProps::INFIX_RIGHT);
               debug_assert(+(op.props() & OpProps::PREFIX));
               operatorStack.push_back(op);
            }
            prevTokIsOperand = false;
            ++tokIt;
            goto PARSE_EXPR_CONTINUE;
         case TokenType::BLOCK_DELIM: {
            BlockType blockType = tokIt->blockType();
            BlockDelimRole role = tokIt->blockDelimRole();
            if (!isOpener(role)) { goto END_OF_EXPR; }
            ++tokIt;
            if (prevTokIsOperand) { //function call, initializer list, subscript, or specializer
               debug_assert(operandStack.size());
               index<ArgList> args = parseArgList(blockType);
               operandStack.push_back(+tree.makeExpr(getInvoker(blockType),operandStack.pop_back(),(index<astNode>)args));
            } else if (blockType == BlockType::INIT_LIST) { //tuple
               operandStack.push_back(+parseArgList(blockType));
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
   while (tokIt->type() == TokenType::KEYWORD && isQualifier(tokIt->keywordID())) {
      quals |= toQual(tokIt->keywordID());
      ++tokIt;
   }
   return quals;
}

template<bool isDecl> clef::index<clef::Identifier> clef::Parser::tryParseIdentifier(index<Identifier> scopeName) {
   QualMask quals = parseQuals();
   //handle keywords
   if (tokIt->type() == TokenType::KEYWORD) {
      index<Identifier> keyword = tree.make<Identifier>(tokIt->keywordID(), index<SpecList>{}, quals);
      ++tokIt;
      if (tryConsumeOperator(OpID::SCOPE_RESOLUTION)) {
         logError(ErrCode::BAD_KEYWORD, "keywords may not name scopes");
      }
      return keyword;
   }

   //handle other identifiers
   if (tokIt->type() != TokenType::IDEN) { return 0; }

   index<Identifier> name = scopeName;

   do {
      name = tree.make<Identifier>(tokIt->name(), name);
      ++tokIt;

      if (tryConsumeBlockDelim(BlockType::SPECIALIZER, BlockDelimRole::OPEN)) {
         index<SpecList> specializer = parseSpecList<isDecl>(BlockType::SPECIALIZER);
         tree[name].specializer() = specializer;
      }

      if (!tryConsumeOperator(OpID::SCOPE_RESOLUTION)) { break; }

      if (tokIt->type() == TokenType::KEYWORD) { logError(ErrCode::BAD_IDEN, "keywords may not name or be members of scopes"); }
      else if (tokIt->type() != TokenType::IDEN) { logError(ErrCode::BAD_IDEN, "only identifiers may name or be members of scopes (%u)", +tokIt->type()); }
   } while (true);
   tree[name].setQualMask(quals);
   
   return name;
}
template<bool isDecl> clef::index<clef::Identifier> clef::Parser::parseIdentifier(index<Identifier> scopeName) {
   index<Identifier> name = tryParseIdentifier<isDecl>(scopeName);
   if (!name) {
      logError(ErrCode::BAD_IDEN, "expected an identifier");
   }
   return name;
}
template clef::index<clef::Identifier> clef::Parser::parseIdentifier<true>(index<Identifier>);
template clef::index<clef::Identifier> clef::Parser::parseIdentifier<false>(index<Identifier>);
template clef::index<clef::Identifier> clef::Parser::tryParseIdentifier<true>(index<Identifier>);
template clef::index<clef::Identifier> clef::Parser::tryParseIdentifier<false>(index<Identifier>);

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
   switch (tokIt->type()) {
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
   index<Variable> err = parseParam();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "CATCH block without closing parens");
   index<Scope> handler = parseProcedure();
   return tree.make<TryCatch>(procedure, err, handler);
}

clef::index<clef::Function> clef::Parser::parseFunction() {
   index<Identifier> name = tryParseIdentifier<true>();
   
   //params
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
   index<ParamList> params = parseParamList(BlockType::CALL);
   

   //return type
   consumeOperator(OpID::ARROW, "FUNC without trailing return type");
   index<Type> returnType = parseTypename();

   //make signature
   index<FuncSig> sig = tree.make<FuncSig>(returnType, params);

   if (tryConsumeEOS()) { //forward declaration
      if (name) {
         return tree.remake<Function>(name, sig, tree[name]);
      } else {
         return tree.make<Function>(sig);
      }
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FUNC definition");
   index<Scope> procedure = parseProcedure();

   //EOS
   consumeEOS("FUNC without EOS");

   //return
   if (name) {
      return tree.remake<Function>(name, sig, procedure, tree[name]);
   } else {
      return tree.make<Function>(sig, procedure);
   }
}

//!TODO: implement parseASM
clef::index<clef::Asm> clef::Parser::parseASM() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "inline assembly is not yet supported");
}
#endif //PARSER_CPP