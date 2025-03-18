#ifndef CLASS_CPP
#define CLASS_CPP

#include "ast-nodes/types/Class.hpp"

#include "io.hpp"

void clef::Class::printf() const {
   __printf("class");
   mcsl::printf(mcsl::FMT(" {spec: id=%u}"), +spec());
}

#endif //CLASS_CPP