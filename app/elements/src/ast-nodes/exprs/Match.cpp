#ifndef MATCH_CPP
#define MATCH_CPP

#include "ast-nodes/exprs/Match.hpp"

#include "io.hpp"

void clef::Match::printf() const {
   mcsl::printf(mcsl::FMT("MATCH (id=%u) {id=%u}"), +condition(), +cases());
}

#endif //MATCH_CPP