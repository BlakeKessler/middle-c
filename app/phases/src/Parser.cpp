#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "Lexer.hpp"
#include "Symbol.hpp"
#include "pretty-print.hpp"

#include "dyn_arr.hpp"

clef::res<void> clef::Parser::consumeKeyword(KeywordID kw) {
   if (currTok.type() != TokenType::KEYWORD) {
      return {ErrCode::MISSING_KW};
   }
   if (currTok.keywordID() != kw) {
      return {ErrCode::BAD_KW};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeOp(OpID op) {
   if (currTok.type() != TokenType::OP) {
      return {ErrCode::MISSING_OP};
   }
   if (currTok.opID() != op) {
      return {ErrCode::BAD_OP};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeBlockDelim(BlockType type, BlockDelimRole role) {
   if (currTok.type() != TokenType::BLOCK_DELIM) {
      return {ErrCode::MISSING_BLOCK_DELIM};
   }
   if (currTok.block().type != type || !(+currTok.block().role & +role)) {
      return {ErrCode::BAD_BLOCK_DELIM};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeEOS() {
   if (currTok.type() != TokenType::EOS) {
      return {ErrCode::MISSING_EOS};
   }
   nextToken();
   return {};
}
clef::res<clef::Label> clef::Parser::parseLabel() {
   if (currTok.type() != TokenType::IDEN) {
      return {ErrCode::MISSING_LABEL};
   }
   const mcsl::str_slice name = currTok.name();
   nextToken();
   return Label{name};
}

//no primary comma
//no primary label
clef::res<clef::Expr*> clef::Parser::parseCoreExpr() {
   mcsl::dyn_arr<mcsl::pair<OpData, Token>> operatorStack;
   mcsl::dyn_arr<mcsl::pair<Expr*, Token>> operandStack;
   bool prevTokIsOperand = false;

   #pragma region helpers
   //evaluate a subexpression
   const auto eval = [&]() {
      debug_assert(operatorStack.size());
      auto op = operatorStack.pop_back();
      Expr* lhs;
      Expr* rhs;

      #define POP_OPERAND(dest, code, msg, ...) \
         if (!operandStack.size()) { \
            logError(op.second, code, FMT(msg) __VA_OPT__(,) __VA_ARGS__); \
         } \
         dest = operandStack.pop_back().first
      if (op.first.op() == Oplike::INLINE_IF) {
         POP_OPERAND(rhs, ErrCode::BAD_EXPR, "bad ternary conditional expression");
         POP_OPERAND(lhs, ErrCode::BAD_EXPR, "bad ternary conditional expression");
         POP_OPERAND(Expr* cond, ErrCode::BAD_EXPR, "bad ternary conditional expression");
         Ternary* tern = tree.make<Ternary>(cond, lhs, rhs);
         Expr* expr = tree.make<Expr>(tern);
         operandStack.emplace_back(expr, op.second);
      }
      else {
         if (prevTokIsOperand) { //binary or postfix
            POP_OPERAND(rhs, ErrCode::BAD_EXPR, (+(op.first.props() & OpProps::CAN_BE_BINARY) ? "expected binary expression" : "expected postfix expression"));
            if (+(op.first.props() & OpProps::CAN_BE_BINARY)) { //binary
               debug_assert(!+(op.first.props() & OpProps::CAN_BE_POSTFIX));
               POP_OPERAND(lhs, ErrCode::BAD_EXPR, "expected binary expression");
            } else { //postfix
               lhs = nullptr;
            }
         }
         else {
            if (!+(op.first.props() & OpProps::CAN_BE_PREFIX)) { //prefix
               logError(op.second, ErrCode::BAD_EXPR, FMT("missing lhs for operator `%s`"), toString(op.first.opID()));
            }
            rhs = nullptr;
            POP_OPERAND(lhs, ErrCode::BAD_EXPR, "`%s` is not a prefix operator", toString(op.first.opID()));
         }
         #undef POP_OPERAND

         Expr* expr = tree.make<Expr>(lhs, rhs, op.first.opID());
         operandStack.emplace_back(expr, op.second);
      }
   };
   //push an operator to the operator stack
   const auto pushOperator = [&](OpData op, Token tok) {
      while (operatorStack.size()) { //while there are operators to check
         //get the last operator on the stack
         auto back = operatorStack.back().first;
         //stop checking if it has lower precedence than `op`
         if (back.precedence() < op.precedence()) {
            break;
         }
         //stop checking if it has equal precedence to `op` and `op` is right associative
         if (back.precedence() == op.precedence()) {
            constexpr auto mask = OpProps::IS_LEFT_ASSOC | OpProps::CAN_BE_POSTFIX;
            if (!+(op.props() & mask)) {
               break;
            }
         }
         //evaluate a subexpression
         eval();
      }
      operatorStack.emplace_back(op, tok);
   };
   #pragma endregion helpers

   //parse expression
   while (!_toks.done()) {
      switch (currTok.type()) {
         case TokenType::NONE: UNREACHABLE;
         case TokenType::__OPLIKE: UNREACHABLE;

         case TokenType::MACRO_INVOKE: TODO;

         case TokenType::KEYWORD: { //keywords
            const KeywordID kw = currTok.keywordID();
            if (kw == KeywordID::FUNC) {
               nextToken();
               auto f = parseFunc().expect(FMT("invalid inline function definition"));
               Expr* expr = tree.make<Expr>(f.first, f.second);
               operandStack.emplace_back(expr, currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (kw == KeywordID::LET) {
               logError(currTok, ErrCode::BAD_EXPR, FMT("may not declare new variables in subexpressions"));
            }
            else if (isValue(kw)) {
               nextToken();
               Expr* expr;
               switch (kw) {
                  case KeywordID::THIS: fthru;
                  case KeywordID::SELF:
                     TODO;
                     break;
                  case KeywordID::TRUE: fthru;
                  case KeywordID::FALSE:
                     UNREACHABLE;
                  case KeywordID::NULLPTR:
                     expr = tree.make<Expr>(Literal::makePtr(nullptr));
                     break;
               }
               operandStack.emplace_back(expr, currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isCast(kw)) {
               nextToken();
               operandStack.emplace_back(parseCast(kw).expect(FMT("invalid cast expression")), currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isPrefixOpLike(kw)) {
               if (operatorStack.size()) { //prevent use in subexpressions
                  logError(currTok, ErrCode::BAD_EXPR, FMT("`%s` expressions cannot be subexpressions"), toString(kw));
               }
               operatorStack.emplace_back(OpData{currTok.tokStr(), toOplike(kw), OpProps::PREFIX, 0, TokenType::KEYWORD}, currTok);
               prevTokIsOperand = false;
               nextToken();
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isUnaryFuncLike(kw)) {
               nextToken();
               consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN).expect(FMT("keyword `%s` must use function call syntax (and is not generic)"), toString(kw));
               auto argList = parseArgList(BlockType::CALL, false);
               TODO;
               // operandStack.push_back(+make<Expr>(kw, argList));
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isType(kw)) {
               TODO;
               // operandStack.push_back(+make<Identifier>(tree.getFundType(kw)));
               nextToken();
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            
            logError(currTok, ErrCode::BAD_KW, FMT("bad keyword in expression"));
         }

         #define DEF_LIT(Type, TYPE) \
            case TokenType::TYPE##_NUM:      \
            operandStack.emplace_back(       \
               tree.make<Expr>(              \
                  Literal::make##Type(       \
                     currTok.val##Type(),    \
                     toTypeID(               \
                        currTok.keywordID(), \
                        tree.dataModel()))), \
               currTok);                     \
            nextToken();                     \
            prevTokIsOperand = true;         \
            goto PARSE_EXPR_CONTINUE
         DEF_LIT(Uint, UINT);
         DEF_LIT(Sint, SINT);
         DEF_LIT(Real, REAL);
         #undef DEF_LIT
         #define DEF_LIT(Type, TYPE) \
            case TokenType::TYPE##_LIT:      \
            operandStack.emplace_back(       \
               tree.make<Expr>(              \
                  Literal::make##Type(       \
                     currTok.val##Type())),  \
               currTok);                     \
            nextToken();                     \
            prevTokIsOperand = true;         \
            goto PARSE_EXPR_CONTINUE
         DEF_LIT(Bool, BOOL);
         DEF_LIT(Char, CHAR);
         DEF_LIT(Str, STR);
         #undef DEF_LIT
      }
      TODO;

      PARSE_EXPR_CONTINUE:
   }

   if (!operandStack.size()) {
      return {ErrCode::BAD_EXPR};
   }
   while (operandStack.size() != 1) {
      eval();
   }
   return {operandStack.front().first};
}

#endif