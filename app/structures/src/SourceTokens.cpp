#ifndef SOURCE_TOKENS_CPP
#define SOURCE_TOKENS_CPP

#include "SourceTokens.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::SourceTokens& toks) {
   using namespace clef;
   for (const Token* it = toks.begin(); it < toks.end(); ++it) {
      write(file, *it);
      write(file, '\n');
   }
   return file;
}

#endif //SOURCE_TOKENS_CPP