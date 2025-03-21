#ifndef DECL_CPP
#define DECL_CPP

#include "ast-nodes/exprs/Declaration.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Decl& obj) {
   file.printf(FMT("declaration (name: id=%u) : (type: id=%u)"), +obj.name(), +obj.type());
   return file;
}

#endif //DECL_CPP