#ifndef IF_CPP
#define IF_CPP

#include "ast-nodes/exprs/If.hpp"

#include "io.hpp"

void clef::If::printf() const {
   mcsl::printf(mcsl::FMT("IF (id=%u) {id=%u}"), +condition(), +procedure());
   if (elseStmt()) {
      mcsl::printf(mcsl::FMT(" ELSE {id=%u}"), +elseStmt());
   }
}

#endif //IF_CPP