#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "SourceTokens.hpp"

class clef::Lexer {
   protected:
      static bool lexChar(char*& curr, char* const end, mcsl::dyn_arr<clef::Token>& toks);
      static bool lexStr(char*& curr, char* const end, char* const tokBegin, mcsl::dyn_arr<clef::Token>& toks);
      static bool lexInterpStr(char*& curr, char* const end, char* const tokBegin, mcsl::dyn_arr<clef::Token>& toks);
      static bool lexExpr(char*& curr, char* const end, mcsl::dyn_arr<clef::Token>& toks);
   public:
      static SourceTokens LexSource(Source&& src);
      static inline SourceTokens LexFile(const char* path) { return LexSource(Source::readFile(path)); }


      static char parseChar(char*& curr, char* const end);
};

#endif //LEXER_HPP