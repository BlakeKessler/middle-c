#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"

/*inline*/ bool clef::Parser::consumeKeyword(const KeywordID keywordID, const char* errStr) {
   Identifier* keyword = parseIdentifier();
   if (!keyword || keyword->keywordID() != keywordID) {
      logError(ErrCode::PARSER_UNSPEC, errStr);
      return false;
   }
   return true;
}
/*inline*/ bool clef::Parser::tryConsumeKeyword(const KeywordID keywordID) {
   auto oldTokIt = tokIt;
   Identifier* keyword = parseIdentifier();
   if (keyword && keyword->keywordID() == keywordID) {
      return true;
   } else {
      tokIt = oldTokIt;
      return false;
   }
}

/*inline*/ bool clef::Parser::consumeOperator(const OpID id, const char* errStr) {
   if (tokIt->type() != TokenType::OP || OPERATORS[*tokIt].opID() != id) {
      logError(ErrCode::PARSER_UNSPEC, errStr);
      return false;
   }
   ++tokIt;
   return true;
}
/*inline*/ bool clef::Parser::tryConsumeOperator(const OpID id) {
   if (tokIt->type() != TokenType::OP || OPERATORS[*tokIt].opID() != id) {
      return false;
   }
   ++tokIt;
   return true;
}

/*inline*/ bool clef::Parser::consumeEOS(const char* errStr) {
   if (tokIt->type() == TokenType::EOS) {
      ++tokIt;
      return true;
   }
   logError(ErrCode::PARSER_UNSPEC, errStr);
   return false;
}
/*inline*/ bool clef::Parser::tryConsumeEOS() {
   if (tokIt->type() == TokenType::EOS) {
      ++tokIt;
      return true;
   }
   return false;
}

#endif //PARSER_HELPERS_CPP