#ifndef DO_WHILE_LOOP_CPP
#define DO_WHILE_LOOP_CPP

#include "ast-nodes/exprs/DoWhileLoop.hpp"

#include "io.hpp"

void clef::DoWhileLoop::printf() const {
   mcsl::printf(mcsl::FMT("DO {id=%u} WHILE (id=%u)"), +procedure(), +condition());
}

#endif //DO_WHILE_LOOP_CPP