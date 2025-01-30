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
      Match(index<Expr> cond, index<MatchCases> cases):Statement{OpID::SWITCH,NodeType::EXPR,NodeType::MATCH_CASES,cond,cases} {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<MatchCases>& cases() { return reinterpret_cast<index<MatchCases>&>(_rhs); }
      index<const MatchCases> cases() const { return _rhs; }
};

#endif //MATCH_HPP