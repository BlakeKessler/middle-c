#ifndef MATCH_CPP
#define MATCH_CPP

#include "ast-nodes/exprs/Match.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Match& obj) {
   file.printf(FMT("MATCH (id=%u) {id=%u}"), +obj.condition(), +obj.cases());
   return file;
}

#endif //MATCH_CPP