#ifndef FUNC_SIG_CPP
#define FUNC_SIG_CPP

#include "ast-nodes/types/FunctionSignature.hpp"

#include <cstdio>

void clef::FuncSig::printf() const {
   std::printf("function signature: \033[4m%.*s\033[24m (scope: id=%u): (id=%u) -> id=%u", name().size(), name().begin(), +scopeName(), +params(), +returnType());
}

#endif //FUNC_SIG_CPP