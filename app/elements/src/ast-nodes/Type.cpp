#ifndef TYPE_CPP
#define TYPE_CPP

#include "ast-nodes/Type.hpp"

mcsl::File& mcsl::write(File& file, const clef::Type& obj) {
   obj.printAs(file, FMT("type"));
   return file;
}

#endif //TYPE_CPP