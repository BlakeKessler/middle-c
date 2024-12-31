#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"

struct clef::Identifier {
   private:
      Decl* _decl;
      const mcsl::raw_str_span _name;
   public:
      Identifier():_decl{},_name{} {}
      Identifier(const char* str, const uint len, Decl* decl = nullptr):_decl{decl},_name{const_cast<char*>(str), len} {}

      KeywordID keywordID() const;


      bool operator==(const Identifier& other) const;
      bool operator!=(const Identifier& other) const;
      bool operator<=>(const Identifier& other) const;

      operator mcsl::raw_str_span() const { return _name; }
};

#endif //IDENTIFIER_HPP