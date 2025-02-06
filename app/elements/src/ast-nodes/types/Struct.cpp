#ifndef STRUCT_CPP
#define STRUCT_CPP

#include "ast-nodes/types/Struct.hpp"

#include <cstdio>

void clef::Struct::printf() const {
   std::printf("struct: \033[4m%.*s\033[24m (scope: id=%u) {spec: id=%u}", name().size(), name().begin(), +scopeName(), +spec());
}

#endif //STRUCT_CPP