#ifndef PARAM_LIST_CPP
#define PARAM_LIST_CPP

#include "ast-nodes/node-lists/ParameterList.hpp"

#include "io.hpp"

void clef::ParamList::printf() const {
   mcsl::printf(mcsl::FMT("parameters ("));
   if (size()) {
      mcsl::printf(mcsl::FMT("id=%u"), +self[0]);
      for (uint i = 1; i < size(); ++i) {
         mcsl::printf(mcsl::FMT(", id=%u"), +self[i]);
      }
   }
   mcsl::printf(mcsl::FMT(")"));
}

#endif //PARAM_LIST_CPP