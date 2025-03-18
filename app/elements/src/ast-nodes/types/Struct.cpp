#ifndef STRUCT_CPP
#define STRUCT_CPP

#include "ast-nodes/types/Struct.hpp"

#include "io.hpp"

void clef::Struct::printf() const {
   __printf("struct");
   mcsl::printf(mcsl::FMT(" {spec: id=%u}"), +spec());
}

#endif //STRUCT_CPP