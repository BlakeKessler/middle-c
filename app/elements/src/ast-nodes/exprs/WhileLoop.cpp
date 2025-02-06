#ifndef WHILE_LOOP_CPP
#define WHILE_LOOP_CPP

#include "ast-nodes/exprs/WhileLoop.hpp"

#include <cstdio>

void clef::WhileLoop::printf() const {
   std::printf("WHILE (id=%u) {id=%u}", +condition(), +procedure());
}

#endif //WHILE_LOOP_CPP