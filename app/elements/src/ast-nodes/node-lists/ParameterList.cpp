#ifndef PARAM_LIST_CPP
#define PARAM_LIST_CPP

#include "ast-nodes/node-lists/ParameterList.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::ParamList& obj) {
   file.printf(FMT("parameters ("));
   if (obj.size()) {
      file.printf(FMT("id=%u"), +obj[0]);
      for (uint i = 1; i < obj.size(); ++i) {
         file.printf(FMT(", id=%u"), +obj[i]);
      }
   }
   file.printf(FMT(")"));
   return file;
}

#endif //PARAM_LIST_CPP