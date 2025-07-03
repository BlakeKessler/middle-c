#pragma once
#ifndef CLEF_ERR_HPP
#define CLEF_ERR_HPP

#include "CLEF.hpp"

#include "io.hpp"

namespace clef {
   extern const mcsl::str_slice ERR_MSG_ARR[];
   [[noreturn]] void throwError(const clef::ErrCode code, const mcsl::str_slice formatStr, auto&&... args);
   [[noreturn]] void throwError(const clef::ErrCode code, const uint lineNum, const mcsl::str_slice line, const mcsl::str_slice tok, const mcsl::str_slice path, const mcsl::str_slice formatStr, auto&&... args);
}

//!CLEF formatted error thrower
[[noreturn]] void clef::throwError(const clef::ErrCode code, const mcsl::str_slice formatStr, auto&&... args) {
   // mcsl::printf(mcsl::FMT("\n"));
   mcsl::stdout.flush();
   mcsl::err_printf(mcsl::FMT("\033[31;1;4mCLEF ERROR:\033[0m "));
   mcsl::err_printf(formatStr, args...);
   mcsl::err_printf(FMT(" [%s]"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(mcsl::FMT("\n"));
   mcsl::stderr.flush();
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}
//!CLEF formatted error thrower with line num
[[noreturn]] void clef::throwError(const clef::ErrCode code, const uint lineNum, const mcsl::str_slice line, const mcsl::str_slice tok, const mcsl::str_slice path, const mcsl::str_slice formatStr, auto&&... args) {
   // mcsl::printf(mcsl::FMT("\n"));
   mcsl::stdout.flush();
   // mcsl::err_printf(mcsl::FMT("\033[31;1;4mCLEF ERROR:\033[0m %s"), ERR_MSG_ARR[+code]);
   // mcsl::err_printf(formatStr, args...);
   // mcsl::err_printf(mcsl::FMT(" \033[35m(line %u)\033[0m\n"), lineNum);
   
   debug_assert(tok.begin() >= line.begin());
   debug_assert(tok.end() <= line.end());
   const mcsl::str_slice beforeTok = mcsl::str_slice::make(line.begin(), tok.begin());
   const mcsl::str_slice afterTok = mcsl::str_slice::make(tok.end(), line.end());
   
   mcsl::err_printf(mcsl::FMT("\033[1m%s:%u:%u: \033[31mERROR:\033[39m "), path, lineNum, beforeTok.size() + 1);
   mcsl::err_printf(formatStr, args...);
   mcsl::err_printf(mcsl::FMT(" [%s, %u]\033[22m\n"), ERR_MSG_ARR[+code], +code);
   uint spaces = mcsl::err_printf(mcsl::FMT("    %u "), lineNum);
   uint spaces2 = mcsl::err_printf(mcsl::FMT("| %s"), beforeTok) - 1;
   mcsl::err_printf(mcsl::FMT("\033[4m%s\033[24m%s"), tok, afterTok);
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

#endif //CLEF_ERR_HPP