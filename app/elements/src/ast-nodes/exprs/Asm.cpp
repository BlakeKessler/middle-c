#ifndef ASM_CPP
#define ASM_CPP

#include "ast-nodes/exprs/Asm.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Asm& obj) {
   file.write(FMT("ASM (not implemented)"));
   return file;
   (void)obj;
}

#endif //ASM_CPP