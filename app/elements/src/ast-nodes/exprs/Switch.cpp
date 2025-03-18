#ifndef SWITCH_CPP
#define SWITCH_CPP

#include "ast-nodes/exprs/Switch.hpp"

#include "io.hpp"

void clef::Switch::printf() const {
   mcsl::printf(mcsl::FMT("SWITCH (id=%u) {id=%u}"), +condition(), +cases());
}

#endif //SWITCH_CPP