#ifndef FOREACH_LOOP_PARAMS_CPP
#define FOREACH_LOOP_PARAMS_CPP

#include "ast-nodes/ForeachLoopParams.hpp"
#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ForeachLoopParams obj) {
   file.printf(mcsl::FMT("FOREACH_PARAMS (id=%u : id=%u)"), +obj.iterator(), +obj.target());
   return file;
}

#endif //FOREACH_LOOP_PARAMS_CPP