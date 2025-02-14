#ifndef VAR_CPP
#define VAR_CPP

#include "ast-nodes/Variable.hpp"

#include <cstdio>

void clef::Variable::printf() const {
   __printf("variable");
   std::printf(": (type id=%u) = (value id=%u)", +type(), +val());
}

#endif //VAR_CPP