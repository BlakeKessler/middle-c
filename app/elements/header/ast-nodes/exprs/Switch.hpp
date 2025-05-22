#pragma once
#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Switch : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::SWITCH; }
      static constexpr OpID pseudoOpID() { return OpID::SWITCH; }

      Switch():Statement{} {}
      Switch(index<Expr> cond, index<SwitchCases> cases):Statement{OpID::SWITCH,NodeType::EXPR,NodeType::SWITCH_CASES,cond,cases} {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<SwitchCases>& cases() { return reinterpret_cast<index<SwitchCases>&>(_rhs);}
      index<const SwitchCases> cases() const { return _rhs;}
};

namespace mcsl {
   File& write(File&, const clef::Switch&);
};

#endif //SWITCH_HPP