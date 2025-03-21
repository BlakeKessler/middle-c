#ifndef UNION_CPP
#define UNION_CPP

#include "ast-nodes/types/Union.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Union& obj) {
   obj.printAs(file, FMT("union"));
   file.printf(FMT(" {memebers: id=%u}"), +obj.members());
   return file;
}

#endif //UNION_CPP