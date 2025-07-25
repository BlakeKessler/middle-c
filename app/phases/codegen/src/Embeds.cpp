#ifndef CODE_GEN_EMBEDS_CPP
#define CODE_GEN_EMBEDS_CPP

#include "CodeGenerator.hpp"

void clef::CodeGenerator::initFile() {
   constexpr const static char fundTypedefsBuf[] = {
      #embed "app/embeds/fundTypedefs-LP64.c" //IntelliSense does not know about the embed directive
   };
   constexpr const mcsl::str_slice fundTypedefs = mcsl::str_slice::make(fundTypedefsBuf, sizeof(fundTypedefsBuf));
   out.write(fundTypedefs);
}

#endif