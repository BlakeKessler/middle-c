#ifndef UNION_CPP
#define UNION_CPP

#include "ast-nodes/types/Union.hpp"

#include <cstdio>

void clef::Union::printf() const {
   std::printf("union: \033[4m%.*s\033[24m (scope: id=%u) {memebers: id=%u}", name().size(), name().begin(), +scopeName(), +members());
}

#endif //UNION_CPP