#ifndef VAR_PARAM_CPP
#define VAR_PARAM_CPP

#include "ast-nodes/types/VariadicParameter.hpp"

#include <cstdio>

void clef::VariadicParam::printf() const {
   std::printf("variadic parameter: \033[4m%.*s\033[24m (scope: id=%u)", name().size(), name().begin(), +scopeName());
}

#endif //VAR_PARAM_CPP