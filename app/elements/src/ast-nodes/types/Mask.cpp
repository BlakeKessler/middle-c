#ifndef MASK_CPP
#define MASK_CPP

#include "ast-nodes/types/Mask.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Mask& obj) {
   obj.printAs(file, FMT("mask"));
   file.printf(FMT(" : (base: id=%u) {members: id=%u}"), +obj.baseType(), +obj.enumerators());
   return file;
}

#endif //MASK_CPP