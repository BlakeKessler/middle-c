#pragma once
#ifndef CLEF_ERR_HPP
#define CLEF_ERR_HPP

#include "CLEF.hpp"

#include "io.hpp"

namespace clef {
   extern const mcsl::str_slice ERR_MSG_ARR[];
   [[noreturn]] void internalError(const clef::ErrCode code, const mcsl::str_slice formatStr, const mcsl::Printable auto&... args);
}

//!CLEF formatted error thrower
[[noreturn]] void clef::internalError(const clef::ErrCode code, const mcsl::str_slice formatStr, const mcsl::Printable auto&... args) {
   // mcsl::printf(mcsl::FMT("\n"));
   mcsl::stdout.flush();
   mcsl::err_printf(mcsl::FMT("\033[31;1;4mCLEF INTERNAL ERROR:\033[0m "));
   mcsl::err_printf(formatStr, std::forward<decltype(args)>(args)...);
   mcsl::err_printf(FMT(" [%s]"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(mcsl::FMT("\nPlease make a bug report.\n"));
   mcsl::stderr.flush();
   #ifdef NDEBUG
      std::exit(EXIT_FAILURE);
   #else
      std::abort();
   #endif
}

#endif //CLEF_ERR_HPP