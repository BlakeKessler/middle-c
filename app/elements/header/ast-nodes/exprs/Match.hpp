#pragma once
#ifndef MATCH_HPP
#define MATCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Match : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::MATCH; }

      Match():Statement{} {}
      Match(Expr* cond, MatchCases* cases):Statement{OpID::SWITCH,NodeType::EXPR,NodeType::MATCH_CASES,cond,cases} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      MatchCases*& cases() { return (MatchCases*&)_rhs;}
      const MatchCases* cases() const { return (MatchCases*)_rhs;}
};

#endif //MATCH_HPP