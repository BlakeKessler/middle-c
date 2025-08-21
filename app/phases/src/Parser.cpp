#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "Lexer.hpp"
#include "OperatorData.hpp"
#include "Symbol.hpp"
#include "pretty-print.hpp"

#include "dyn_arr.hpp"

void clef::Parser::nextToken() {
   prevTok = currTok;
   currTok = _toks.nextToken();
   if (currTok.type() == TokenType::MACRO_INVOKE) {
      TODO;
   }
}

clef::res<void> clef::Parser::readKeyword(KeywordID kw) {
   if (currTok.type() != TokenType::KEYWORD) {
      return {ErrCode::MISSING_KW};
   }
   if (currTok.keywordID() != kw) {
      return {ErrCode::BAD_KW};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::readOp(Oplike op) {
   if (currTok.type() != TokenType::OP) {
      return {ErrCode::MISSING_OP};
   }
   if (currTok.op() != op) {
      return {ErrCode::BAD_OP};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::readBlockDelim(BlockType type, BlockDelimRole role) {
   if (currTok.type() != TokenType::BLOCK_DELIM) {
      return {ErrCode::MISSING_BLOCK_DELIM};
   }
   if (currTok.block().type != type || !(+currTok.block().role & +role)) {
      return {ErrCode::BAD_BLOCK_DELIM};
   }
   nextToken();
   return {};
}
clef::res<void> clef::Parser::readEOS() {
   if (currTok.type() != TokenType::EOS) {
      return {ErrCode::MISSING_EOS};
   }
   nextToken();
   return {};
}

bool clef::Parser::isKeyword(KeywordID kw) {
   return 
      currTok.type() == TokenType::KEYWORD &&
      currTok.keywordID() == kw;
}
bool clef::Parser::isOp(Oplike op) {
   return 
      currTok.type() == TokenType::OP &&
      currTok.op() == op;
}
bool clef::Parser::isBlockDelim(BlockType type, BlockDelimRole role) {
   return
      currTok.type() == TokenType::BLOCK_DELIM &&
      currTok.block().type == type &&
      +(currTok.block().role & role);
}
bool clef::Parser::isEOS() {
   return currTok.type() == TokenType::EOS;
}


clef::res<clef::Label> clef::Parser::parseLabel() {
   if (currTok.type() != TokenType::IDEN) {
      return {ErrCode::MISSING_LABEL};
   }
   const mcsl::str_slice name = currTok.name();
   nextToken();
   return Label{name};
}

clef::Expr* clef::Parser::parseCast(KeywordID castID) {
   using enum BlockType;
   using enum BlockDelimRole;
   using enum ErrCode;

   //type to cast to
   expect(readBlockDelim(SPECIALIZER, OPEN), BAD_EXPR, FMT("expected specializer with type"));
   Identifier type = parseType();
   expect(readBlockDelim(SPECIALIZER, CLOSE), BAD_EXPR, FMT("the only expected specializer parameter is the type to cast to"));
   
   //expession being casted
   expect(readBlockDelim(CALL, OPEN), BAD_EXPR, FMT("typecasting uses function call syntax"));
   Expr* val = parseExpr();
   expect(readBlockDelim(CALL, CLOSE), BAD_EXPR, FMT("unclosed block `%s`"), toString(Oplike::CALL_OPEN));

   //create and return cast expression node
   return tree.make<Expr>(tree.make<Expr>(type), val, toOpID(castID));
}

//no primary comma
//no primary label
clef::Expr* clef::Parser::parseCoreExpr() {
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
         OpData back = operatorStack.back().first;
         //stop checking if `op` is unary and `back` is binary
         if (!+(op.props() & OpProps::CAN_BE_BINARY) && +(back.props() & OpProps::CAN_BE_BINARY)) {
            break;
         }
         //stop checking if it has lower precedence than `op`
         if (back.precedence() < op.precedence()) {
            break;
         }
         //stop checking if it has equal precedence to `op` and `op` is right associative
         if (back.precedence() == op.precedence()) {
            constexpr OpProps mask = OpProps::IS_LEFT_ASSOC | OpProps::CAN_BE_POSTFIX;
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
            nextToken();
            if (kw == KeywordID::FUNC) { //inline functions
               auto [fn, ov] = parseFunc();
               Expr* expr = tree.make<Expr>(fn, ov);
               operandStack.emplace_back(expr, currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isValue(kw)) { //value keywords
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
               operandStack.emplace_back(parseCast(kw), currTok);
               prevTokIsOperand = true;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isPrefixOpLike(kw)) { //prefix-operator-like keywords (EX: `return`)
               if (operatorStack.size()) { //prevent use in subexpressions
                  logError(prevTok, ErrCode::BAD_EXPR, FMT("`%s` expressions cannot be subexpressions"), toString(kw));
               }
               operatorStack.emplace_back(OpData{prevTok.tokStr(), toOplike(kw), OpProps::PREFIX, 0, TokenType::KEYWORD}, prevTok);
               prevTokIsOperand = false;
               goto PARSE_EXPR_CONTINUE;
            }
            else if (isUnaryFuncLike(kw)) { //unary function-like
               expect(readBlockDelim(BlockType::CALL, BlockDelimRole::OPEN), ErrCode::BAD_KW, FMT("keyword `%s` must use function call syntax (and is not generic)"), toString(kw));
               Expr* arg = parseExpr();
               expect(readBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE), ErrCode::BAD_KW, FMT("unclosed block `%s`"), toString(Oplike::CALL_CLOSE));
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
            operandStack.emplace_back(tree.make<Expr>(expect(parseIden<false>({}), ErrCode::BAD_EXPR, FMT("invalid identifier"))), currTok);
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
               Args* args = parseArgList<false>(block.type);
               operandStack.emplace_back(tree.make<Expr>(args), tok);
            } else if (block.type == BlockType::LIST) { //tuple
               Args* args = parseArgList<false>(block.type);
               operandStack.emplace_back(tree.make<Expr>(args), tok);
            } else if (block.type == BlockType::SPECIALIZER) { [[unlikely]]; //specializer (illegal)
               logError(tok, ErrCode::BAD_EXPR, FMT("floating specializer"));
            } else { //block subexpression
               debug_assert(block.type == BlockType::CALL);
               Expr* expr = parseExpr();
               expect(readBlockDelim(block.type, BlockDelimRole::CLOSE), ErrCode::BAD_BLOCK_DELIM, FMT("unclosed block `%s`"), toString(block.type));
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
               Expr* trueVal = parseExpr();
               operandStack.emplace_back(trueVal, currTok);
               expect(readOp(Oplike::INLINE_ELSE), ErrCode::BAD_EXPR, FMT("bad ternary conditional expression"));
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
      logError(currTok, ErrCode::BAD_EXPR, FMT("empty expression"));
   }
   while (operatorStack.size()) {
      eval();
   }
   debug_assert(operandStack.size() == 1);
   return operandStack.front().first;
}

clef::Expr* clef::Parser::parseExpr() {
   Expr* expr = parseCoreExpr();
   while (readOp(Oplike::COMMA).is_ok()) {
      expr = tree.make<Expr>(expr, parseCoreExpr(), OpID::COMMA);
   }
   return expr;
}

template<bool isDecl> clef::res<clef::Identifier> clef::Parser::parseIden(Identifier typeName) {
   if (currTok.type() == TokenType::KEYWORD) {
      TODO;
   }
   
   Env oldEnv = env;
   
   //check for fully qualified name
   if (readOp(Oplike::SCOPE_RESOLUTION).is_ok()) {
      env = {
         .scope = tree.globalScope()->symbol(),
         .fn = nullptr,
         .type = nullptr
      };
   }

   if (currTok.type() != TokenType::IDEN) {
      TODO;
   }
   //name
   Identifier iden{.symbol = tree.globalScope()->symbol(), .gens = nullptr};
   
   do {
      {
         res<Symbol*> r = [&]() -> auto {
            if constexpr (isDecl) {
               return iden.symbol->insert(currTok.name());
            } else {
               return iden.symbol->get(currTok.name());
            }
         }();
         if (r.is_err()) {
            return {r.err()};
         }
         iden.symbol = r.ok();
      }
      nextToken();
      //specializer
      if (readOp(Oplike::SPECIALIZER_OPEN).is_ok()) {
         if constexpr (isDecl) {
            iden.gens = parseArgList<true>(BlockType::SPECIALIZER);
            iden.symbol->setGenParams(iden.gens);
         } else {
            if (!iden.symbol->isGeneric()) {
               logError(prevTok, ErrCode::BAD_GENERIC, FMT("`%s` is not generic"), iden);
            }
            iden.gens = parseArgList<false>(BlockType::SPECIALIZER);
         }
      } else { iden.gens = nullptr; }
   } while (readOp(Oplike::SCOPE_RESOLUTION).is_ok());

   if constexpr (isDecl) {
      iden.symbol->setType(typeName);
   } else {
      if (iden.symbol->type() != typeName.symbol->type()) {
         logError(prevTok, ErrCode::TYPE_CONFLICT, FMT("`%s` expected to be of type `%s`"), iden, typeName);
      }
   }

   env = oldEnv;
   return iden;
}

clef::Identifier clef::Parser::parseType() {
   if (currTok.type() == TokenType::KEYWORD) {
      KeywordID kw = currTok.keywordID();
      nextToken();
      if (isType(kw)) {
         return tree.getFundType(kw);
      }
      else if (isObjectType(kw)) {
         return parseTypeDef(kw);
      }
      else {
         logError(currTok, ErrCode::MISSING_TYPE, FMT("keyword `%s` does not name a type"), toString(kw));
      }
   } else if (currTok.type() == TokenType::IDEN) {
      Identifier name = expect(parseIden<false>({}), ErrCode::MISSING_TYPE, FMT("invalid type name"));
      if (!Symbol::isType(name.symbol->symbolType())) {
         logError(prevTok, ErrCode::MISSING_TYPE, FMT("`%s` does not name a type"), name);
      }
      return name;
   } else {
      logError(currTok, ErrCode::MISSING_TYPE, FMT("expected a type"));
   }
}


clef::Expr* clef::Parser::parseDecl() {
   Identifier typeName = parseType();
   Identifier name = expect(parseIden<true>(typeName), ErrCode::BAD_EXPR, FMT("expected variable name"));
   
   return tree.make<Expr>(tree.make<Expr>(typeName), tree.make<Expr>(name), OpID::LET);
}
clef::Expr* clef::Parser::parseParam() {
   Identifier typeName = parseType();
   Identifier name = parseIden<false>(typeName).orelse({});

   return tree.make<Expr>(tree.make<Expr>(typeName), tree.make<Expr>(name), OpID::LET); 
}

#pragma region type

clef::Identifier clef::Parser::parseTypeDef() {
   if (currTok.type() == TokenType::KEYWORD) {
      KeywordID kw = currTok.keywordID();
      nextToken();
      return parseTypeDef(kw);
   } else if (isBlockDelim(BlockType::LIST, BlockDelimRole::OPEN)) {
      return parseTuple();
   } else {
      logError(currTok, ErrCode::MISSING_TYPE, FMT("expected type definition"));
   }
   UNREACHABLE;
}
clef::Identifier clef::Parser::parseTypeDef(KeywordID kw) {
   using enum KeywordID;
   switch (kw) {
      case      CLASS: return parseClass();
      case     STRUCT: return parseStruct();
      case      TRAIT: return parseTrait();
      case      UNION: return parseUnion();
      case       ENUM: return parseEnum();
      case ENUM_UNION: return parseEnumunion();
      case       MASK: return parseMask();
      case  NAMESPACE: return parseNamespace();
      case      TUPLE: return parseTuple();

      default: UNREACHABLE;
   }
}

clef::Identifier clef::Parser::parseTuple() {
   //name of type (optional)
   res<Identifier> r = parseIden<true>({});
   Identifier name;
   if (r.is_ok()) { //named
      name = r.ok();
      if (name.symbol) { //identifier is already declared
         //check that the identifier refers to a tuple
         if (name.symbol->symbolType() != Symbol::TUPLE) {
            logError(currTok, ErrCode::CONFLICTING_REDECL, FMT("%s `%s` cannot be redeclared as a tuple"), toString(name.symbol->symbolType()), name);
         }
         //check that this is not a redeclaration
         if (name.symbol->type()) {
            logError(currTok, ErrCode::REDEF, FMT("tuple `%s` has already been defined"), name);
         }
      }
      else { //identifier is NOT already declared
         name = {.symbol = registerType(Symbol::TUPLE, name), .gens = {}};
      }
   } else { //anonymous
      name = {.symbol = registerType(Symbol::TUPLE, {}), .gens = {}};
   }
   //preserve and update env
   Env oldEnv = env;
   env = {
      .scope = name.symbol,
      .fn = nullptr,
      .type = name.symbol
   };
   //get tuple object
   TypeSpec::Tuple& tup = name.symbol->type().spec()->tup();

   //parse members
   expect(readBlockDelim(BlockType::LIST, BlockDelimRole::OPEN), ErrCode::BAD_TYPE_DEF, FMT("tuples are defined using curly braces"));
   do { //member
      Expr* memb = parseParam();
      tup.membs.push_back(memb->iden());
   } while ( //delimiter
      readOp(Oplike::COMMA).is_ok() //read member delimiter
      && !isBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE) //allow trailing commas (EX: `{uint,}`)
   );
   //closing curly bracket
   expect(readBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE), ErrCode::BAD_BLOCK_DELIM, FMT("unclosed block `%s`"), toString(Oplike::LIST_OPEN));

   //restore enviornment state
   env = oldEnv;

   //return
   return name;
}

#pragma endregion type

#endif