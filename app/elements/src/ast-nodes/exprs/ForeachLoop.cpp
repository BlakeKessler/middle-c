#ifndef FOREACH_LOOP_CPP
#define FOREACH_LOOP_CPP

#include "ast-nodes/exprs/ForeachLoop.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ForeachLoop& obj) {
   file.printf(FMT("FOREACH (id=%u : id=%u) {id=%u}"), +obj.iterator(), +obj.target(), +obj.procedure());
   return file;
}

#endif //FOREACH_LOOP_CPP