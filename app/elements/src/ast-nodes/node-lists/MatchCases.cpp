#ifndef MATCH_CASES_CPP
#define MATCH_CASES_CPP

#include "ast-nodes/node-lists/MatchCases.hpp"

#include "io.hpp"

void clef::MatchCases::printf() const {
   mcsl::printf(mcsl::FMT("MATCH_CASES:"));
   for (uint i = 0; i < size(); ++i) {
      auto tmp = self[i];
      mcsl::printf(mcsl::FMT(" {case (id=%u): {id=%u}}"), +tmp.first, +tmp.second);
   }
}

#endif //MATCH_CASES_CPP