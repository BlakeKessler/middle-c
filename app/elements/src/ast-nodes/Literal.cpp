#ifndef LITERAL_CPP
#define LITERAL_CPP

#include "ast-nodes/Literal.hpp"
#include "pretty-print.hpp"

#include <cstdio>

void clef::Literal::printf() const {
   std::printf("%s literal: ", toString(type()));
   switch (type()) {
      case LitType::NONE: break;

      case LitType::POINTER: std::printf("%p", _ptrLit); break;

      case LitType::UINT: std::printf("%lu", _uintLit); break;
      case LitType::SINT: std::printf("%ld", _sintLit); break;
      case LitType::FLOAT: std::printf("%lf", _floatLit); break;

      case LitType::CHAR: std::printf("%c", _charLit); break;
      case LitType::STRING: [[fallthrough]];
      case LitType::INTERP_STR: [[fallthrough]];
      case LitType::FORMAT: [[fallthrough]];
      case LitType::REGEX: std::printf("%.*s", _regexLit.size(), _regexLit.begin()); break;

      case LitType::TYPEID: std::printf("%u", +_typeid); break;
   }
}

#endif //LITERAL_CPP