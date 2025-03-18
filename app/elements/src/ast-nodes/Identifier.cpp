#ifndef IDENTIFIER_CPP
#define IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

bool clef::Identifier::operator==(const Identifier& other) const {
   if (+_keywordID || +other._keywordID) {
      return _keywordID == other._keywordID;
   }
   return sameName(other) && sameScope(other);
}

void clef::Identifier::__printName() const {
   mcsl::printf(mcsl::FMT("\033[4m"));
   if (+_keywordID) {
      mcsl::printf(mcsl::FMT("%s"), toString(_keywordID));
   } else {
      mcsl::printf(mcsl::FMT("%s"), _name_size, _name_buf);
   }
   mcsl::printf(mcsl::FMT("\033[24m"));
}
void clef::Identifier::__printf(const char* nodetype) const {
   mcsl::printf(mcsl::FMT("%s: "), nodetype);
   __printName();
   mcsl::printf(mcsl::FMT(" (scope: id=%u)"), +scopeName());
}
void clef::Identifier::printf() const {
   __printf("identifier");
}

#endif //IDENTIFIER_CPP