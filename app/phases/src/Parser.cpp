#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"
#include "Lexer.hpp"

clef::res<void> clef::Parser::consumeKeyword(KeywordID kw) {
   Token tok = _toks.nextToken();
   if (tok.type() != TokenType::KEYWORD) {
      return {ErrCode::MISSING_KW};
   }
   if (tok.keywordID() != kw) {
      return {ErrCode::BAD_KW};
   }
   return {};
}
clef::res<void> clef::Parser::consumeOp(OpID op) {
   Token tok = _toks.nextToken();
   if (tok.type() != TokenType::OP) {
      return {ErrCode::MISSING_OP};
   }
   if (tok.opID() != op) {
      return {ErrCode::BAD_OP};
   }
   return {};
}

#endif