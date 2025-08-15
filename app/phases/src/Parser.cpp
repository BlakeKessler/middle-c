#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "Lexer.hpp"
#include "Symbol.hpp"
#include "pretty-print.hpp"

#include "dyn_arr.hpp"

clef::res<void> clef::Parser::consumeKeyword(KeywordID kw) {
   if (_currTok.type() != TokenType::KEYWORD) {
      return {ErrCode::MISSING_KW};
   }
   if (_currTok.keywordID() != kw) {
      return {ErrCode::BAD_KW};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeOp(OpID op) {
   if (_currTok.type() != TokenType::OP) {
      return {ErrCode::MISSING_OP};
   }
   if (_currTok.opID() != op) {
      return {ErrCode::BAD_OP};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeBlockDelim(BlockType type, BlockDelimRole role) {
   if (_currTok.type() != TokenType::BLOCK_DELIM) {
      return {ErrCode::MISSING_BLOCK_DELIM};
   }
   if (_currTok.block().type != type || !(+_currTok.block().role & +role)) {
      return {ErrCode::BAD_BLOCK_DELIM};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::consumeEOS() {
   if (_currTok.type() != TokenType::EOS) {
      return {ErrCode::MISSING_EOS};
   }
   nextToken();
   return {};
}
clef::res<clef::Label> clef::Parser::parseLabel() {
   if (_currTok.type() != TokenType::IDEN) {
      return {ErrCode::MISSING_LABEL};
   }
   const mcsl::str_slice name = _currTok.name();
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
         Ternary* tern = _tree.make<Ternary>(cond, lhs, rhs);
         Expr* expr = _tree.make<Expr>(tern);
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

         Expr* expr = _tree.make<Expr>(lhs, rhs, op.first.opID());
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
      TODO;
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