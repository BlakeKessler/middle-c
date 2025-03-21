#ifndef FUNC_SIG_CPP
#define FUNC_SIG_CPP

#include "ast-nodes/types/FunctionSignature.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::FuncSig& obj) {
   obj.printAs(file, FMT("function signature"));
   file.printf(FMT(": (id=%u) -> id=%u"), +obj.params(), +obj.returnType());
   return file;
}

#endif //FUNC_SIG_CPP