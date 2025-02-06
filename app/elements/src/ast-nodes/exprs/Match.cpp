#ifndef MATCH_CPP
#define MATCH_CPP

#include "ast-nodes/exprs/Match.hpp"

#include <cstdio>

void clef::Match::printf() const {
   std::printf("MATCH (id=%u) {id=%u}", +condition(), +cases());
}

#endif //MATCH_CPP