#ifndef MASK_CPP
#define MASK_CPP

#include "ast-nodes/types/Mask.hpp"

#include <cstdio>

void clef::Mask::printf() const {
   __printf("mask");
   std::printf(" : (base: id=%u) {members: id=%u}", +baseType(), +enumerators());
}

#endif //MASK_CPP