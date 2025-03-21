#ifndef CLASS_CPP
#define CLASS_CPP

#include "ast-nodes/types/Class.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Class& obj) {
   obj.printAs(file, FMT("class"));
   file.printf(FMT(" {spec: id=%u}"), +obj.spec());
   return file;
}

#endif //CLASS_CPP