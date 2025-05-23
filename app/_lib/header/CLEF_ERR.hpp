#pragma once
#ifndef CLEF_ERR_HPP
#define CLEF_ERR_HPP

#include "CLEF.hpp"

#include "io.hpp"

namespace clef {
   extern const mcsl::str_slice ERR_MSG_ARR[];
   [[noreturn]] void throwError(const clef::ErrCode code, const mcsl::str_slice formatStr, auto&&... args);
   [[noreturn]] void throwError(const clef::ErrCode code, const uint lineNum, const mcsl::str_slice formatStr, auto&&... args);
}

//!CLEF formatted error thrower
[[noreturn]] void clef::throwError(const clef::ErrCode code, const mcsl::str_slice formatStr, auto&&... args) {
   mcsl::printf(mcsl::FMT("\n"));
   mcsl::stdout.flush();
   mcsl::err_printf(mcsl::FMT("\033[31;1;4mCLEF ERROR:\033[0m %s"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(formatStr, args...);
   mcsl::err_printf(mcsl::FMT("\n"));
   mcsl::stderr.flush();
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}
//!CLEF formatted error thrower with line num
[[noreturn]] void clef::throwError(const clef::ErrCode code, const uint lineNum, const mcsl::str_slice formatStr, auto&&... args) {
   mcsl::printf(mcsl::FMT("\n"));
   mcsl::stdout.flush();
   mcsl::err_printf(mcsl::FMT("\033[31;1;4mCLEF ERROR:\033[0m %s"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(formatStr, args...);
   mcsl::err_printf(mcsl::FMT(" \033[35m(line %u)\033[0m\n"), lineNum);
   mcsl::stderr.flush();
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}

#endif //CLEF_ERR_HPP