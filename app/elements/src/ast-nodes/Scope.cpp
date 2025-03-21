#ifndef SCOPE_CPP
#define SCOPE_CPP

#include "ast-nodes/Scope.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Scope& obj) {
   file.printf(FMT("scope:"));
   for (uint i = 0; i < obj.size(); ++i) {
      file.printf(FMT(" id=%u;"), +obj[i]);
   }
   return file;
}

#endif //SCOPE_CPP