#ifndef FOR_LOOP_CPP
#define FOR_LOOP_CPP

#include "ast-nodes/exprs/ForLoop.hpp"

#include "io.hpp"

void clef::ForLoop::printf() const {
   mcsl::printf(mcsl::FMT("FOR (id=%u) {id=%u}"), +params(), +procedure());
}

#endif //FOR_LOOP_CPP