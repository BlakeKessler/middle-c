#pragma once
#ifndef MATCH_HPP
#define MATCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Match : public clef::Expression {
   private:

   public:
      Match():Expression{} {}
      Match(Expr* cond, MatchCases* cases):Expression{OperatorID::MATCH,cond,cases} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      MatchCases*& cases() { return (MatchCases*&)_rhs;}
      const MatchCases* cases() const { return (MatchCases*)_rhs;}
};

#endif //MATCH_HPP