#pragma once
#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Switch : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::SWITCH; }

      Switch():Statement{} {}
      Switch(Expr* cond, SwitchCases* cases):Statement{OpID::SWITCH,NodeType::EXPR,NodeType::SWITCH_CASES,cond,cases} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      SwitchCases*& cases() { return (SwitchCases*&)_rhs;}
      const SwitchCases* cases() const { return (SwitchCases*)_rhs;}
};

#endif //SWITCH_HPP