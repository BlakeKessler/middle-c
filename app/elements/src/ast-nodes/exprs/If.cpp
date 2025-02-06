#ifndef IF_CPP
#define IF_CPP

#include "ast-nodes/exprs/If.hpp"

#include <cstdio>

void clef::If::printf() const {
   std::printf("IF (id=%u) {id=%u}", +condition(), +procedure());
   if (elseStmt()) {
      std::printf(" ELSE {id=%u}", +elseStmt());
   }
}

#endif //IF_CPP