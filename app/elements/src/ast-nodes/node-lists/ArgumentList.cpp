#ifndef ARG_LIST_CPP
#define ARG_LIST_CPP

#include "ast-nodes/node-lists/ArgumentList.hpp"

#include "io.hpp"

void clef::ArgList::printf() const {
   mcsl::printf(mcsl::FMT("arguments ("));
   if (size()) {
      mcsl::printf(mcsl::FMT("id=%u"), +self[0]);
      for (uint i = 1; i < size(); ++i) {
         mcsl::printf(mcsl::FMT(", id=%u"), +self[i]);
      }
   }
   mcsl::printf(mcsl::FMT(")"));
}

#endif //ARG_LIST_CPP