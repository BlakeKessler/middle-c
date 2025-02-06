#ifndef SWITCH_CPP
#define SWITCH_CPP

#include "ast-nodes/exprs/Switch.hpp"

#include <cstdio>

void clef::Switch::printf() const {
   std::printf("SWITCH (id=%u) {id=%u}", +condition(), +cases());
}

#endif //SWITCH_CPP