#ifndef UNION_CPP
#define UNION_CPP

#include "ast-nodes/types/Union.hpp"

#include <cstdio>

void clef::Union::printf() const {
   __printf("union");
   std::printf(" {memebers: id=%u}", +members());
}

#endif //UNION_CPP