#ifndef NAMESPACE_CPP
#define NAMESPACE_CPP

#include "ast-nodes/types/Namespace.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Namespace& obj) {
   obj.printAs(file, FMT("namespace"));
   file.printf(FMT(" {spec: id=%u}"), +obj.spec());
   return file;
}

#endif //NAMESPACE_CPP