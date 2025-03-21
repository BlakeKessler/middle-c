#ifndef DO_WHILE_LOOP_CPP
#define DO_WHILE_LOOP_CPP

#include "ast-nodes/exprs/DoWhileLoop.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::DoWhileLoop& obj) {
   file.printf(FMT("DO {id=%u} WHILE (id=%u)"), +obj.procedure(), +obj.condition());
   return file;
}

#endif //DO_WHILE_LOOP_CPP