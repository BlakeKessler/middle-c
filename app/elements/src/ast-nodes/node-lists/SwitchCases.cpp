#ifndef SWITCH_CASES_CPP
#define SWITCH_CASES_CPP

#include "ast-nodes/node-lists/SwitchCases.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::SwitchCases& obj) {
   file.printf(FMT("SWITCH_CASES {id=%u}:"), +obj.procedure());
   for (uint i = 0; i < obj.size(); ++i) {
      auto tmp = obj[i];
      file.printf(FMT(" {case (id=%u): id=%u}"), +tmp.first, +tmp.second);
   }
   return file;
}

#endif //SWITCH_CASES_CPP