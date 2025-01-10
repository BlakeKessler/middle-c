#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "SourceTokens.hpp"

class clef::Lexer {
   protected:
      static constexpr const mcsl::raw_str _nameof = "Lexer";
      static bool lexExpr(char*& curr, char* const end, mcsl::dyn_arr<clef::Token>& toks);
   public:
      static constexpr const auto& nameof() { return _nameof; }

      static SourceTokens LexSource(Source&& src);
      static inline SourceTokens LexFile(const char* path) { return LexSource(Source::readFile(path)); }


      static char parseChar(char*& curr, char* const end);
};

#endif //LEXER_HPP