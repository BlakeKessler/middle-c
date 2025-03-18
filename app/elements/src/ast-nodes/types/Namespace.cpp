#ifndef NAMESPACE_CPP
#define NAMESPACE_CPP

#include "ast-nodes/types/Namespace.hpp"

#include "io.hpp"

void clef::Namespace::printf() const {
   __printf("namespace");
   mcsl::printf(mcsl::FMT(" {spec: id=%u}"), +spec());
}

#endif //NAMESPACE_CPP