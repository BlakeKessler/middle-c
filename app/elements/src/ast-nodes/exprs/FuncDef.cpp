#ifndef FUNC_DEF_CPP
#define FUNC_DEF_CPP

#include "ast-nodes/exprs/FuncDef.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::FuncDef& obj) {
   file.printf(FMT("function definition (name: id=%u) (params: id=%u) {procedure: id=%u}"), +obj.name(), +obj.params(), +obj.procedure());
   return file;
}

#endif //FUNC_DEF_CPP