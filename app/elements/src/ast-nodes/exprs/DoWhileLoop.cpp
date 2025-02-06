#ifndef DO_WHILE_LOOP_CPP
#define DO_WHILE_LOOP_CPP

#include "ast-nodes/exprs/DoWhileLoop.hpp"

#include <cstdio>

void clef::DoWhileLoop::printf() const {
   std::printf("DO {id=%u} WHILE (id=%u)", +procedure(), +condition());
}

#endif //DO_WHILE_LOOP_CPP