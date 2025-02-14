#ifndef GENERIC_TYPE_CPP
#define GENERIC_TYPE_CPP

#include "ast-nodes/types/GenericType.hpp"

#include <cstdio>

void clef::GenericType::printf() const {
   __printf("generic type");
}

#endif //GENERIC_TYPE_CPP