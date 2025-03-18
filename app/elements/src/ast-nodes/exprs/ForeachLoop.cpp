#ifndef FOREACH_LOOP_CPP
#define FOREACH_LOOP_CPP

#include "ast-nodes/exprs/ForeachLoop.hpp"

#include "io.hpp"

void clef::ForeachLoop::printf() const {
   mcsl::printf(mcsl::FMT("FOREACH (id=%u) {id=%u}"), +params(), +procedure());
}

#endif //FOREACH_LOOP_CPP