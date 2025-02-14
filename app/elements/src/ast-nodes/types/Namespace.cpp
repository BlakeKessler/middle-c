#ifndef NAMESPACE_CPP
#define NAMESPACE_CPP

#include "ast-nodes/types/Namespace.hpp"

#include <cstdio>

void clef::Namespace::printf() const {
   __printf("namespace");
   std::printf(" {spec: id=%u}", +spec());
}

#endif //NAMESPACE_CPP