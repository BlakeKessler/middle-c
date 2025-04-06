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

uint mcsl::writef(File& file, const clef::SourceTokens& toks, char mode, FmtArgs args) {
   using namespace clef;
   assume((mode | CASE_BIT) == 's');
   uint charsPrinted = 0;
   for (const Token* it = toks.begin(); it < toks.end(); ++it) {
      charsPrinted += mcsl::writef(file, *it, mode, args);
      // mcsl::write(' '), ++charsPrinted;
      mcsl::write('\n'), ++charsPrinted;
   }
   return charsPrinted;
}

#endif //SOURCE_TOKENS_CPP