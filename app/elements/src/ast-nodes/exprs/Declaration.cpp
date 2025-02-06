#ifndef DECL_CPP
#define DECL_CPP

#include "ast-nodes/exprs/Declaration.hpp"

#include <cstdio>

void clef::Decl::printf() const {
   std::printf("declaration (name: id=%u) : (type: id=%u)", +name(), +type());
}

#endif //DECL_CPP