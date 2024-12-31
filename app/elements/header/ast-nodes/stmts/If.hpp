#pragma once
#ifndef IF_HPP
#define IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::If : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::IF; }

      If():Statement{} {}
      If(Expr* condition, Scope* procedure):Statement{OperatorID::IF,condition,procedure} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      Scope*& procedure() { return (Scope*&)_rhs;}
      const Scope* procedure() const { return (Scope*)_rhs;}
};

#endif //IF_HPP