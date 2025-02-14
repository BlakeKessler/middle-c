#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

void clef::Parser::parse(const SourceTokens& src, SyntaxTree& tree) {
   Parser parser{src, tree};
   while (parser.tokIt < parser.endtok) {
      parser.parseStmt();
   }
   // return parser.tree;
}

clef::index<clef::Stmt> clef::Parser::parseStmt() {
START_PARSE_STMT:
   switch (tokIt->type()) {
      case TokenType::NONE        : UNREACHABLE;
      
      case TokenType::KEYWORD     :
         switch (tokIt->keywordID()) {
            case KeywordID::VOID    : [[fallthrough]];
            case KeywordID::AUTO    : [[fallthrough]];
            default: {
               debug_assert(isType(tokIt->keywordID()));
               auto [var, decl] = parseVarDecl();
               tree.getNode(decl).anyCast(NodeType::STMT);
               return (index<Stmt>)decl;
               static_assert(mcsl::same_t<decltype(decl), index<Decl>>);
            } break;
            case KeywordID::NULLPTR : [[fallthrough]];
            case KeywordID::THIS    : [[fallthrough]];
            case KeywordID::SELF    : [[fallthrough]];
            case KeywordID::TRUE    : [[fallthrough]];
            case KeywordID::FALSE   :
               goto STMT_STARTS_WITH_VALUE;

            case KeywordID::ASM           : ++tokIt; return parseASM(); break;
            
            case KeywordID::CLASS         : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::CLASS), parseClass());
            case KeywordID::STRUCT        : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::STRUCT), parseStruct());
            case KeywordID::INTERFACE     : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::INTERFACE), parseInterface());
            case KeywordID::UNION         : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::UNION), parseUnion());
            case KeywordID::ENUM          : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::ENUM), parseEnum());
            case KeywordID::MASK          : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::MASK), parseMask());
            case KeywordID::NAMESPACE     : ++tokIt; return tree.make<Stmt, Decl>(tree.getFundType(KeywordID::NAMESPACE), parseNamespace());
            case KeywordID::FUNC          : {
               ++tokIt;
               index<Function> funcptr = parseFunction();
               return tree.make<Stmt, Decl>(tree[funcptr].signature(), funcptr);
            }

            
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

            case KeywordID::BREAK         : [[fallthrough]];
            case KeywordID::CONTINUE      : {
               ++tokIt;
               consumeEOS("bad nullary keyword (BREAK or CONTINUE)");
               return tree.make<Stmt>(KeywordID::CONTINUE);
            }

            case KeywordID::THROW         : [[fallthrough]];
            case KeywordID::ASSERT        : [[fallthrough]];
            case KeywordID::DEBUG_ASSERT  : [[fallthrough]];
            case KeywordID::STATIC_ASSERT : [[fallthrough]];
            case KeywordID::RETURN        : {
               ++tokIt;
               index<Expr> expr = parseExpr();
               consumeEOS("bad unary keyword (THROW, ASSERT, STATIC_ASSERT, or RETURN)");
               return tree.make<Stmt>(KeywordID::RETURN, expr);
            }

            case KeywordID::USING         : {
               ++tokIt;
               index<Identifier> alias = parseIdentifier();
               consumeOperator(OpID::ASSIGN, "alias definitions must use the assignment operator (and cannot be forward-declared)");
               index<Expr> valExpr = parseExpr();
               return tree.make<Stmt>(KeywordID::USING, alias, valExpr);
            }

            UNREACHABLE;

            case KeywordID::CAST          : [[fallthrough]];
            case KeywordID::UP_CAST       : [[fallthrough]];
            case KeywordID::DYN_CAST      : [[fallthrough]];
            case KeywordID::BIT_CAST      : [[fallthrough]];
            case KeywordID::CONST_CAST    : {
               index<Expr> stmtContents = parseExpr();
               consumeEOS("bad cast statement");
               tree.getNode(stmtContents).upCast(NodeType::STMT);
               return (index<Stmt>)stmtContents;
            }

            case KeywordID::_NOT_A_KEYWORD: UNREACHABLE;
         }
      case TokenType::IDEN        : {
         index<astNode> tmp = (index<astNode>)parseIdentifier();
         if (tokIt->type() == TokenType::IDEN) {
            tree[tmp].upCast(NodeType::TYPE);
            index<Decl> decl = parseDecl((index<Type>)tmp);
            tmp = (index<astNode>)decl;
         }
         index<Expr> stmtContents = parseExpr(tmp);
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
      if (isBinary(op)) { //!NOTE: PRIORITIZES BINARY OVER POSTFIX-UNARY
         if (!operandStack.size()) { logError(ErrCode::BAD_EXPR, "bad expression (missing LHS on stack)"); }
         lhs = operandStack.pop_back();
         operandStack.push_back(+tree.makeExpr(op.opID(), lhs, rhs));
      } else {
         debug_assert(+(op & OpProps::POSTFIX));
         operandStack.push_back(+tree.makeExpr(op.opID(), rhs));
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

               default: logError(ErrCode::PARSER_NOT_IMPLEMENTED, "cannot currently parse this plaintext segment type (%hhu)", +tokIt->ptxtType());
            }
            UNREACHABLE;

         case TokenType::EOS: goto END_OF_EXPR;
         case TokenType::OP:
            if (tokIt->opID() == OpID::COMMA) {
               goto END_OF_EXPR;
            }
            if (prevTokIsOperand) { //binary or postfix unary
               OpData op = tokIt->op();
               while (operatorStack.size() && precedence(operatorStack.back()) >= precedence(op)) { //!NOTE: UNFINISHED (does not consider associativity yet)
                  eval();
               }
               op.removeProps(OpProps::PREFIX);
               operatorStack.push_back(op);
            } else { //prefix unary
               OpData op = tokIt->op();
               op.removeProps(OpProps::POSTFIX | OpProps::INFIX_LEFT | OpProps::INFIX_RIGHT);
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
               //!TODO: push invoker to operator stack?
            } else if (blockType == BlockType::INIT_LIST) { //tuple
               operandStack.push_back(+parseArgList(blockType));
            } else { //block subexpression
               operandStack.push_back(+parseExpr());
               //!TODO: push invoker to operator stack?
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
      std::printf("%u excess operand(s)\n", operandStack.size() - 1);
      for (uint i = 0; i < operandStack.size(); ++i) { //!NOTE: this is for debugging - don't forget to remove
         tree[operandStack[i]].printf();
         std::printf("\n");
      }
      std::fflush(stdout);
      logError(ErrCode::BAD_EXPR, "invalid expression");
   }
   return toExpr(operandStack[0]);
}

clef::index<clef::Identifier> clef::Parser::tryParseIdentifier(index<Identifier> scopeName) {
   //handle keywords
   if (tokIt->type() == TokenType::KEYWORD) {
      index<Identifier> keyword = tree.make<Identifier>(tokIt->keywordID());
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
      if (!tryConsumeOperator(OpID::SCOPE_RESOLUTION)) { break; }

      if   (tokIt->type() == TokenType::KEYWORD) { logError(ErrCode::BAD_IDEN, "keywords may not name or be members of scopes"); }
      else if (tokIt->type() != TokenType::IDEN) { logError(ErrCode::BAD_IDEN, "only identifiers may name or be members of scopes (%hhu)", +tokIt->type()); }
   } while (true);
   
   return name;
}
clef::index<clef::Identifier> clef::Parser::parseIdentifier(index<Identifier> scopeName) {
   index<Identifier> name = tryParseIdentifier(scopeName);
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
   switch (tokIt->type()) {
      case TokenType::OP: { //basic ELSE
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
         
         index<Stmt> firstStmt = parseStmt();
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
   index<Decl> decl = parseDecl();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "CATCH block without closing parens");
   index<Scope> handler = parseProcedure();
   return tree.make<TryCatch>(procedure, decl, handler);
}

clef::index<clef::Function> clef::Parser::parseFunction() {
   index<Identifier> name = tryParseIdentifier();
   
   //params
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
   index<ParamList> params = parseParamList(BlockType::CALL);
   

   //return type
   consumeOperator(OpID::MEMBER_OF_POINTER_ACCESS, "FUNC without trailing return type");
   // TypeQualMask returnTypeQuals = parseQuals();
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

clef::index<clef::Asm> clef::Parser::parseASM() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "inline assembly is not yet supported");
}
#endif //PARSER_CPP