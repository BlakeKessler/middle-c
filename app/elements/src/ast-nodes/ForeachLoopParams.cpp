#ifndef FOREACH_LOOP_PARAMS_CPP
#define FOREACH_LOOP_PARAMS_CPP

#include "ast-nodes/ForeachLoopParams.hpp"

#include <cstdio>

void clef::ForeachLoopParams::printf() const {
   std::printf("FOREACH_PARAMS (id=%u : id=%u)", +decl(), +target());
}

#endif //FOREACH_LOOP_PARAMS_CPP