#ifndef VAR_CPP
#define VAR_CPP

#include "ast-nodes/Variable.hpp"

#include "io.hpp"

void clef::Variable::printf() const {
   __printf("variable");
   mcsl::printf(mcsl::FMT(": (type id=%u) = (value id=%u)"), +type(), +val());
}

#endif //VAR_CPP