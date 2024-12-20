#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "string.hpp"

struct clef::Identifier {
   private:
      mcsl::string _name;
      NodeType _type;
   public:
      Identifier():_name{},_type{} {}
      template<mcsl::str_t str_t> Identifier(const str_t& str):_name{str} {}

      mcsl::string& name() { return _name; }
      const mcsl::string& name() const { return _name; }
      NodeType type() const { return _type; }

      bool operator==(const Identifier& other) const { return _name == other._name; }
      template <mcsl::str_t str_t> bool operator==(const str_t& str) const { return _name == str; }
};

#endif //IDENTIFIER_HPP