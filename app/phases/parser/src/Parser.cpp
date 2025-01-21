#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

clef::Stmt* clef::Parser::parseStmt() {
START_PARSE_STMT:


   switch (tokIt->type()) {
      case TokenType::NONE        : UNREACHABLE;
      
      case TokenType::KEYWORD     :
         switch (tokIt->keywordID()) {
            case KeywordID::VOID    : [[fallthrough]];
            case KeywordID::AUTO    : [[fallthrough]];
            default:
               debug_assert(isType(tokIt->keywordID()));
               Variable* var = parseVariable();
               //!NOTE: UNFINISHED
               break;

            case KeywordID::NULLPTR : [[fallthrough]];
            case KeywordID::THIS    : [[fallthrough]];
            case KeywordID::SELF    :
               goto STMT_STARTS_WITH_VALUE;

            // case KeywordID::ASM           : ++tokIt; return parseASM(); break;
            
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
            case KeywordID::STATIC_ASSERT : [[fallthrough]];
            case KeywordID::RETURN        : {
               const KeywordID kw = tokIt->keywordID();
               ++tokIt;
               Expr* expr = parseExpr();
               consumeEOS("bad unary keyword (THROW, ASSERT, STATIC_ASSERT, or RETURN)");
               return new (tree.allocNode(NodeType::STMT)) Stmt{kw, expr};
            }

            case KeywordID::USING         : break; //!NOTE: UNFINISHED

            case KeywordID::_NOT_A_KEYWORD: UNREACHABLE;
         }
      case TokenType::IDEN        : break; //!NOTE: UNFINISHED
      
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
}

clef::Expr* clef::Parser::parseExpr() {
   Expr* expr = parseExprNoPrimaryComma();
   while (tryConsumeOperator(OpID::COMMA)) {
      expr = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::COMMA, expr, parseExprNoPrimaryComma()};
   }
   return expr;
}

clef::Expr* clef::Parser::parseExprNoPrimaryComma() {
   mcsl::dyn_arr<OpData> operatorStack;
   mcsl::dyn_arr<astNode*> operandStack;
   bool prevTokIsOperand = false;

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
               operandStack.push_back((astNode*)tree.getValueKeyword(kw));
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
         case TokenType::INT_NUM: //!NOTE: UNFINISHED
         case TokenType::REAL_NUM: //!NOTE: UNFINISHED
         case TokenType::PTXT_SEG: //!NOTE: UNFINISHED

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
      logError(ErrCode::BAD_EXPR, "invalid expression");
   }
   return (Expr*)operandStack.back(); //!NOTE: WILL PROBABLY CAUSE ISSUES (not all AST nodes have the same memory layout as expressions)
}

clef::Scope* clef::Parser::parseProcedure() {
   Scope* scope = (Scope*)tree.allocNode(NodeType::SCOPE);

   while (tokIt < endtok) {
      if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
         return scope;
      }
      scope->push_back(parseStmt());
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
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
clef::Type* clef::Parser::parseTypename(Identifier* scopeName) {
   Type* name = (Type*)parseIdentifier(scopeName);
   ((astNode*)name)->upCast(NodeType::TYPE);
   return name;
}

clef::Decl* clef::Parser::parseDecl(Identifier* scopeName) {
   if (tryConsumeKeyword(KeywordID::FUNC)) {
      Function* func = parseFuncDecl(scopeName);
      return new (tree.allocNode(NodeType::DECL)) Decl{func->signature(), func};
   }

   Type* type = parseTypename(scopeName);

   Identifier* name = parseIdentifier(scopeName);
   return new (tree.allocNode(NodeType::DECL)) Decl{type, name};
}

clef::Function* clef::Parser::parseFuncDecl(Identifier* scopeName) {
   Identifier* name = parseIdentifier(scopeName);
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "function definition must include parameter list");
   ParamList* params = parseParamList(BlockType::CALL);
   consumeOperator(OpID::ARROW, "function definitions must provide a return type");
   Type* returnType = parseTypename(scopeName);

   FuncSig* sig = new (tree.allocNode(NodeType::FUNC_SIG)) FuncSig{returnType, params};
   return new (tree.allocNode(NodeType::FUNC)) Function{sig, nullptr, name};
}

clef::Variable* clef::Parser::parseVariable(Identifier* scopeName) {
   Decl* decl = parseDecl(scopeName);
   if (tryConsumeEOS()) {
      Variable* var = new (decl) Variable{decl->type(), decl->name()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeOperator(OpID::ASSIGN)) {
      Variable* var = new (decl) Variable{decl->type(), decl->name(), parseExpr()};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::INIT_LIST);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::LIST_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   if (tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN)) {
      ArgList* args = parseArgList(BlockType::CALL);
      Expr* invoke = new (tree.allocNode(NodeType::EXPR)) Expr{OpID::CALL_INVOKE, decl->type(), args};
      Variable* var = new (decl) Variable{decl->type(), decl->name(), invoke};
         ((astNode*)var)->anyCast(NodeType::VAR);
      return var;
   }
   logError(ErrCode::BAD_DECL, "bad variable definition");
}

#pragma region loop

clef::Loop* clef::Parser::parseForLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOR loop without opening parens for condition");

   //params
   Stmt* decl = parseStmt();
   Stmt* condition = parseStmt();
   Expr* inc = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOR loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOR block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("FOR statement without EOS token");

   //return
   ForLoopParams* params = new (tree.allocNode(NodeType::FOREACH_LOOP_PARAMS)) ForLoopParams{decl, condition, inc};
   Loop* loop = new (tree.allocNode(NodeType::LOOP)) Loop{params, proc};

   return loop;
}

clef::Loop* clef::Parser::parseForeachLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   TypeQualMask quals = parseQuals();
   Type* itType = parseTypename();
   Identifier* itName = parseIdentifier();

   //IN operator
   consumeOperator(OpID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   Expr* target = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOREACH block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("FOREACH statement without EOS token");

   //return
   Decl* decl = new (tree.allocNode(NodeType::DECL)) Decl{itType, itName};
   ForeachLoopParams* params = new (tree.allocNode(NodeType::FOREACH_LOOP_PARAMS)) ForeachLoopParams{decl, target};
   Loop* loop = new (tree.allocNode(NodeType::LOOP)) Loop{params, proc};

   return loop;
}

clef::Loop* clef::Parser::parseWhileLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "WHILE loop without opening parens for condition");

   //condition
   Expr* condition = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad WHILE block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::WHILE, condition, proc};
}

clef::Loop* clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad DO WHILE block");
   Scope* proc = parseProcedure();

   //WHILE keyword
   consumeKeyword(KeywordID::WHILE, "DO WHILE without WHILE");

   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "DO WHILE loop without opening parens for condition");
   Expr* condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "DO WHILE loop without closing parens for condition");

   //EOS
   consumeEOS("DO WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::DO_WHILE, condition, proc};
}

#pragma endregion loop

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
         
         //!NOTE: UNFINISHED

      } else if (tryConsumeKeyword(KeywordID::DEFAULT)) { //DEFAULT
         consumeOperator(OpID::LABEL_DELIM, "bad DEFAULT in SWITCH statement");
         
         //!NOTE: UNFINISHED

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
   ParamList* params = new (tree.allocNode(NodeType::FUNC_SIG)) ParamList{tree.allocBuf<Variable*>()};
   if (!tryConsumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE)) {
      do {
         //parse parameter
         TypeQualMask quals = parseQuals();
         Type* paramType = parseTypename();
         Identifier* paramName = tryParseIdentifier();
         Expr* defaultVal;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            defaultVal = parseExprNoPrimaryComma();
            if (!defaultVal) { logError(ErrCode::BAD_FUNC, "invalid FUNC parameter default value"); }
         } else { defaultVal = nullptr; }

         //push to parameter list
         ((astNode*)paramName)->upCast(NodeType::VAR);
         params->push_back(new (paramName) Variable{paramType, paramName, defaultVal});
      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "bad FUNC parameter list");
   }
   

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

#pragma region objectType

//!NOTE: clef::Parser::parseStruct temporarily relies on this function
clef::Class* clef::Parser::parseClass() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->upCast(NodeType::CLASS);
      return new (name) Class{(Type*)name};
   }

   //inheritance (including implemented interfaces)
   ObjTypeSpec* spec = new (tree.allocInterfaceSpec()) ObjTypeSpec{};
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         Type* parentType = parseTypename();
         spec->inheritedTypes().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() == TokenType::KEYWORD) {
         switch (tokIt->keywordID()) {
            case KeywordID::CLASS    : ++tokIt; spec->memberTypes().push_back(parseClass()); break;
            case KeywordID::STRUCT   : ++tokIt; spec->memberTypes().push_back(parseStruct()); break;
            case KeywordID::INTERFACE: ++tokIt; spec->memberTypes().push_back(parseInterface()); break;
            case KeywordID::UNION    : ++tokIt; spec->memberTypes().push_back(parseUnion()); break;
            case KeywordID::ENUM     : ++tokIt; spec->memberTypes().push_back(parseEnum()); break;
            case KeywordID::MASK     : ++tokIt; spec->memberTypes().push_back(parseMask()); break;
            case KeywordID::NAMESPACE: ++tokIt; spec->memberTypes().push_back(parseNamespace()); break;
            case KeywordID::FUNC     : ++tokIt; spec->methods().push_back(parseFunction()); break; //!NOTE: does not account for static functions
            default: goto MEMB_VAR_DECL;
         }
         continue;
      }
      MEMB_VAR_DECL:
      spec->members().push_back(parseVariable()); //!NOTE: does not account for static functions
   }

   //EOS
   consumeEOS("CLASS without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::CLASS);
   return new (name) Class{spec, (Type*)name};
}

//!NOTE: temporarily relies on clef::Parser::parseClass
clef::Struct* clef::Parser::parseStruct() {
   Class* classptr = parseClass();
   ((astNode*)classptr)->anyCast(NodeType::STRUCT);
   return (Struct*)classptr;
}

clef::Interface* clef::Parser::parseInterface() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->upCast(NodeType::INTERFACE);
      return new (name) Interface{(Type*)name};
   }
   
   //inheritance
   InterfaceSpec* spec = new (tree.allocInterfaceSpec()) InterfaceSpec{};
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         Interface* parentType = (Interface*)parseIdentifier();
            ((astNode*)parentType)->upCast(NodeType::INTERFACE);
         spec->inheritedInterfaces().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad INTERFACE definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      TypeQualMask quals = parseQuals();
      consumeKeyword(KeywordID::FUNC, "INTERFACE can only contain functions");
      Function* func = parseFunction();
      if (+(quals & TypeQualMask::STATIC)) {
         spec->staticFuncs().push_back(func);
      } else {
         spec->methods().push_back(func);
      }
   }

   //EOS
   consumeEOS("INTERFACE without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::INTERFACE);
   return new (name) Interface{spec, (Type*)name};
}

clef::Union* clef::Parser::parseUnion() {
   Identifier* name = tryParseIdentifier();

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous UNION");
      }
      ((astNode*)name)->upCast(NodeType::UNION);
      return new (name) Union{name};
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad UNION definition");

   ParameterList* members = new (tree.allocNode(NodeType::PARAM_LIST)) ParameterList{tree.allocBuf<Variable*>()};
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //parse member
      Type* memberType = parseTypename();
      Identifier* memberName = parseIdentifier();
      consumeEOS("bad UNION member");
      
      //push to members list
      Variable* member = (Variable*)memberName;
      ((astNode*)member)->upCast(NodeType::VAR);
      member->type() = memberType;
      members->push_back(member);
   }

   //EOS
   consumeEOS("UNION without EOS");

   //return
   if (name) { ((astNode*)name)->upCast(NodeType::UNION); }
   return new (name) Union{name, members};
}

//!NOTE: clef::Parser::parseMask relies on this function - be careful changing observable behavior
clef::Enum* clef::Parser::parseEnum() {
   Identifier* name = tryParseIdentifier();
   Type* baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename();
   } else { baseType = nullptr; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous ENUM");
      }
      ((astNode*)name)->anyCast(NodeType::ENUM);
      return new (name) Enum{baseType};
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ENUM definition");
   ParamList* enumerators = new (tree.allocNode(NodeType::PARAM_LIST)) ParamList{tree.allocBuf<Variable*>()};
   if (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      do {
         Identifier* enumerator = parseIdentifier();
         Expr* val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
         } else { val = nullptr; }

         ((astNode*)enumerator)->upCast(NodeType::VAR);
         enumerators->push_back(new (enumerator) Variable{baseType, enumerator, val});

      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE, "bad ENUM enumerator");
   }
   
   //EOS
   consumeEOS("ENUM without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::ENUM);
   return new (name) Enum{(Type*)name, baseType, enumerators};
}

//!NOTE: ASSUMES THAT THE SYNTAX AND MEMORY LAYOUT OF THE AST NODES FOR MASKS AND ENUMS ARE IDENTICAL
clef::Mask* clef::Parser::parseMask() {
   astNode* mask = (astNode*)parseEnum();
   mask->anyCast(NodeType::MASK);
   return (Mask*)mask;
}

#pragma endregion objectType
#endif //PARSER_CPP