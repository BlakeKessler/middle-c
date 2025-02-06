#ifndef CLASS_CPP
#define CLASS_CPP

#include "ast-nodes/types/Class.hpp"

#include <cstdio>

void clef::Class::printf() const {
   std::printf("class: \033[4m%.*s\033[24m (scope: id=%u) {spec: id=%u}", name().size(), name().begin(), +scopeName(), +spec());
}

#endif //CLASS_CPP