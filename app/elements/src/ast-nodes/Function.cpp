#ifndef FUNC_CPP
#define FUNC_CPP

#include "ast-nodes/Function.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Function& obj) {
   obj.printAs(file, FMT("function"));
   file.printf(FMT(" - (signature id=%u) {procedure id=%u}"), +obj.signature(), +obj.procedure());
   return file;
}

#endif //FUNC_CPP