#ifndef INTERFACE_CPP
#define INTERFACE_CPP

#include "ast-nodes/types/Interface.hpp"

#include <cstdio>

void clef::Interface::printf() const {
   __printf("interface");
   std::printf(" {spec: id=%u}", +spec());
}

#endif //INTERFACE_CPP