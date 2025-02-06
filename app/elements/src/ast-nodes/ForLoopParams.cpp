#ifndef FOR_LOOP_PARAMS
#define FOR_LOOP_PARAMS

#include "ast-nodes/ForLoopParams.hpp"

#include <cstdio>

void clef::ForLoopParams::printf() const {
   std::printf("FOR_PARAMS: (id=%u; id=%u; id=%u)", +decl(), +condition(), +increment());
}

#endif //FOR_LOOP_PARAMS