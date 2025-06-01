#ifndef MACRO_CPP
#define MACRO_CPP

#include "ast-nodes/Macro.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Macro& obj) {
   obj.printAs(file, FMT("macro"));
   file.printf(FMT(" - (signature id=%u) {procedure id=%u}"), +obj.signature(), +obj.procedure());
   return file;
}

#endif //MACRO_CPP