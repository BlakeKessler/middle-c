#ifndef SWITCH_CASES_CPP
#define SWITCH_CASES_CPP

#include "ast-nodes/node-lists/SwitchCases.hpp"

#include <cstdio>

void clef::SwitchCases::printf() const {
   std::printf("SWITCH_CASES {id=%u}:", +procedure());
   for (uint i = 0; i < size(); ++i) {
      auto tmp = self[i];
      std::printf(" {case (id=%u): id=%u}", +tmp.first, +tmp.second);
   }
}

#endif //SWITCH_CASES_CPP