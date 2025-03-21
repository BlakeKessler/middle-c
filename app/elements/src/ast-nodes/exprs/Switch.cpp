#ifndef SWITCH_CPP
#define SWITCH_CPP

#include "ast-nodes/exprs/Switch.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Switch& obj) {
   file.printf(FMT("SWITCH (id=%u) {id=%u}"), +obj.condition(), +obj.cases());
   return file;
}

#endif //SWITCH_CPP