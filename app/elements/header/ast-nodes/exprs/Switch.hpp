#pragma once
#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Switch : public clef::Expression {
   private:

   public:
      Switch():Expression{} {}
      Switch(Expr* cond, SwitchCases* cases):Expression{OperatorID::SWITCH,cond,cases} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      SwitchCases*& cases() { return (SwitchCases*&)_rhs;}
      const SwitchCases* cases() const { return (SwitchCases*)_rhs;}
};

#endif //SWITCH_HPP