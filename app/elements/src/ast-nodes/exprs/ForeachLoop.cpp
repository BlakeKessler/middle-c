#ifndef FOREACH_LOOP_CPP
#define FOREACH_LOOP_CPP

#include "ast-nodes/exprs/ForeachLoop.hpp"

#include <cstdio>

void clef::ForeachLoop::printf() const {
   std::printf("FOREACH (id=%u) {id=%u}", +params(), +procedure());
}

#endif //FOREACH_LOOP_CPP