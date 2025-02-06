#ifndef TYPE_CPP
#define TYPE_CPP

#include "ast-nodes/Type.hpp"

#include <cstdio>

void clef::Type::printf() const {
   std::printf("type: \033[4m%.*s\033[24m (scope: id=%u)", name().size(), name().begin(), +scopeName());
}

#endif //TYPE_CPP