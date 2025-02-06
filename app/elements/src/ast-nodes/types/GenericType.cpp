#ifndef GENERIC_TYPE_CPP
#define GENERIC_TYPE_CPP

#include "ast-nodes/types/GenericType.hpp"

#include <cstdio>

void clef::GenericType::printf() const {
   std::printf("generic type: \033[4m%.*s\033[24m (scope: id=%u)", name().size(), name().begin(), +scopeName());
}

#endif //GENERIC_TYPE_CPP