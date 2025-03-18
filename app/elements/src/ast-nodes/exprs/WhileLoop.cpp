#ifndef WHILE_LOOP_CPP
#define WHILE_LOOP_CPP

#include "ast-nodes/exprs/WhileLoop.hpp"

#include "io.hpp"

void clef::WhileLoop::printf() const {
   mcsl::printf(mcsl::FMT("WHILE (id=%u) {id=%u}"), +condition(), +procedure());
}

#endif //WHILE_LOOP_CPP