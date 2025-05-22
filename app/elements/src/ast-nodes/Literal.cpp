#ifndef LITERAL_CPP
#define LITERAL_CPP

#include "ast-nodes/Literal.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(mcsl::File& file, const clef::Literal& obj) {
   using namespace clef;
   file.printf(mcsl::FMT("%s literal: "), toString(obj.type()));
   switch (obj.type()) {
      case LitType::NONE: break;

      case LitType::POINTER: file.printf(mcsl::FMT("%r"), (const void*)obj); break;

      case LitType::UINT: file.printf(mcsl::FMT("%u"), (ulong)obj); break;
      case LitType::SINT: file.printf(mcsl::FMT("%i"), (slong)obj); break;
      case LitType::FLOAT: file.printf(mcsl::FMT("%f"), (flong)obj); break;

      case LitType::BOOL: file.printf(mcsl::FMT("%s"), (bool)obj); break;
      case LitType::CHAR: file.printf(mcsl::FMT("%c"), (char)obj); break;
      case LitType::STRING: [[fallthrough]];
      case LitType::INTERP_STR: [[fallthrough]];
      case LitType::FORMAT: [[fallthrough]];
      case LitType::REGEX: file.printf(mcsl::FMT("%s"), (const str_slice&)obj); break;

      case LitType::TYPEID: file.printf(mcsl::FMT("%u"), (clef::index<const Type>)obj); break;
   }
   return file;
}

#endif //LITERAL_CPP