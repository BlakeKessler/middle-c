#ifndef RAW_IDENTIFIER_CPP
#define RAW_IDENTIFIER_CPP

#include "ast-nodes/Identifier.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::RawIdentifier& obj) {
   file.printf(FMT("raw identifier: \033[4m"));
   if (+obj.keywordID()) {
      file.printf(mcsl::FMT("%s"), toString(obj.keywordID()));
   } else {
      file.printf(mcsl::FMT("%s"), obj.name());
   }
   file.printf(mcsl::FMT("\033[24m"));
   return file;
}

#endif //RAW_IDENTIFIER_CPP