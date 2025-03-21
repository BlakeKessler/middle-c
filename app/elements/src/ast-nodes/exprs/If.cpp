#ifndef IF_CPP
#define IF_CPP

#include "ast-nodes/exprs/If.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::If& obj) {
   file.printf(FMT("IF (id=%u) {id=%u}"), +obj.condition(), +obj.procedure());
   if (obj.elseStmt()) {
      file.printf(FMT(" ELSE {id=%u}"), +obj.elseStmt());
   }
   return file;
}

#endif //IF_CPP