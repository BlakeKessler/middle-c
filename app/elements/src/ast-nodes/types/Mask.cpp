#ifndef MASK_CPP
#define MASK_CPP

#include "ast-nodes/types/Mask.hpp"

#include "io.hpp"

void clef::Mask::printf() const {
   __printf("mask");
   mcsl::printf(mcsl::FMT(" : (base: id=%u) {members: id=%u}"), +baseType(), +enumerators());
}

#endif //MASK_CPP