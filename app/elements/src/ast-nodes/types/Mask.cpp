#ifndef MASK_CPP
#define MASK_CPP

#include "ast-nodes/types/Mask.hpp"

#include <cstdio>

void clef::Mask::printf() const {
   std::printf("mask: \033[4m%.*s\033[24m (scope: id=%u) : (base: id=%u) {members: id=%u}", name().size(), name().begin(), +scopeName(), +baseType(), +enumerators());
}

#endif //MASK_CPP