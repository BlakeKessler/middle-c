#ifndef FUNC_CPP
#define FUNC_CPP

#include "ast-nodes/Function.hpp"

#include <cstdio>

void clef::Function::printf() const {
   __printf("function");
   std::printf(" - (signature id=%u) {procedure id=%u}", +signature(), +procedure());
}

#endif //FUNC_CPP