#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "Lexer.hpp"
#include "OperatorData.hpp"
#include "Symbol.hpp"
#include "pretty-print.hpp"

#include "dyn_arr.hpp"

void clef::Parser::nextToken() {
   currTok = _toks.nextToken();
   if (currTok.type() == TokenType::MACRO_INVOKE) {
      TODO;
   }
}

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
clef::res<void> clef::Parser::consumeOp(Oplike op) {
   if (currTok.type() != TokenType::OP) {
      return {ErrCode::MISSING_OP};
   }
   if (currTok.op() != op) {
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

clef::res<clef::Expr*> clef::Parser::parseCast(KeywordID castID) {
   using enum BlockType;
   using enum BlockDelimRole;
   using enum ErrCode;

   //type to cast to
   expect(consumeBlockDelim(SPECIALIZER, OPEN), BAD_EXPR, FMT("expected specializer with type"));
   auto type = expect(parseType(), BAD_EXPR, FMT("expected type"));
   expect(consumeBlockDelim(SPECIALIZER, CLOSE), BAD_EXPR, FMT("the only expected specializer parameter is the type to cast to"));
   
   //expession being casted
   expect(consumeBlockDelim(CALL, OPEN), BAD_EXPR, FMT("typecasting uses function call syntax"));
   auto val = expect(parseExpr(), BAD_EXPR, FMT("bad typecast expression"));
   expect(consumeBlockDelim(CALL, CLOSE), BAD_EXPR, FMT("unclosed block `%s`"), toString(Oplike::CALL_OPEN));

   //create and return cast expression node
   return tree.make<Expr>(tree.make<Expr>(type.first), val, toOpID(castID));
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

         case TokenType::MACRO_INVOKE:
            //macro expansion happens in `nextToken`
            UNREACHABLE;

         case TokenType::KEYWORD: { //keywords
            const KeywordID kw = currTok.keywordID();
            if (kw == KeywordID::FUNC) { //inline functions
               nextToken();
               auto f = expect(parseFunc(), ErrCode::BAD_EXPR, FMT("invalid inline function definition"));
               Expr* expr = tree.make<Expr>(f.first, f.second);
               operandStack.emplace_back(expr, currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isValue(kw)) { //value keywords
               nextToken();
               Expr* expr;
               switch (kw) {
                  case KeywordID::THIS: fthru;
                  case KeywordID::SELF:
                     TODO;
                     break;
                  case KeywordID::NULLPTR:
                     expr = tree.make<Expr>(Literal::makePtr(nullptr));
                     break;
                  case KeywordID::TRUE: fthru;
                  case KeywordID::FALSE:
                     //made into BOOL_LIT tokens by the lexer
                     UNREACHABLE;

                  default: UNREACHABLE;
               }
               operandStack.emplace_back(expr, currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isCast(kw)) { //typecasts
               nextToken();
               operandStack.emplace_back(expect(parseCast(kw), ErrCode::BAD_EXPR, FMT("invalid cast expression")), currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isPrefixOpLike(kw)) { //prefix-operator-like keywords (EX: `return`)
               if (operatorStack.size()) { //prevent use in subexpressions
                  logError(currTok, ErrCode::BAD_EXPR, FMT("`%s` expressions cannot be subexpressions"), toString(kw));
               }
               operatorStack.emplace_back(OpData{currTok.tokStr(), toOplike(kw), OpProps::PREFIX, 0, TokenType::KEYWORD}, currTok);
               prevTokIsOperand = false;
               nextToken();
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isUnaryFuncLike(kw)) { //unary function-like
               nextToken();
               expect(consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN), ErrCode::BAD_KW, FMT("keyword `%s` must use function call syntax (and is not generic)"), toString(kw));
               Expr* arg = expect(parseExpr(), ErrCode::BAD_EXPR, FMT("bad `%s` expression"), toString(kw));
               expect(consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE), ErrCode::BAD_KW, FMT("unclosed block `%s`"), toString(Oplike::CALL_CLOSE));
               operandStack.emplace_back(tree.make<Expr>(nullptr, arg, toOpID(kw)), currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isType(kw)) {
               logError(currTok, ErrCode::BAD_EXPR, FMT("floating type `%s`"), toString(kw));
            }
            else if (kw == KeywordID::LET) { //let subexpression (illegal)
               logError(currTok, ErrCode::BAD_EXPR, FMT("may not declare new variables in subexpressions"));
            } else {
               logError(currTok, ErrCode::BAD_KW, FMT("bad keyword in expression"));
            }
            UNREACHABLE;
         }

         case TokenType::IDEN: {
            Token tok = currTok;
            operandStack.emplace_back(tree.make<Expr>(expect(parseIden(), ErrCode::BAD_EXPR, FMT("invalid identifier"))), currTok);
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
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

         case TokenType::EOS: goto END_OF_EXPR;
         case TokenType::BLOCK_DELIM: {
            Token tok = currTok;
            auto block = currTok.block();
            if (!isOpener(block.role)) {
               logError(currTok, ErrCode::BAD_EXPR, FMT("unmatched block delimiter `%s`"), currTok.tokStr());
            }
            nextToken();
            
            if (prevTokIsOperand) { //function call, initializer list, subscript, or specializer
               debug_assert(operandStack.size());
               OpData op = block.invoke;
               pushOperator(op, tok);
               auto args = expect(parseArgList(block.type, false), ErrCode::BAD_BLOCK_DELIM, FMT("bad block"));
               operandStack.emplace_back(tree.make<Expr>(args), tok);
            } else if (block.type == BlockType::LIST) { //tuple
               auto args = expect(parseArgList(block.type, false), ErrCode::BAD_BLOCK_DELIM, FMT("bad block"));
               operandStack.emplace_back(tree.make<Expr>(args), tok);
            } else if (block.type == BlockType::SPECIALIZER) { [[unlikely]]; //specializer (illegal)
               logError(tok, ErrCode::BAD_EXPR, FMT("floating specializer"));
            } else { //block subexpression
               debug_assert(block.type == BlockType::CALL);
               Expr* expr = expect(parseExpr(), ErrCode::BAD_BLOCK_DELIM, FMT("unmatched block delimiter `%s`"), tok.tokStr());
               expect(consumeBlockDelim(block.type, BlockDelimRole::CLOSE), ErrCode::BAD_BLOCK_DELIM, FMT("unclosed block `%s`"), toString(block.type));
               operandStack.emplace_back(expr, tok);
            }
            prevTokIsOperand = true;
            goto PARSE_EXPR_CONTINUE;
         }

         case TokenType::OP: {
            if (currTok.op() == Oplike::COMMA || currTok.op() == Oplike::LABEL_DELIM) { //comma or label delimiter - end of expression
               goto END_OF_EXPR;
            }
            else if (currTok.op() == Oplike::INLINE_IF) { //special case for ternary expressions
               operatorStack.emplace_back(currTok.op(), currTok);
               nextToken();
               auto trueVal = expect(parseExpr(), ErrCode::BAD_EXPR, FMT("bad ternary conditional expression"));
               operandStack.emplace_back(trueVal, currTok);
               expect(consumeOp(Oplike::INLINE_ELSE), ErrCode::BAD_EXPR, FMT("bad ternary conditional expression"));
               prevTokIsOperand = false;
               goto PARSE_EXPR_CONTINUE;
            } else {
               debug_assert(currTok.op() != Oplike::INLINE_ELSE);
               if (prevTokIsOperand) { //binary or postfix unary
                  OpData op = currTok.op();
                  op.removeProps(OpProps::PREFIX);
                  debug_assert(+op.props());
                  debug_assert(!(+(op.props() & OpProps::CAN_BE_BINARY) && +(op.props() & OpProps::CAN_BE_POSTFIX)));
                  op.setPrecedence(PRECS.get(op).first);
                  pushOperator(op, currTok);
               } else { //prefix unary
                  OpData op = currTok.op();
                  debug_assert(op.props() & OpProps::CAN_BE_PREFIX);
                  op.setProps(OpProps::PREFIX);
                  op.setPrecedence(PRECS.get(op).first);
                  operatorStack.emplace_back(op, currTok);
               }

               prevTokIsOperand = false;
               nextToken();
               goto PARSE_EXPR_CONTINUE;
            }
         }
         
         case TokenType::ATTR: {
            TODO;
         }

         case TokenType::PREPROC_INIT:
            logError(currTok, ErrCode::BAD_EXPR, FMT("floating preprocessor invoke operator"));
      }

      UNREACHABLE;
      PARSE_EXPR_CONTINUE:
   }
   END_OF_EXPR:

   if (!operandStack.size()) {
      return {ErrCode::EMPTY_EXPR};
   }
   while (operatorStack.size()) {
      eval();
   }
   debug_assert(operandStack.size() == 1);
   return {operandStack.front().first};
}

#endif