#ifndef INTERFACE_CPP
#define INTERFACE_CPP

#include "ast-nodes/types/Interface.hpp"

#include "io.hpp"

void clef::Interface::printf() const {
   __printf("interface");
   mcsl::printf(mcsl::FMT(" {spec: id=%u}"), +spec());
}

#endif //INTERFACE_CPP