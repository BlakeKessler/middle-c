#ifndef IDENTIFIER_CPP
#define IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"
#include "KeywordData.hpp"

clef::KeywordID clef::Identifier::keywordID() const {
   KeywordID id = decodeKeyword(_name);
   if (+id && _scopeName) {
      throwError(ErrCode::BAD_IDEN, "cannot use keyword with scope resolution");
   }
   return id;
}

#endif //IDENTIFIER_CPP