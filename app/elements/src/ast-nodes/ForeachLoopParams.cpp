#ifndef FOREACH_LOOP_PARAMS_CPP
#define FOREACH_LOOP_PARAMS_CPP

#include "ast-nodes/ForeachLoopParams.hpp"
#include "io.hpp"

void clef::ForeachLoopParams::printf() const {
   mcsl::printf(mcsl::FMT("FOREACH_PARAMS (id=%u : id=%u)"), +iterator(), +target());
}

#endif //FOREACH_LOOP_PARAMS_CPP