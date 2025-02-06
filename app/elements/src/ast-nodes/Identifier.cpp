#ifndef IDENTIFIER_CPP
#define IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"

#include <cstdio>

bool clef::Identifier::operator==(const Identifier& other) const {
   if (+_keywordID || +other._keywordID) {
      return _keywordID == other._keywordID;
   }
   return sameName(other) && sameScope(other);
}

void clef::Identifier::printf() const {
   std::printf("identifier: \033[4m%.*s\033[24m (scope: id=%u)", _name_size, _name_buf, +scopeName());
}

#endif //IDENTIFIER_CPP