#ifndef MATCH_CASES_CPP
#define MATCH_CASES_CPP

#include "ast-nodes/node-lists/MatchCases.hpp"

#include <cstdio>

void clef::MatchCases::printf() const {
   std::printf("MATCH_CASES:");
   for (uint i = 0; i < size(); ++i) {
      auto tmp = self[i];
      std::printf(" {case (id=%u): {id=%u}}", +tmp.first, +tmp.second);
   }
}

#endif //MATCH_CASES_CPP