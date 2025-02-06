#ifndef FUNC_CPP
#define FUNC_CPP

#include "ast-nodes/Function.hpp"

#include <cstdio>

void clef::Function::printf() const {
   std::printf("function: \033[4m%.*s\033[24m (scope: id=%u) - (signature id=%u) {procedure id=%u}", name().size(), name().begin(), +scopeName(), +signature(), +procedure());
}

#endif //FUNC_CPP