#ifndef SCOPE_CPP
#define SCOPE_CPP

#include "ast-nodes/Scope.hpp"

#include <cstdio>

void clef::Scope::printf() const {
   std::printf("scope:");
   for (uint i = 0; i < size(); ++i) {
      std::printf(" id=%u;", +self[i]);
   }
}

#endif //SCOPE_CPP