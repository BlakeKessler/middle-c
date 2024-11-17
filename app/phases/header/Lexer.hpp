#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "SourceTokens.hpp"

class clef::Lexer {
   protected:
      static constexpr const mcsl::raw_str _nameof = "Lexer";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      static SourceTokens LexSource(Source&& src);
      static inline SourceTokens LexFile(const char* path) { return LexSource(Source::readFile(path)); }


      static uint emplaceOps(mcsl::dyn_arr<Token>& toks, mcsl::raw_str_span tokstr);
};

#endif //LEXER_HPP