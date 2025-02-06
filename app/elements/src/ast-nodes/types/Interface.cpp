#ifndef INTERFACE_CPP
#define INTERFACE_CPP

#include "ast-nodes/types/Interface.hpp"

#include <cstdio>

void clef::Interface::printf() const {
   std::printf("interface: \033[4m%.*s\033[24m (scope: id=%u) {spec: id=%u}", name().size(), name().begin(), +scopeName(), +spec());
}

#endif //INTERFACE_CPP