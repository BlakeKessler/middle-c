#ifndef SWITCH_CASES_CPP
#define SWITCH_CASES_CPP

#include "ast-nodes/node-lists/SwitchCases.hpp"

#include "io.hpp"

void clef::SwitchCases::printf() const {
   mcsl::printf(mcsl::FMT("SWITCH_CASES {id=%u}:"), +procedure());
   for (uint i = 0; i < size(); ++i) {
      auto tmp = self[i];
      mcsl::printf(mcsl::FMT(" {case (id=%u): id=%u}"), +tmp.first, +tmp.second);
   }
}

#endif //SWITCH_CASES_CPP