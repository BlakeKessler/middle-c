#ifndef CLASS_CPP
#define CLASS_CPP

#include "ast-nodes/types/Class.hpp"

#include <cstdio>

void clef::Class::printf() const {
   __printf("class");
   std::printf(" {spec: id=%u}", +spec());
}

#endif //CLASS_CPP