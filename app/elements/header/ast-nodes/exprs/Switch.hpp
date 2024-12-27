#pragma once
#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Switch : public clef::Expression {
   private:

   public:
      Switch():Expression{} {}
      Switch(Operator* op, Expr* cond, SwitchCases* cases):Expression{op,cond,cases} { assert(_op->isSwitch()); }

      Expr* condition() { return (Expr*)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      SwitchCases* cases() { return (SwitchCases*)_rhs;}
      const SwitchCases* cases() const { return (SwitchCases*)_rhs;}
};

#endif //SWITCH_HPP