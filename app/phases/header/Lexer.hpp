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
};

#endif //LEXER_HPP