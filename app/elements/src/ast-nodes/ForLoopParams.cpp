#ifndef FOR_LOOP_PARAMS_CPP
#define FOR_LOOP_PARAMS_CPP

#include "ast-nodes/ForLoopParams.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ForLoopParams& obj) {
   mcsl::printf(mcsl::FMT("FOR_PARAMS: (id=%u; id=%u; id=%u)"), +obj.decl(), +obj.condition(), +obj.increment());
   return file;
}

#endif //FOR_LOOP_PARAMS_CPP