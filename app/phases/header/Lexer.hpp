#pragma once
#ifndef LEXER_HPP
#define LEXER_HPP

#include "CLEF.hpp"

#include "Source.hpp"
#include "Token.hpp"

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
      static Lexer fromFile(const mcsl::str_slice path) { return Lexer(Source::readFile(path)); }

      bool done() const { return curr >= end; }
      Token nextToken();
      void rewindTo(char*);
      mcsl::str_slice currLine();
      uint lineNum() { return lineIndex + 1; }
      mcsl::str_slice prevTokStr() { return {tokBegin, curr}; }
      const mcsl::str_slice path() const { return src.path(); }

      //error logging
      void logError [[noreturn]] (Token tok, const clef::ErrCode code, const mcsl::str_slice formatStr, const mcsl::Printable auto&... args);
};


#pragma region inlinesrc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
void clef::Lexer::logError [[noreturn]] (Token tok, const clef::ErrCode code, const mcsl::str_slice formatStr, const mcsl::Printable auto&... args) {
   const mcsl::str_slice tokStr = tok.tokStr();

   uint i = lineIndex;
   do {
      const mcsl::str_slice tmp = src.line(i);
      if (tokStr.begin() >= tmp.begin()) {
         debug_assert(tokStr.end() <= tmp.end());
         break;
      }
      --i;
   } while (--i);
   const mcsl::str_slice line = src.line(i);
   debug_assert(tokStr.begin() >= line.begin());
   debug_assert(tokStr.end() <= line.end());

   mcsl::stdout.flush();
   
   const mcsl::str_slice beforeTok = mcsl::str_slice::make(line.begin(), tokStr.begin());
   const mcsl::str_slice afterTok = mcsl::str_slice::make(tokStr.end(), line.end());
   
   mcsl::err_printf(mcsl::FMT("\033[1m%s:%u:%u: \033[31mERROR:\033[39m "), path(), lineNum(), beforeTok.size() + 1);
   mcsl::err_printf(formatStr, args...);
   mcsl::err_printf(mcsl::FMT(" [%s, %u]\033[22m\n"), ERR_MSG_ARR[+code], +code);
   uint spaces = mcsl::err_printf(mcsl::FMT("    %u "), lineNum());
   uint spaces2 = mcsl::err_printf(mcsl::FMT("| %s"), beforeTok) - 1;
   mcsl::err_printf(mcsl::FMT("\033[4m%s\033[24m%s"), tokStr, afterTok);
   if (afterTok.back() != '\n') {
      mcsl::stderr.write('\n');
   }
   mcsl::stderr.write(' ', spaces);
   mcsl::stderr.write('|');
   mcsl::stderr.write(' ', spaces2);
   mcsl::err_printf(mcsl::FMT("\033[1;32m^\033[22;39m\n"));

   mcsl::stderr.flush();
   
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}
#pragma GCC diagnostic pop
#pragma endregion inlinesrc

#endif //LEXER_HPP