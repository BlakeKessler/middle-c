#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "SourceTokens.hpp"

class clef::Lexer {
   protected:
      static constexpr const mcsl::raw_str _name = "Lexer";
   public:
      static constexpr const auto& name() { return _name; }

      static SourceTokens LexFile(const char* path);
      static SourceTokens LexSource(Source&& src);
};

#endif //LEXER_HPP