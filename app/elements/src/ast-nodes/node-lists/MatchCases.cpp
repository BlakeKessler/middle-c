#ifndef MATCH_CASES_CPP
#define MATCH_CASES_CPP

#include "ast-nodes/node-lists/MatchCases.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::MatchCases& obj) {
   file.printf(FMT("MATCH_CASES:"));
   for (uint i = 0; i < obj.size(); ++i) {
      auto tmp = obj[i];
      file.printf(FMT(" {case (id=%u): {id=%u}}"), +tmp.first, +tmp.second);
   }
   return file;
}

#endif //MATCH_CASES_CPP