#ifndef IDENTIFIER_CPP
#define IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"
#include "pretty-print.hpp"

#include <cstdio>

bool clef::Identifier::operator==(const Identifier& other) const {
   if (+_keywordID || +other._keywordID) {
      return _keywordID == other._keywordID;
   }
   return sameName(other) && sameScope(other);
}

void clef::Identifier::__printName() const {
   std::printf("\033[4m");
   if (+_keywordID) {
      std::printf("%s", toString(_keywordID));
   } else {
      std::printf("%.*s", _name_size, _name_buf);
   }
   std::printf("\033[24m");
}
void clef::Identifier::__printf(const char* nodetype) const {
   std::printf("%s: ", nodetype);
   __printName();
   std::printf(" (scope: id=%u)", +scopeName());
}
void clef::Identifier::printf() const {
   __printf("identifier");
}

#endif //IDENTIFIER_CPP