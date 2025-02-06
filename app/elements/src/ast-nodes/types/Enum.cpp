#ifndef ENUM_CPP
#define ENUM_CPP

#include "ast-nodes/types/Enum.hpp"

#include <cstdio>

void clef::Enum::printf() const {
   std::printf("enum: \033[4m%.*s\033[24m (scope: id=%u) : (base: id=%u) {members: id=%u}", name().size(), name().begin(), +scopeName(), +baseType(), +enumerators());
}

#endif //ENUM_CPP