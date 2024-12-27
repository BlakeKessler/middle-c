#ifndef SOURCE_TOKENS_CPP
#define SOURCE_TOKENS_CPP

#include "SourceTokens.hpp"

void clef::SourceTokens::printf() const {
   for (uint i = 0; i < _tokLines.size(); ++i) {
      std::printf("\033[4mline %u:\033[24m ", i + 1);
      for (uint j = 0; j < _tokLines[i].size(); ++j) {
         _tokLines[i][j].printf();
         std::printf(" ");
      }
      std::printf("\n");
   }
}

#endif //SOURCE_TOKENS_CPP