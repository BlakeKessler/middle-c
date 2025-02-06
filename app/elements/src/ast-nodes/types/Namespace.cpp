#ifndef NAMESPACE_CPP
#define NAMESPACE_CPP

#include "ast-nodes/types/Namespace.hpp"

#include <cstdio>

void clef::Namespace::printf() const {
   std::printf("namespace: \033[4m%.*s\033[24m (scope: id=%u) {spec: id=%u}", name().size(), name().begin(), +scopeName(), +spec());
}

#endif //NAMESPACE_CPP