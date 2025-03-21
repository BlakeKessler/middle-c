#ifndef FOR_LOOP_CPP
#define FOR_LOOP_CPP

#include "ast-nodes/exprs/ForLoop.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ForLoop& obj) {
   file.printf(FMT("FOR (id=%u) {id=%u}"), +obj.params(), +obj.procedure());
   return file;
}

#endif //FOR_LOOP_CPP