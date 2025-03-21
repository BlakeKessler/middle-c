#ifndef STRUCT_CPP
#define STRUCT_CPP

#include "ast-nodes/types/Struct.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Struct& obj) {
   obj.printAs(file, FMT("struct"));
   file.printf(FMT(" {spec: id=%u}"), +obj.spec());
   return file;
}

#endif //STRUCT_CPP