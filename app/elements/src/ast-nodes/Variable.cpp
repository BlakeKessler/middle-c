#ifndef VAR_CPP
#define VAR_CPP

#include "ast-nodes/Variable.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Variable& obj) {
   obj.printAs(file, FMT("variable"));
   file.printf(mcsl::FMT(": (type id=%u) = (value id=%u)"), +obj.type(), +obj.val());
   return file;
}

#endif //VAR_CPP