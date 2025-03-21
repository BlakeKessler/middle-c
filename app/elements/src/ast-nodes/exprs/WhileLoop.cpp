#ifndef WHILE_LOOP_CPP
#define WHILE_LOOP_CPP

#include "ast-nodes/exprs/WhileLoop.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::WhileLoop& obj) {
   file.printf(FMT("WHILE (id=%u) {id=%u}"), +obj.condition(), +obj.procedure());
   return file;
}

#endif //WHILE_LOOP_CPP