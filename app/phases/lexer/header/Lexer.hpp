#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "SourceTokens.hpp"

class clef::Lexer {
   private:
      Source src;

      char* const begin;
      char* const end;
      char* curr;
      char* tokBegin;

      uint lineIndex; //current line number (0-indexed)

   protected:
      char parseChar();

      Token lexChar();
      Token lexStr();
      Token lexInterpStr();
      Token lexExpr();
      Token lexFunclikeMacroArgs();
   public:
      Lexer(Source&& s):src{std::move(s)}, begin{src.buf().begin()}, end{src.buf().end()}, curr{begin}, tokBegin{curr}, lineIndex(0) {}
      static Lexer fromFile(const char* path) { return Lexer(Source::readFile(path)); }

      bool done() const { return curr >= end; }
      Token nextToken();
      mcsl::str_slice currLine();
      uint lineNum() { return lineIndex + 1; }
      mcsl::str_slice prevTokStr() { return {tokBegin, curr}; }
      const mcsl::str_slice path() const { return src.path(); }
};

#endif //LEXER_HPP