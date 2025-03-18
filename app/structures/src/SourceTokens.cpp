#ifndef SOURCE_TOKENS_CPP
#define SOURCE_TOKENS_CPP

#include "SourceTokens.hpp"

#include "io.hpp"

void clef::SourceTokens::printf() const {
   for (const Token* it = _tokens.begin(); it < _tokens.end(); ++it) {
      it->printf();
      mcsl::printf(mcsl::FMT("\n"));
   }
}

#endif //SOURCE_TOKENS_CPP