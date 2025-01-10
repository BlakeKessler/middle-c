#ifndef SOURCE_TOKENS_CPP
#define SOURCE_TOKENS_CPP

#include "SourceTokens.hpp"

void clef::SourceTokens::printf() const {
   for (const Token* it = _tokens.begin(); it < _tokens.end(); ++it) {
      it->printf();
      std::printf("\n");
   }
}

#endif //SOURCE_TOKENS_CPP