#ifndef DECL_CPP
#define DECL_CPP

#include "ast-nodes/exprs/Declaration.hpp"

#include "io.hpp"

void clef::Decl::printf() const {
   mcsl::printf(mcsl::FMT("declaration (name: id=%u) : (type: id=%u)"), +name(), +type());
}

#endif //DECL_CPP