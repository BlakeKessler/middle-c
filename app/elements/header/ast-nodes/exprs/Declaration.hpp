#pragma once
#ifndef DECL_HPP
#define DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Declaration : public clef::Expression {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::DECL; }

      Declaration():Expression{} {}
      Declaration(Type* type, Identifier* name):Expression{OpID::DECL,NodeType::TYPE,NodeType::IDEN,type,name} {}

      Type*& type() { return (Type*&)_lhs; }
      const Type* type() const { return (const Type*)_lhs; }
      Identifier*& name() { return (Identifier*&)_rhs; }
      const Identifier* name() const { return (const Identifier*)_rhs; }
};

#endif //DECL_HPP