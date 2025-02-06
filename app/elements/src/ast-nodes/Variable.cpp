#ifndef VAR_CPP
#define VAR_CPP

#include "ast-nodes/Variable.hpp"

#include <cstdio>

void clef::Variable::printf() const {
   std::printf("variable: \033[4m%.*s\033[24m (scope: id=%u): (type id=%u) = (value id=%u)", name().size(), name().begin(), +scopeName(), +type(), +val());
}

#endif //VAR_CPP