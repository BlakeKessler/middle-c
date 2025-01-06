#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "dyn_arr.hpp"

clef::Stmt* clef::Parser::parseStmt() {
   Stmt* stmt;
   Expr* expr = nullptr;

   TypeQualMask quals = parseQuals();
   Identifier* begin = parseIdentifier();
   if (!begin) {
      logError(ErrCode::BAD_STMT, "bad statement start");   
   }


   KeywordID keywordID = begin->keywordID();
   switch (keywordID) {
      default:
         debug_assert(isType(keywordID));
         [[fallthrough]];
      case KeywordID::THIS    : [[fallthrough]];
      case KeywordID::SELF    : [[fallthrough]];
      case KeywordID::VOID    : [[fallthrough]];
      case KeywordID::AUTO    : [[fallthrough]];
      case KeywordID::_NOT_A_KEYWORD:
         switch (tokIt->type()) {
            //!NOTE: UNFINISHED
         }
         //!NOTE: UNFINISHED
         break;

      // case KeywordID::ASM           : stmt = (Stmt*)parseASM(); break;
      
      case KeywordID::CLASS         : stmt = (Stmt*)parseClass(); break;
      case KeywordID::STRUCT        : stmt = (Stmt*)parseStruct(); break;
      case KeywordID::INTERFACE     : stmt = (Stmt*)parseInterface(); break;
      case KeywordID::UNION         : stmt = (Stmt*)parseUnion(); break;
      case KeywordID::ENUM          : stmt = (Stmt*)parseEnum(); break;
      case KeywordID::MASK          : stmt = (Stmt*)parseMask(); break;
      case KeywordID::NAMESPACE     : stmt = (Stmt*)parseNamespace(); break;

      case KeywordID::IF            : stmt = (Stmt*)parseIf(); break;
      case KeywordID::ELSE          : logError(ErrCode::BAD_KEYWORD, "floating ELSE");

      case KeywordID::FOR           : stmt = (Stmt*)parseForLoop(); break;
      case KeywordID::FOREACH       : stmt = (Stmt*)parseForeachLoop(); break;
      case KeywordID::WHILE         : stmt = (Stmt*)parseWhileLoop(); break;
      case KeywordID::DO            : stmt = (Stmt*)parseDoWhileLoop(); break;

      case KeywordID::SWITCH        : stmt = (Stmt*)parseSwitch(); break;
      case KeywordID::MATCH         : stmt = (Stmt*)parseMatch(); break;

      case KeywordID::BREAK         : [[fallthrough]];
      case KeywordID::CONTINUE      :
         consumeEOS("bad BREAK or CONTINUE");

         //!NOTE: UNFINISHED

         break;
      
      case KeywordID::CASE          : [[fallthrough]];
      case KeywordID::DEFAULT       : logError(ErrCode::BAD_KEYWORD, "floating CASE or DEFAULT");

      case KeywordID::GOTO          : //!NOTE: UNFINISHED

      case KeywordID::TRY           : stmt = (Stmt*)parseTryCatch(); break;
      case KeywordID::CATCH         : logError(ErrCode::BAD_KEYWORD, "floating CATCH");

      case KeywordID::THROW         : [[fallthrough]];
      case KeywordID::ASSERT        : [[fallthrough]];
      case KeywordID::STATIC_ASSERT : [[fallthrough]];
      case KeywordID::RETURN        : {
         expr = parseExpr();
         consumeEOS("bad THROW, ASSERT, STATIC_ASSERT, or RETURN");
         stmt = (Stmt*)tree.allocNode(NodeType::STMT);
         *stmt = Stmt{OpID::CALL_INVOKE, begin, expr};
         break;
      }

      case KeywordID::USING         : //!NOTE: UNFINISHED
   }

   return stmt;
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

   do {
      if (isOperand(tokIt->type())) { //push operand
         if (tokIt->type() == TokenType::CHAR || tokIt->type() == TokenType::IDEN || tokIt->type() == TokenType::STRT) {
            operandStack.push_back((astNode*)parseIdentifier());
         } else {
            operandStack.push_back((astNode*)parseNumLit());
         }
         prevTokIsOperand = true;
      } else if (isOperator(tokIt->type())) { //handle operator
         OpData op = OPERATORS[*tokIt];
         if (op == OpID::EOS || (op == OpID::COMMA && operatorStack.size())) { //operator that cannot be part of this type of expression
            break;
         }

         if (prevTokIsOperand) { //binary or postfix unary
            while (operatorStack.size() && precedence(operatorStack.back()) >= precedence(op)) { //!NOTE: UNFINISHED (does not consider associativity yet)
               eval();
            }
            op.removeProps(OpProps::PREFIX);
            operatorStack.push_back(op);
         } else { //prefix unary
            op.removeProps(OpProps::POSTFIX | OpProps::INFIX_LEFT | OpProps::INFIX_RIGHT);
            operatorStack.push_back(op);
         }
         prevTokIsOperand = false;
      } else if (isBlockLike(tokIt->type())) { //handle block delimiter
         OpData op = OPERATORS[*tokIt];
         if (isStringLike(op)) { //string and character literals
            switch (op.opID()) {
               case OpID::STRING : operandStack.push_back((astNode*)parseStringLit()); break;
               case OpID::CHAR   : operandStack.push_back((astNode*)parseCharLit()); break;
               default: std::unreachable();
            }
            prevTokIsOperand = true;
         }
         else { //blocks
            if (!isOpener(op)) { logError(ErrCode::BAD_BLOCK_DELIM, "floating closing block delimiter"); }
            ++tokIt;
            if (prevTokIsOperand) { //function call, initializer list, subscript, or specializer
               ArgList* args = parseArgList(getCloser(op));
               operandStack.push_back((astNode*)(new (tree.allocNode(NodeType::EXPR)) Expr{getInvoker(op),operandStack.pop_back(),(astNode*)args}));
            } else if (op == OpID::LIST_OPEN) { //tuple
               operandStack.push_back((astNode*)(parseArgList(getCloser(op))));
            } else { //block subexpression
               operandStack.push_back((astNode*)parseExpr());
               if (!isBlockLike(tokIt->type()) || getCloser(op) != OPERATORS[*tokIt]) { logError(ErrCode::BAD_BLOCK_DELIM, "bad block subexpression"); }
            }
            prevTokIsOperand = true;
         }
      } else {
         break;
      }
   } while (++tokIt < endtok);

   while (operatorStack.size()) { eval(); }
   assert(operandStack.size() == 1);
   return (Expr*)operandStack.back();
}

clef::Scope* clef::Parser::parseProcedure() {
   Scope* scope = (Scope*)tree.allocNode(NodeType::SCOPE);

   while (tokIt < endtok) {
      if (tryConsumeOperator(OpID::LIST_CLOSE)) {
         return scope;
      }
      scope->push_back(parseStmt());
   }

   //reached end of source without finding closing token
   logError(ErrCode::UNCLOSED_BLOCK, "unclosed procedure block");
}

clef::Identifier* clef::Parser::tryParseIdentifier(Identifier* scopeName) {
   if (!+(tokIt->type() & TokenType::CHAR)) { return nullptr; }

   Identifier* name = scopeName;
   const Token* tmp;

   do {
      name = new (tree.allocNode(NodeType::IDEN)) Identifier{*tokIt, name};
      tmp = ++tokIt;
   } while (!tryConsumeOperator(OpID::SCOPE_RESOLUTION) || !+(tokIt->type() & TokenType::CHAR));
   
   tokIt = tmp;
   return name;
}
clef::Identifier* clef::Parser::parseIdentifier(Identifier* scopeName) {
   if (!+(tokIt->type() & TokenType::CHAR)) { logError(ErrCode::BAD_IDEN, "bad IDENTIFIER"); }

   Identifier* name = scopeName;
   const Token* tmp;

   do {
      name = new (tree.allocNode(NodeType::IDEN)) Identifier{*tokIt, name};
      tmp = ++tokIt;
   } while (!tryConsumeOperator(OpID::SCOPE_RESOLUTION) || !+(tokIt->type() & TokenType::CHAR));
   
   if (tokIt != tmp || name == scopeName) { logError(ErrCode::BAD_IDEN, "bad IDENTIFIER"); }
   return name;
}
clef::Type* clef::Parser::parseTypename(Identifier* scopeName) {
   Type* name = (Type*)parseIdentifier(scopeName);
   ((astNode*)name)->upCast(NodeType::TYPE);
   return name;
}

clef::Decl* clef::Parser::parseDecl(Identifier* scopeName) {
   Type* type = parseTypename(scopeName);
   Identifier* name = parseIdentifier(scopeName);
   return new (tree.allocNode(NodeType::DECL)) Decl{type, name};
}
#pragma region loop

clef::Loop* clef::Parser::parseForLoop() {
   //open parens
   consumeOperator(OpID::CALL_OPEN, "FOR loop without opening parens for condition");

   //params
   Stmt* decl = parseStmt();
   Stmt* condition = parseStmt();
   Expr* inc = parseExpr();

   //closing parens
   consumeOperator(OpID::CALL_CLOSE, "FOR loop without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad FOR block");
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
   consumeOperator(OpID::CALL_OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   TypeQualMask quals = parseQuals();
   Type* itType = (Type*)parseIdentifier();
      ((astNode*)itType)->upCast(NodeType::TYPE);
   Identifier* itName = parseIdentifier();

   //IN operator
   consumeOperator(OpID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   Expr* target = parseExpr();

   //closing parens
   consumeOperator(OpID::CALL_CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad FOREACH block");
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
   consumeOperator(OpID::CALL_OPEN, "WHILE loop without opening parens for condition");

   //condition
   Expr* condition = parseExpr();

   //closing parens
   consumeOperator(OpID::CALL_CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad WHILE block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::WHILE, condition, proc};
}

clef::Loop* clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad DO WHILE block");
   Scope* proc = parseProcedure();

   //WHILE keyword
   consumeKeyword(KeywordID::WHILE, "DO WHILE without WHILE");

   //condition
   consumeOperator(OpID::CALL_OPEN, "DO WHILE loop without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OpID::CALL_CLOSE, "DO WHILE loop without closing parens for condition");

   //EOS
   consumeEOS("DO WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::DO_WHILE, condition, proc};
}

#pragma endregion loop

clef::If* clef::Parser::parseIf() {
   //condition
   consumeOperator(OpID::CALL_OPEN, "IF statement without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OpID::CALL_CLOSE, "IF statement without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad IF block");
   Scope* proc = parseProcedure();

   //EOS
   if (tryConsumeEOS()) {
      return new (tree.allocNode(NodeType::IF)) If{condition, proc};
   }

   //ELSE
   consumeKeyword(KeywordID::ELSE, "IF statement without EOS token");
   switch (tokIt->type()) {
      case TokenType::OP: { //basic ELSE
         consumeOperator(OpID::LIST_OPEN, "bad ELSE block");
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
   consumeOperator(OpID::CALL_OPEN, "SWITCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OpID::CALL_CLOSE, "SWITCH without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad SWITCH block");
   Scope* procedure = new (tree.allocNode(NodeType::SCOPE)) Scope{tree.allocBuf<Stmt*>()};
   SwitchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) SwitchCases{tree.allocBuf<mcsl::pair<Expr*,Stmt*>>(), procedure};

   while (!tryConsumeOperator(OpID::LIST_CLOSE)) {
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
   consumeOperator(OpID::CALL_OPEN, "MATCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OpID::CALL_CLOSE, "MATCH without closing parens for condition");

   //procedure
   consumeOperator(OpID::LIST_OPEN, "bad MATCH block");
   MatchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) MatchCases{tree.allocBuf<mcsl::pair<Expr*,Scope*>>()};

   while (!tryConsumeOperator(OpID::LIST_CLOSE)) {
      Expr* caseExpr;
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         caseExpr = parseExpr();
      } else {
         consumeKeyword(KeywordID::DEFAULT, "bad MATCH block procedure");
         caseExpr = nullptr;   
      }
      consumeOperator(OpID::LABEL_DELIM, "bad CASE or DEFAULT in MATCH statement");
      consumeOperator(OpID::LIST_OPEN, "bad CASE or DEFAULT procedure in MATCH statement");
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
   consumeOperator(OpID::CALL_OPEN, "CATCH block without opening parens");
   Decl* decl = parseDecl();
   consumeOperator(OpID::CALL_CLOSE, "CATCH block without closing parens");
   Scope* handler = parseProcedure();
   return new (tree.allocNode(NodeType::TRY_CATCH)) TryCatch{procedure, decl, handler};
}

clef::Function* clef::Parser::parseFunction() {
   Identifier* name = tryParseIdentifier();
   
   //params
   consumeOperator(OpID::CALL_OPEN, "FUNC without parameters");
   ParamList* params = new (tree.allocNode(NodeType::FUNC_SIG)) ParamList{tree.allocBuf<Variable*>()};
   if (!tryConsumeOperator(OpID::CALL_CLOSE)) {
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
      consumeOperator(OpID::CALL_CLOSE, "bad FUNC parameter list");
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
   consumeOperator(OpID::LIST_OPEN, "bad FUNC definition");
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
   consumeOperator(OpID::LIST_OPEN, "bad INTERFACE definition");
   while (!tryConsumeOperator(OpID::LIST_CLOSE)) {
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

   consumeOperator(OpID::LIST_OPEN, "bad UNION definition");

   ParameterList* members = new (tree.allocNode(NodeType::PARAM_LIST)) ParameterList{tree.allocBuf<Variable*>()};
   while (!tryConsumeOperator(OpID::LIST_CLOSE)) {
      //parse member
      Type* memberType = (Type*)parseIdentifier();
         ((astNode*)memberType)->upCast(NodeType::TYPE);
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
      baseType = (Type*)parseIdentifier();
         ((astNode*)baseType)->upCast(NodeType::TYPE);
   } else { baseType = nullptr; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous ENUM");
      }
      ((astNode*)name)->anyCast(NodeType::ENUM);
      return new (name) Enum{baseType};
   }

   consumeOperator(OpID::LIST_OPEN, "bad ENUM definition");
   ParamList* enumerators = new (tree.allocNode(NodeType::PARAM_LIST)) ParamList{tree.allocBuf<Variable*>()};
   if (!tryConsumeOperator(OpID::LIST_CLOSE)) {
      do {
         Identifier* enumerator = parseIdentifier();
         Expr* val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
         } else { val = nullptr; }

         ((astNode*)enumerator)->upCast(NodeType::VAR);
         enumerators->push_back(new (enumerator) Variable{baseType, enumerator, val});

      } while (tryConsumeOperator(OpID::COMMA));
      consumeOperator(OpID::LIST_CLOSE, "bad ENUM enumerator");
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