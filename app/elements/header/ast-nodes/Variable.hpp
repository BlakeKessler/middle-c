#pragma once
#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Variable : public clef::Identifier {
   private:
      index<Type> _type;
      index<Expr> _val;
   public:
      static constexpr NodeType nodeType() { return NodeType::VAR; }

      Variable(index<Type> type = {}, Identifier name = {}, index<Expr> val = {}):Identifier{name},_type{type},_val{val} {}

      index<Type>& type() { return _type; }
      index<const Type> type() const { return _type; }

      index<Expr>& val() { return _val; }
      index<const Expr> val() const { return _val; }
};

namespace mcsl {
   File& write(File&, const clef::Variable&);
};

#endif //VARIABLE_HPP