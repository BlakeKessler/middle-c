#pragma once
#ifndef FOR_LOOP_HPP
#define FOR_LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/Scope.hpp"

struct clef::ForLoop : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FOR_LOOP; }
      static constexpr OpID pseudoOpID() { return OpID::FOR; }

      ForLoop():Statement{} {}
      ForLoop(index<Expr> decl, index<Expr> cond, index<Expr> inc, index<Scope> procedure): Statement{OpID::FOR, decl, cond, inc, procedure} {}

      index<Expr>& decl() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> decl() const { return _lhs; }
      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_rhs); }
      index<const Expr> condition() const { return _rhs; }
      index<Expr>& increment() { return reinterpret_cast<index<Expr>&>(_extras[0]); }
      index<const Expr> increment() const { return _extras[0]; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_extras[1]); }
      index<const Scope> procedure() const { return _extras[1]; }
};

namespace mcsl {
   File& write(File&, const clef::ForLoop&);
};

#endif //FOR_LOOP_HPP