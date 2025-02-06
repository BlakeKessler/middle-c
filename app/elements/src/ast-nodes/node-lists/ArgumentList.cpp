#ifndef ARG_LIST_CPP
#define ARG_LIST_CPP

#include "ast-nodes/node-lists/ArgumentList.hpp"

#include <cstdio>

void clef::ArgList::printf() const {
   std::printf("arguments (");
   if (size()) {
      std::printf("id=%u", +self[0]);
      for (uint i = 1; i < size(); ++i) {
         std::printf(", id=%u", +self[i]);
      }
   }
   std::printf(")");
}

#endif //ARG_LIST_CPP