#pragma once
#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Variable : public clef::Identifier {
   private:
      Type* _type;
      Expr* _val;
   public:
      static constexpr NodeType nodeType() { return NodeType::VAR; }

      Variable(Type* type = {}, Identifier* name = {}, Expr* val = {}):Identifier{name?*name:Identifier{}},_type{type},_val{val} {}

      Type*& type() { return _type; }
      const Type* type() const { return _type; }

      Expr*& val() { return _val; }
      const Expr* val() const { return _val; }
};

#endif //VARIABLE_HPP