#ifndef PARAM_LIST_CPP
#define PARAM_LIST_CPP

#include "ast-nodes/node-lists/ParameterList.hpp"

#include <cstdio>

void clef::ParamList::printf() const {
   std::printf("parameters (");
   if (size()) {
      std::printf("id=%u", +self[0]);
      for (uint i = 1; i < size(); ++i) {
         std::printf(", id=%u", +self[i]);
      }
   }
   std::printf(")");
}

#endif //PARAM_LIST_CPP