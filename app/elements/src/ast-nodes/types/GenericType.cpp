#ifndef GENERIC_TYPE_CPP
#define GENERIC_TYPE_CPP

#include "ast-nodes/types/GenericType.hpp"

mcsl::File& mcsl::write(File& file, const clef::GenericType& obj) {
   obj.printAs(file, FMT("generic type"));
   return file;
}

#endif //GENERIC_TYPE_CPP