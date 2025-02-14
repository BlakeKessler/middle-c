#ifndef ENUM_CPP
#define ENUM_CPP

#include "ast-nodes/types/Enum.hpp"

#include <cstdio>

void clef::Enum::printf() const {
   __printf("enum");
   std::printf(" : (base: id=%u) {members: id=%u}", +baseType(), +enumerators());
}

#endif //ENUM_CPP