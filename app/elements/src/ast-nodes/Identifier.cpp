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

void clef::Identifier::__printName(mcsl::File& file) const {
   file.write(mcsl::FMT("\033[4m"));
   if (+_keywordID) {
      file.printf(mcsl::FMT("%s"), toString(_keywordID));
   } else {
      file.printf(mcsl::FMT("%s"), name());
   }
   file.write(mcsl::FMT("\033[24m"));
}
void clef::Identifier::printAs(mcsl::File& file, const mcsl::str_slice nodetype) const {
   file.printf(mcsl::FMT("%s: "), nodetype);
   __printName(file);
   file.printf(mcsl::FMT(" (scope: id=%u)"), +scopeName());
}
mcsl::File& mcsl::write(File& file, const clef::Identifier& obj) {
   obj.printAs(file, FMT("identifier"));
   return file;
}

#endif //IDENTIFIER_CPP