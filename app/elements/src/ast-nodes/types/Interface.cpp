#ifndef INTERFACE_CPP
#define INTERFACE_CPP

#include "ast-nodes/types/Interface.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Interface& obj) {
   obj.printAs(file, FMT("interface"));
   file.printf(FMT(" {spec: id=%u}"), +obj.spec());
   return file;
}

#endif //INTERFACE_CPP