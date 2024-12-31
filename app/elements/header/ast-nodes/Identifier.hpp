#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"

struct clef::Identifier {
   private:
      const mcsl::raw_str_span _name;
      Identifier* _scopeName;
   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(const mcsl::raw_str_span name = {}, Identifier* scopeName = {}):_name{name},_scopeName{scopeName} {}

      Identifier*& scopeName() { return _scopeName; }
      const Identifier* scopeName() const { return _scopeName; }

      KeywordID keywordID() const;


      bool operator==(const Identifier& other) const;
      bool operator!=(const Identifier& other) const;
      bool operator<=>(const Identifier& other) const;
      
      operator const mcsl::raw_str_span() const { return _name; } //unqualified name
};

#endif //IDENTIFIER_HPP