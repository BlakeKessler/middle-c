#pragma once
#ifndef IF_HPP
#define IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::If : public clef::Expression {
   private:

   public:
      If():Expression{} {}
      If(Operator* op, Expr* condition, Scope* procedure):Expression{op,condition,procedure} { assert(_op->isIf()); }

      Expr* condition() { return (Expr*)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      Scope* procedure() { return (Scope*)_rhs;}
      const Scope* procedure() const { return (Scope*)_rhs;}
};

#endif //IF_HPP