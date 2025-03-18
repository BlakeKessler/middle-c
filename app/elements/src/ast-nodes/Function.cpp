#ifndef FUNC_CPP
#define FUNC_CPP

#include "ast-nodes/Function.hpp"

#include "io.hpp"

void clef::Function::printf() const {
   __printf("function");
   mcsl::printf(mcsl::FMT(" - (signature id=%u) {procedure id=%u}"), +signature(), +procedure());
}

#endif //FUNC_CPP