#ifndef ENUM_CPP
#define ENUM_CPP

#include "ast-nodes/types/Enum.hpp"

#include "io.hpp"

void clef::Enum::printf() const {
   __printf("enum");
   mcsl::printf(mcsl::FMT(" : (base: id=%u) {members: id=%u}"), +baseType(), +enumerators());
}

#endif //ENUM_CPP