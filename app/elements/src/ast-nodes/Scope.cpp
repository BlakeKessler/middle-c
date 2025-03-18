#ifndef SCOPE_CPP
#define SCOPE_CPP

#include "ast-nodes/Scope.hpp"

#include "io.hpp"

void clef::Scope::printf() const {
   mcsl::printf(mcsl::FMT("scope:"));
   for (uint i = 0; i < size(); ++i) {
      mcsl::printf(mcsl::FMT(" id=%u;"), +self[i]);
   }
}

#endif //SCOPE_CPP