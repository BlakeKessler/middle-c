#ifndef FOR_LOOP_PARAMS
#define FOR_LOOP_PARAMS

#include "ast-nodes/ForLoopParams.hpp"

#include "io.hpp"

void clef::ForLoopParams::printf() const {
   mcsl::printf(mcsl::FMT("FOR_PARAMS: (id=%u; id=%u; id=%u)"), +decl(), +condition(), +increment());
}

#endif //FOR_LOOP_PARAMS