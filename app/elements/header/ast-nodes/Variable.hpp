#pragma once
#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Variable : public clef::Identifier {
   private:
      Type* _type;

   public:
      static constexpr NodeType nodeType() { return NodeType::VAR; }

      Variable():Identifier{},_type{} {}
      Variable(Type* type):Identifier{},_type{type} {}
      Variable(Type* type, Identifier* name):Identifier{name?*name:Identifier{}},_type{type} {}

      Type*& type() { return _type; }
      const Type* type() const { return _type; }
};

#endif //VARIABLE_HPP