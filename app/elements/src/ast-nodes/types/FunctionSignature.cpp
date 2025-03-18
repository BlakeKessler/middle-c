#ifndef FUNC_SIG_CPP
#define FUNC_SIG_CPP

#include "ast-nodes/types/FunctionSignature.hpp"

#include "io.hpp"

void clef::FuncSig::printf() const {
   __printf("function signature");
   mcsl::printf(mcsl::FMT(": (id=%u) -> id=%u"), +params(), +returnType());
}

#endif //FUNC_SIG_CPP