#ifndef LITERAL_CPP
#define LITERAL_CPP

#include "ast-nodes/Literal.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

void clef::Literal::printf() const {
   mcsl::printf(mcsl::FMT("%s literal: "), toString(type()));
   switch (type()) {
      case LitType::NONE: break;

      case LitType::POINTER: mcsl::printf(mcsl::FMT("%r"), _ptrLit); break;

      case LitType::UINT: mcsl::printf(mcsl::FMT("%u"), _uintLit); break;
      case LitType::SINT: mcsl::printf(mcsl::FMT("%i"), _sintLit); break;
      case LitType::FLOAT: mcsl::printf(mcsl::FMT("%f"), _floatLit); break;

      case LitType::CHAR: mcsl::printf(mcsl::FMT("%c"), _charLit); break;
      case LitType::STRING: [[fallthrough]];
      case LitType::INTERP_STR: [[fallthrough]];
      case LitType::FORMAT: [[fallthrough]];
      case LitType::REGEX: mcsl::printf(mcsl::FMT("%s"), _regexLit.size(), _regexLit.begin()); break;

      case LitType::TYPEID: mcsl::printf(mcsl::FMT("%u"), +_typeid); break;
   }
}

#endif //LITERAL_CPP