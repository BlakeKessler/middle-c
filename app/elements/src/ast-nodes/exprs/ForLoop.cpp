#ifndef FOR_LOOP_CPP
#define FOR_LOOP_CPP

#include "ast-nodes/exprs/ForLoop.hpp"

#include <cstdio>

void clef::ForLoop::printf() const {
   std::printf("FOR (id=%u) {id=%u}", +params(), +procedure());
}

#endif //FOR_LOOP_CPP