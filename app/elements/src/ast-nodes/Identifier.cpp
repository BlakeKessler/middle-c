#ifndef IDENTIFIER_CPP
#define IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"

bool clef::Identifier::operator==(const Identifier& other) const {
   if (+_keywordID || +other._keywordID) {
      return _keywordID == other._keywordID;
   }
   return sameName(other) && sameScope(other);
}

#endif //IDENTIFIER_CPP