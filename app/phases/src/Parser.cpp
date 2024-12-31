#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "InterfaceSpec.hpp"

clef::Stmt* clef::Parser::parseStmt() {
   Stmt* stmt;
   Expr* expr = nullptr;

   TypeQualMask quals = parseQuals();
   Identifier* begin = parseIdentifier(); //might just be a standard identifier
   if (!begin) {
      logError(ErrCode::PARSER_UNSPEC, "bad statement start");   
   }


   KeywordID keywordID = begin->keywordID();
   switch (keywordID) {
      default:
         assert(isType(keywordID));
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

      case KeywordID::ASM           : stmt = (Stmt*)parseASM(); break;
      
      case KeywordID::CLASS         : stmt = (Stmt*)parseClass(); break;
      case KeywordID::STRUCT        : stmt = (Stmt*)parseStruct(); break;
      case KeywordID::INTERFACE     : stmt = (Stmt*)parseInterface(); break;
      case KeywordID::UNION         : stmt = (Stmt*)parseUnion(); break;
      case KeywordID::ENUM          : stmt = (Stmt*)parseEnum(); break;
      case KeywordID::MASK          : stmt = (Stmt*)parseMask(); break;
      case KeywordID::NAMESPACE     : stmt = (Stmt*)parseNamespace(); break;

      case KeywordID::IF            : stmt = (Stmt*)parseIf(); break;
      case KeywordID::ELSE          : logError(ErrCode::PARSER_UNSPEC, "floating ELSE");

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
      case KeywordID::DEFAULT       : logError(ErrCode::PARSER_UNSPEC, "floating CASE or DEFAULT");

      case KeywordID::GOTO          : //!NOTE: UNFINISHED

      case KeywordID::TRY           : stmt = (Stmt*)parseTryCatch(); break;
      case KeywordID::CATCH         : logError(ErrCode::PARSER_UNSPEC, "floating CATCH");

      case KeywordID::THROW         : [[fallthrough]];
      case KeywordID::ASSERT        : [[fallthrough]];
      case KeywordID::STATIC_ASSERT : [[fallthrough]];
      case KeywordID::RETURN        : {
         expr = parseExpr();
         consumeEOS("bad THROW, ASSERT, STATIC_ASSERT, or RETURN");
         stmt = (Stmt*)tree.allocNode(NodeType::STMT);
         *stmt = Stmt{OperatorID::CALL, begin, expr};
         break;
      }

      case KeywordID::USING         : //!NOTE: UNFINISHED
   }

   return stmt;
}

clef::Scope* clef::Parser::parseProcedure() {
   Scope* scope = (Scope*)tree.allocNode(NodeType::SCOPE);

   while (tokIt < endtok) {
      if (tryConsumeOperator(OperatorID::LIST_CLOSE)) {
         return scope;
      }
      scope->push_back(parseStmt());
   }

   //reached end of source without finding closing token
   logError(ErrCode::PARSER_UNSPEC, "unclosed procedure");
}

#pragma region loop

clef::Loop* clef::Parser::parseForLoop() {
   //open parens
   consumeOperator(OperatorID::CALL_OPEN, "FOR loop without opening parens for condition");

   //params
   Stmt* decl = parseStmt();
   Stmt* condition = parseStmt();
   Expr* inc = parseExpr();

   //closing parens
   consumeOperator(OperatorID::CALL_CLOSE, "FOR loop without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad FOR block");
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
   consumeOperator(OperatorID::CALL_OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   TypeQualMask quals = parseQuals();
   Type* itType = (Type*)parseIdentifier();
      ((astNode*)itType)->downCast(NodeType::TYPE);
   Identifier* itName = parseIdentifier();

   //IN operator
   consumeOperator(OperatorID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   Expr* target = parseExpr();

   //closing parens
   consumeOperator(OperatorID::CALL_CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad FOREACH block");
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
   consumeOperator(OperatorID::CALL_OPEN, "WHILE loop without opening parens for condition");

   //condition
   Expr* condition = parseExpr();

   //closing parens
   consumeOperator(OperatorID::CALL_CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad WHILE block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OperatorID::WHILE, condition, proc};
}

clef::Loop* clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad DO WHILE block");
   Scope* proc = parseProcedure();

   //WHILE keyword
   consumeKeyword(KeywordID::WHILE, "DO WHILE without WHILE");

   //condition
   consumeOperator(OperatorID::CALL_OPEN, "DO WHILE loop without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OperatorID::CALL_CLOSE, "DO WHILE loop without closing parens for condition");

   //EOS
   consumeEOS("DO WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OperatorID::DO_WHILE, condition, proc};
}

#pragma endregion loop

clef::If* clef::Parser::parseIf() {
   //condition
   consumeOperator(OperatorID::CALL_OPEN, "IF statement without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OperatorID::CALL_CLOSE, "IF statement without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad IF block");
   Scope* proc = parseProcedure();

   //EOS
   if (tryConsumeEOS()) {
      return new (tree.allocNode(NodeType::IF)) If{condition, proc};
   }

   //ELSE
   consumeKeyword(KeywordID::ELSE, "IF statement without EOS token");
   switch (tokIt->type()) {
      case TokenType::OP: //basic ELSE
         consumeOperator(OperatorID::LIST_OPEN, "bad ELSE block");
         Scope* proc = parseProcedure();
         //EOS
         consumeEOS("DO WHILE statement without EOS token");

         //return
         If* ifStmt = new (tree.allocNode(NodeType::IF)) If{condition, proc};
         Else* elseStmt = new (tree.allocNode(NodeType::ELSE)) Else{ifStmt, proc};
         return elseStmt;
      
      default: //ELSE IF
         consumeKeyword(KeywordID::IF, "bad ELSE IF block");
         If* ifOfElif = parseIf(); //will consume EOS

         //return
         If* ifStmt = new (tree.allocNode(NodeType::IF)) If{condition, proc};
         ElseIf* elifStmt = new (tree.allocNode(NodeType::ELSE_IF)) ElseIf{ifStmt, ifOfElif};
         return elifStmt;
   }
}

clef::Switch* clef::Parser::parseSwitch() {
   //condition
   consumeOperator(OperatorID::CALL_OPEN, "SWITCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OperatorID::CALL_CLOSE, "SWITCH without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad SWITCH block");
   Scope* procedure = new (tree.allocNode(NodeType::SCOPE)) Scope{tree.allocBuf<Stmt*>()};
   SwitchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) SwitchCases{tree.allocBuf<mcsl::pair<Expr*,Stmt*>>(), procedure};

   while (!tryConsumeOperator(OperatorID::LIST_CLOSE)) {
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         Expr* caseExpr = parseExpr();
         consumeOperator(OperatorID::LABEL_DELIM, "bad CASE in SWITCH statement");
         
         //!NOTE: UNFINISHED

      } else if (tryConsumeKeyword(KeywordID::DEFAULT)) { //DEFAULT
         consumeOperator(OperatorID::LABEL_DELIM, "bad DEFAULT in SWITCH statement");
         
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
   consumeOperator(OperatorID::CALL_OPEN, "MATCH without opening parens for condition");
   Expr* condition = parseExpr();
   consumeOperator(OperatorID::CALL_CLOSE, "MATCH without closing parens for condition");

   //procedure
   consumeOperator(OperatorID::LIST_OPEN, "bad MATCH block");
   MatchCases* cases = new (tree.allocNode(NodeType::SWITCH_CASES)) MatchCases{tree.allocBuf<mcsl::pair<Expr*,Scope*>>()};

   while (!tryConsumeOperator(OperatorID::LIST_CLOSE)) {
      Expr* caseExpr;
      if (tryConsumeKeyword(KeywordID::CASE)) { //CASE
         caseExpr = parseExpr();
      } else {
         consumeKeyword(KeywordID::DEFAULT, "bad MATCH block procedure");
         caseExpr = nullptr;   
      }
      consumeOperator(OperatorID::LABEL_DELIM, "bad CASE or DEFAULT in MATCH statement");
      consumeOperator(OperatorID::LIST_OPEN, "bad CASE or DEFAULT procedure in MATCH statement");
      Scope* procedure = parseProcedure();
      
      cases->emplace_back(caseExpr, procedure);
   }

   //EOS
   consumeEOS("MATCH statement without EOS");

   //return
   return new (tree.allocNode(NodeType::MATCH)) Match{condition, cases};
}

clef::Function* clef::Parser::parseFunction() {
   Identifier* name = tryParseIdentifier();
   
   //params
   consumeOperator(OperatorID::CALL_OPEN, "FUNC without parameters");
   ParamList* params = new (tree.allocNode(NodeType::FUNC_SIG)) ParamList{tree.allocBuf<Variable*>()};
   while (!tryConsumeOperator(OperatorID::CALL_CLOSE)) {
      //parse parameter
      TypeQualMask quals = parseQuals();
      Type* paramType = (Type*)parseIdentifier();
         ((astNode*)paramType)->downCast(NodeType::TYPE);
      Identifier* paramName = tryParseIdentifier();
      //!NOTE: UNFINISHED (no support for default values)
      // Expr* defaultVal;
      // if (tryConsumeOperator(OperatorID::ASSIGN)) {
      //    defaultVal = parseExpr();
      //    if (!defaultVal) { logError(ErrCode::PARSER_UNSPEC, "invalid FUNC parameter default value"); }
      // } else { defaultVal = nullptr; }

      //push to parameter list
      ((astNode*)paramName)->downCast(NodeType::VAR);
      params->push_back(new (paramName) Variable{paramType, paramName});

      //check for comma
      if (!tryConsumeOperator(OperatorID::COMMA)) {
         consumeOperator(OperatorID::CALL_CLOSE, "bad FUNC parameter list");
      }
   }

   //return type
   consumeOperator(OperatorID::MEMBER_OF_POINTER_ACCESS, "FUNC without trailing return type");
   TypeQualMask returnTypeQuals = parseQuals();
   Type* returnType = (Type*)parseIdentifier();
      ((astNode*)returnType)->downCast(NodeType::TYPE);

   //make signature
   FuncSig* sig = new (tree.allocNode(NodeType::FUNC_SIG)) FuncSig{sig, params};

   if (tryConsumeEOS()) { //forward declaration
      if (name) {
         Function* func = (Function*)name;
         ((astNode*)func)->downCast(NodeType::FUNC);
         return new (func) Function{sig, name};
      } else {
         return new (tree.allocNode(NodeType::FUNC)) Function{sig};
      }
   }

   //definition
   consumeOperator(OperatorID::LIST_OPEN, "bad FUNC definition");
   Scope* procedure = parseProcedure();

   //EOS
   consumeEOS("FUNC without EOS");

   //return
   if (name) {
      Function* func = (Function*)name;
      ((astNode*)func)->downCast(NodeType::FUNC);
      return new (func) Function{sig, procedure, name};
   } else {
      return new (tree.allocNode(NodeType::FUNC)) Function{sig, procedure};
   }
}

#pragma region objectType

clef::Interface* clef::Parser::parseInterface() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->downCast(NodeType::INTERFACE);
      return new (name) Interface{(Type*)name};
   }
   
   //inheritance
   InterfaceSpec* spec = new (tree.allocInterfaceSpec()) InterfaceSpec{};
   if (tryConsumeOperator(OperatorID::LABEL_DELIM)) {
      do {
         Interface* parentType = (Interface*)parseIdentifier();
            ((astNode*)parentType)->downCast(NodeType::INTERFACE);
         spec->inheritedInterfaces().push_back(parentType);
      } while (tryConsumeOperator(OperatorID::COMMA));
   }

   //definition
   consumeOperator(OperatorID::LIST_OPEN, "bad INTERFACE definition");
   while (!tryConsumeOperator(OperatorID::LIST_CLOSE)) {
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
   ((astNode*)name)->downCast(NodeType::INTERFACE);
   return new (name) Interface{spec, (Type*)name};
}

clef::Union* clef::Parser::parseUnion() {
   Identifier* name = tryParseIdentifier();

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::PARSER_UNSPEC, "cannot forward-declare an anonymous UNION");
      }
      ((astNode*)name)->downCast(NodeType::UNION);
      return new (name) Union{name};
   }

   consumeOperator(OperatorID::LIST_OPEN, "bad UNION definition");

   ParameterList* members = new (tree.allocNode(NodeType::PARAM_LIST)) ParameterList{tree.allocBuf<Variable*>()};
   while (!tryConsumeOperator(OperatorID::LIST_CLOSE)) {
      //parse member
      Type* memberType = (Type*)parseIdentifier();
         ((astNode*)memberType)->downCast(NodeType::TYPE);
      Identifier* memberName = parseIdentifier();
      consumeEOS("bad UNION member");
      
      //push to members list
      Variable* member = (Variable*)memberName;
      ((astNode*)member)->downCast(NodeType::VAR);
      member->type() = memberType;
      members->push_back(member);
   }

   //EOS
   consumeEOS("UNION without EOS");

   //return
   if (name) { ((astNode*)name)->downCast(NodeType::UNION); }
   return new (name) Union{name, members};
}

//!NOTE: clef::Parser::parseMask relies on this function - be careful changing observable behavior
clef::Enum* clef::Parser::parseEnum() {
   Identifier* name = tryParseIdentifier();
   Type* baseType;
   if (tryConsumeOperator(OperatorID::LABEL_DELIM)) {
      baseType = (Type*)parseIdentifier();
         ((astNode*)baseType)->downCast(NodeType::TYPE);
   } else { baseType = nullptr; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::PARSER_UNSPEC, "cannot forward-declare an anonymous ENUM");
      }
      ((astNode*)name)->anyCast(NodeType::ENUM);
      return new (name) Enum{baseType};
   }

   consumeOperator(OperatorID::LIST_OPEN, "bad ENUM definition");
   while (!tryConsumeOperator(OperatorID::LIST_CLOSE)) {
      Identifier* enumerator = parseIdentifier();
      Expr* val;
      if (tryConsumeOperator(OperatorID::ASSIGN)) {
         val = parseExpr();
      } else { val = nullptr; }

      //!NOTE: UNFINISHED (need to actually add push enumerator to the enum)

      if (!tryConsumeOperator(OperatorID::COMMA)) {
         consumeOperator(OperatorID::LIST_CLOSE, "bad ENUM enumerator");
         break;
      }
   }

   //EOS
   consumeEOS("ENUM without EOS");

   //return
   //!NOTE: UNFINISHED
}

//!NOTE: ASSUMES THAT THE SYNTAX AND MEMORY LAYOUT OF THE AST NODES FOR MASKS AND ENUMS ARE IDENTICAL
clef::Mask* clef::Parser::parseMask() {
   astNode* mask = (astNode*)parseEnum();
   mask->anyCast(NodeType::MASK);
   return (Mask*)mask;
}

#pragma endregion objectType
#endif //PARSER_CPP