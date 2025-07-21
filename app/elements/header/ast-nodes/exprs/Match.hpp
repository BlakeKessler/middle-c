#pragma once
#ifndef MATCH_HPP
#define MATCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Stmt.hpp"

struct clef::Match : public clef::Stmt {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::MATCH; }
      static constexpr OpID pseudoOpID() { return OpID::MATCH; }

      Match():Stmt{} {}
      Match(index<Expr> cond, index<MatchCases> cases):Stmt{OpID::SWITCH,NodeType::EXPR,NodeType::MATCH_CASES,cond,cases} {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<MatchCases>& cases() { return reinterpret_cast<index<MatchCases>&>(_rhs); }
      index<const MatchCases> cases() const { return _rhs; }
};

namespace mcsl {
   File& write(File&, const clef::Match&);
};

#endif //MATCH_HPP