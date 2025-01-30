#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

clef::SyntaxTree clef::Parser::parse(const SourceTokens& src) {
   Parser parser{src};
   while (parser.tokIt < parser.endtok) {
      parser.parseStmt();
   }
   return parser.tree;
}

clef::Stmt* clef::Parser::parseStmt() {
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
               return (Stmt*)decl;
            } break;
            case KeywordID::NULLPTR : [[fallthrough]];
            case KeywordID::THIS    : [[fallthrough]];
            case KeywordID::SELF    : [[fallthrough]];
            case KeywordID::TRUE    : [[fallthrough]];
            case KeywordID::FALSE   :
               goto STMT_STARTS_WITH_VALUE;

            case KeywordID::ASM           : ++tokIt; return (Stmt*)parseASM(); break;
            
            case KeywordID::CLASS         : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::CLASS), parseClass()});
            case KeywordID::STRUCT        : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::STRUCT), parseStruct()});
            case KeywordID::INTERFACE     : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::INTERFACE), parseInterface()});
            case KeywordID::UNION         : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::UNION), parseUnion()});
            case KeywordID::ENUM          : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::ENUM), parseEnum()});
            case KeywordID::MASK          : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::MASK), parseMask()});
            case KeywordID::NAMESPACE     : ++tokIt; return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{tree.getFundType(KeywordID::NAMESPACE), parseNamespace()});
            case KeywordID::FUNC          : {
               ++tokIt;
               Function* funcptr = parseFunction();
               return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Decl{funcptr->signature(), funcptr});
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
                  Expr* caseExpr = parseExpr();
                  consumeEOS("bad GOTO CASE statement (missing EOS)");
                  return new (tree.allocNode(NodeType::STMT)) Stmt{OpID::GOTO_CASE, caseExpr};
               }
               //goto default
               if (tryConsumeKeyword(KeywordID::DEFAULT)) {
                  consumeEOS("bad GOTO DEFAULT statement (missing EOS)");
                  return new (tree.allocNode(NodeType::STMT)) Stmt{OpID::GOTO_CASE};
               }
               //goto
               Identifier* label = parseIdentifier();
               if (label->scopeName()) { logError(ErrCode::BAD_IDEN, "label may not be scoped"); }
               consumeEOS("bad GOTO statement (missing EOS)");
               return new (tree.allocNode(NodeType::STMT)) Stmt{OpID::GOTO, label};
            }

            case KeywordID::TRY           : ++tokIt; return parseTryCatch(); break;
            case KeywordID::CATCH         : logError(ErrCode::BAD_KEYWORD, "floating CATCH");

            case KeywordID::BREAK         : [[fallthrough]];
            case KeywordID::CONTINUE      : {
               const KeywordID kw = tokIt->keywordID();
               ++tokIt;
               consumeEOS("bad nullary keyword (BREAK or CONTINUE)");
               return new (tree.allocNode(NodeType::STMT)) Stmt{kw};
            }

            case KeywordID::THROW         : [[fallthrough]];
            case KeywordID::ASSERT        : [[fallthrough]];
            case KeywordID::DEBUG_ASSERT  : [[fallthrough]];
            case KeywordID::STATIC_ASSERT : [[fallthrough]];
            case KeywordID::RETURN        : {
               const KeywordID kw = tokIt->keywordID();
               ++tokIt;
               Expr* expr = parseExpr();
               consumeEOS("bad unary keyword (THROW, ASSERT, STATIC_ASSERT, or RETURN)");
               return new (tree.allocNode(NodeType::STMT)) Stmt{kw, expr};
            }

            case KeywordID::USING         : {
               ++tokIt;
               Identifier* alias = parseIdentifier();
               consumeOperator(OpID::ASSIGN, "alias definitions must use the assignment operator (and cannot be forward-declared)");
               Expr* valExpr = parseExpr();
               return (Stmt*)(new (tree.allocNode(NodeType::STMT)) Stmt{KeywordID::USING, alias, valExpr});
            }

            UNREACHABLE;

            case KeywordID::CAST          : [[fallthrough]];
            case KeywordID::UP_CAST       : [[fallthrough]];
            case KeywordID::DYN_CAST      : [[fallthrough]];
            case KeywordID::BIT_CAST      : [[fallthrough]];
            case KeywordID::CONST_CAST    : {
               Expr* stmtContents = parseExpr();
               consumeEOS("bad cast statement");
               ((astNode*)stmtContents)->upCast(NodeType::STMT);
               return (Stmt*)stmtContents;
            }

            case KeywordID::_NOT_A_KEYWORD: UNREACHABLE;
         }
      case TokenType::IDEN        : {
         astNode* tmp = (astNode*)parseIdentifier();
         if (tokIt->type() == TokenType::IDEN) {
            tmp->upCast(NodeType::TYPE);
            Decl* decl = parseDecl((Type*)tmp);
            tmp = (astNode*)decl;
         }
         Expr* stmtContents = parseExpr(tmp);
         consumeEOS("invalid statement");
         ((astNode*)stmtContents)->upCast(NodeType::STMT);
         return (Stmt*)stmtContents;
      }
      
      case TokenType::INT_NUM     : [[fallthrough]];
      case TokenType::REAL_NUM    : [[fallthrough]];
      case TokenType::OP          : [[fallthrough]];
      case TokenType::BLOCK_DELIM : [[fallthrough]];
      case TokenType::PTXT_SEG    : STMT_STARTS_WITH_VALUE: {
         Expr* stmtContents = parseExpr();
         consumeEOS("statement must end with EOS token");
         ((astNode*)stmtContents)->upCast(NodeType::STMT);
         return (Stmt*)stmtContents;
      }

      case TokenType::PREPROC_INIT: ++tokIt; return parsePreprocStmt();
      case TokenType::PREPROC_EOS : ++tokIt; goto START_PARSE_STMT;
      case TokenType::EOS         : ++tokIt; return new (tree.allocNode(NodeType::STMT)) Stmt{};
      case TokenType::ESC         : ++tokIt += 2; goto START_PARSE_STMT;

      default: UNREACHABLE;
   }
   UNREACHABLE;
}

clef::Expr* clef::Parser::parseExpr(astNode* initOperand) {
   Expr* expr = parseExprNoPrimaryComma(initOperand);
   while (tryConsumeOperator(OpID::COMMA)) {
      expr = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::COMMA, expr, parseExprNoPrimaryComma()};
   }
   return expr;
}
clef::Expr* clef::Parser::parseExprNoPrimaryComma(astNode* initOperand) {
   mcsl::dyn_arr<OpData> operatorStack;
   mcsl::dyn_arr<astNode*> operandStack;
   bool prevTokIsOperand = false;

   if (initOperand) {
      operandStack.push_back(initOperand);
   }

   auto eval = [&]() { //evaluate the subexpression on the top of the stacks
      OpData op = operatorStack.pop_back();
      if (!operandStack.size()) { logError(ErrCode::BAD_EXPR, "bad expression (missing RHS on stack)"); }
      astNode* rhs = operandStack.pop_back();
      astNode* lhs;
      if (isBinary(op)) { //!NOTE: PRIORITIZES BINARY OVER POSTFIX-UNARY
         if (!operandStack.size()) { logError(ErrCode::BAD_EXPR, "bad expression (missing LHS on stack)"); }
         lhs = operandStack.pop_back();
      } else { debug_assert(+(op & OpProps::POSTFIX)); lhs = nullptr; }

      operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::EXPR)) Expr{op.opID(), lhs, rhs}));
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
               operandStack.push_back((astNode*)parseFunction());
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isValue(kw)) {
               operandStack.push_back(tree.getValueKeyword(kw));
               ++tokIt;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isCast(kw)) {
               ++tokIt;
               operandStack.push_back((astNode*)parseCast(kw));
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isC_FunctionLike(kw)) {
               ++tokIt;
               consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "bad function-like keyword use");
               ArgList* argList = parseArgList(BlockType::CALL);
               operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::EXPR)) Expr{kw, argList}));
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isType(kw)) {
               operandStack.push_back((astNode*)(tree.getFundType(kw)));
               ++tokIt;
               goto PARSE_EXPR_CONTINUE;
            }
            
            logError(ErrCode::BAD_KEYWORD, "bad keyword in expression");
         }

         case TokenType::IDEN: operandStack.push_back((astNode*)parseIdentifier()); goto PARSE_EXPR_CONTINUE;
         case TokenType::INT_NUM:
            operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::LITERAL)) Literal{tokIt++->intVal()}));
            goto PARSE_EXPR_CONTINUE;
         case TokenType::REAL_NUM:
            operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::LITERAL)) Literal{tokIt++->realVal()}));
            goto PARSE_EXPR_CONTINUE;
         case TokenType::PTXT_SEG:
            switch (tokIt->ptxtType()) {
               case PtxtType::CHAR:
                  operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::LITERAL)) Literal{tokIt++->charVal()}));
                  goto PARSE_EXPR_CONTINUE;
               case PtxtType::UNPROCESSED_STR:
                  operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::LITERAL)) Literal{tokIt++->unprocessedStrVal()}));
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
               ArgList* args = parseArgList(blockType);
               operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::EXPR)) Expr{getInvoker(blockType),operandStack.pop_back(),(astNode*)args}));
            } else if (blockType == BlockType::INIT_LIST) { //tuple
               operandStack.push_back((astNode*)(parseArgList(blockType)));
            } else { //block subexpression
               operandStack.push_back((astNode*)parseExpr());
               consumeBlockDelim(blockType, BlockDelimRole::CLOSE, "bad block subexpression");
            }
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
         operandStack[i]->print();
         std::printf("\n");
         std::fflush(stdout);
      }
      logError(ErrCode::BAD_EXPR, "invalid expression");
   }
   return (Expr*)operandStack.back(); //!NOTE: WILL PROBABLY CAUSE ISSUES (not all AST nodes have the same memory layout as expressions)
}

clef::Identifier* clef::Parser::tryParseIdentifier(Identifier* scopeName) {
   //handle keywords
   if (tokIt->type() == TokenType::KEYWORD) {
      Identifier* keyword = new (tree.allocNode(NodeType::KEYWORD)) Identifier{tokIt->keywordID()};
      ++tokIt;
      if (tryConsumeOperator(OpID::SCOPE_RESOLUTION)) {
         logError(ErrCode::BAD_KEYWORD, "keywords may not name scopes");
      }
      return keyword;
   }

   //handle other identifiers
   if (tokIt->type() != TokenType::IDEN) { return nullptr; }

   Identifier* name = scopeName;

   do {
      name = new (tree.allocNode(NodeType::IDEN)) Identifier{tokIt->name(), name};
      
      ++tokIt;
      if (!tryConsumeOperator(OpID::SCOPE_RESOLUTION)) { break; }

      if   (tokIt->type() == TokenType::KEYWORD) { logError(ErrCode::BAD_IDEN, "keywords may not name or be members of scopes"); }
      else if (tokIt->type() != TokenType::IDEN) { logError(ErrCode::BAD_IDEN, "only identifiers may name or be members of scopes (%hhu)", +tokIt->type()); }
   } while (true);
   
   return name;
}
clef::Identifier* clef::Parser::parseIdentifier(Identifier* scopeName) {
   Identifier* name = tryParseIdentifier(scopeName);
   if (!name) {
      logError(ErrCode::BAD_IDEN, "expected an identifier");
   }
   return name;
}

clef::If* clef::Parser::parseIf() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "IF statement without opening parens for condition");
   Expr* condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "IF statement without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad IF block");
   Scope* proc = parseProcedure();

   //EOS
   if (tryConsumeEOS()) {
      return new (tree.allocNode(NodeType::IF)) If{condition, proc};
   }

   //ELSE
   consumeKeyword(KeywordID::ELSE, "IF statement without EOS token");
   switch (tokIt->type()) {
      case TokenType::OP: { //basic ELSE
         consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ELSE block");
         Scope* proc = parseProcedure();
         //EOS
         consumeEOS("DO WHILE statement without EOS token");

         //return
         If* elseStmt = new (tree.allocNode(NodeType::IF)) If{nullptr, proc, nullptr};
         If* ifStmt = new (tree.allocNode(NodeType::IF)) If{condition, proc, elseStmt};
         return ifStmt;
      }
      default: { //ELSE IF
         consumeKeyword(KeywordID::IF, "bad ELSE IF block");
         If* elifStmt = parseIf(); //will consume EOS

         //return
         If* ifStmt = new (tree.allocNode(NodeType::IF)) If{condition, proc, elifStmt};
         return ifStmt;
      }
   }
}

clef::Switch* clef::Parser::parseSwitch() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "SWITCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "SWITCH without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad SWITCH block");
   Scope* procedure = new (tree.allocNode(NodeType::SCOPE)) Scope{tree.allocBuf<Stmt*>()};
   SwitchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) SwitchCases{tree.allocBuf<mcsl::pair<Expr*,Stmt*>>(), procedure};

   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         Expr* caseExpr = parseExpr();
         consumeOperator(OpID::LABEL_DELIM, "bad CASE in SWITCH statement");
         
         Stmt* firstStmt = parseStmt();
         procedure->push_back(firstStmt);
         cases->push_back({caseExpr, firstStmt});

      } else if (tryConsumeKeyword(KeywordID::DEFAULT)) { //DEFAULT
         consumeOperator(OpID::LABEL_DELIM, "bad DEFAULT in SWITCH statement");
         
         Stmt* firstStmt = parseStmt();
         procedure->push_back(firstStmt);
         cases->push_back({nullptr, firstStmt});

      } else { //standard statement
         procedure->push_back(parseStmt());
      }
   }

   //EOS
   consumeEOS("SWITCH statement without EOS");

   //return
   return new (tree.allocNode(NodeType::SWITCH)) Switch{condition, cases};
}

clef::Match* clef::Parser::parseMatch() {
   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "MATCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "MATCH without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad MATCH block");
   MatchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) MatchCases{tree.allocBuf<mcsl::pair<Expr*,Scope*>>()};

   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      Expr* caseExpr;
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         caseExpr = parseExpr();
      } else {
         consumeKeyword(KeywordID::DEFAULT, "bad MATCH block procedure");
         caseExpr = nullptr;   
      }
      consumeOperator(OpID::LABEL_DELIM, "bad CASE or DEFAULT in MATCH statement");
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CASE or DEFAULT procedure in MATCH statement");
      Scope* procedure = parseProcedure();
      
      cases->emplace_back(caseExpr, procedure);
   }

   //EOS
   consumeEOS("MATCH statement without EOS");

   //return
   return new (tree.allocNode(NodeType::MATCH)) Match{condition, cases};
}

clef::TryCatch* clef::Parser::parseTryCatch() {
   Scope* procedure = parseProcedure();
   consumeKeyword(KeywordID::CATCH, "TRY block without CATCH block");
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "CATCH block without opening parens");
   Decl* decl = parseDecl();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "CATCH block without closing parens");
   Scope* handler = parseProcedure();
   return new (tree.allocNode(NodeType::TRY_CATCH)) TryCatch{procedure, decl, handler};
}

clef::Function* clef::Parser::parseFunction() {
   Identifier* name = tryParseIdentifier();
   
   //params
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FUNC without parameters");
   ParamList* params = parseParamList(BlockType::CALL);
   

   //return type
   consumeOperator(OpID::MEMBER_OF_POINTER_ACCESS, "FUNC without trailing return type");
   TypeQualMask returnTypeQuals = parseQuals();
   Type* returnType = parseTypename();

   //make signature
   FuncSig* sig = new (tree.allocNode(NodeType::FUNC_SIG)) FuncSig{returnType, params};

   if (tryConsumeEOS()) { //forward declaration
      if (name) {
         Function* func = (Function*)name;
         ((astNode*)func)->upCast(NodeType::FUNC);
         return new (func) Function{sig, name};
      } else {
         return new (tree.allocNode(NodeType::FUNC)) Function{sig};
      }
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FUNC definition");
   Scope* procedure = parseProcedure();

   //EOS
   consumeEOS("FUNC without EOS");

   //return
   if (name) {
      Function* func = (Function*)name;
      ((astNode*)func)->upCast(NodeType::FUNC);
      return new (func) Function{sig, procedure, name};
   } else {
      return new (tree.allocNode(NodeType::FUNC)) Function{sig, procedure};
   }
}

clef::Asm* clef::Parser::parseASM() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "inline assembly is not yet supported");
}
#endif //PARSER_CPP