#ifndef STRUCT_CPP
#define STRUCT_CPP

#include "ast-nodes/types/Struct.hpp"

#include <cstdio>

void clef::Struct::printf() const {
   __printf("struct");
   std::printf(" {spec: id=%u}", +spec());
}

#endif //STRUCT_CPP