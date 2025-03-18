#ifndef UNION_CPP
#define UNION_CPP

#include "ast-nodes/types/Union.hpp"

#include "io.hpp"

void clef::Union::printf() const {
   __printf("union");
   mcsl::printf(mcsl::FMT(" {memebers: id=%u}"), +members());
}

#endif //UNION_CPP