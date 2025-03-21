#ifndef ENUM_CPP
#define ENUM_CPP

#include "ast-nodes/types/Enum.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Enum& obj) {
   obj.printAs(file, FMT("enum"));
   file.printf(FMT(" : (base: id=%u) {members: id=%u}"), +obj.baseType(), +obj.enumerators());
   return file;
}

#endif //ENUM_CPP