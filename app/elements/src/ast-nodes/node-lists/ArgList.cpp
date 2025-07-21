#ifndef ARG_LIST_CPP
#define ARG_LIST_CPP

#include "ast-nodes/node-lists/ArgList.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ArgList& obj) {
   file.printf(FMT("arguments ("));
   if (obj.size()) {
      file.printf(FMT("id=%u"), +obj[0]);
      for (uint i = 1; i < obj.size(); ++i) {
         file.printf(FMT(", id=%u"), +obj[i]);
      }
   }
   file.printf(mcsl::FMT(")"));
   return file;
}

#endif //ARG_LIST_CPP