#pragma once
#ifndef FOREACH_LOOP_HPP
#define FOREACH_LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/exprs/Declaration.hpp"
#include "ast-nodes/Scope.hpp"

struct clef::ForeachLoop : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FOREACH_LOOP; }
      static constexpr OpID pseudoOpID() { return OpID::FOREACH; }

      ForeachLoop():Statement{} {}
      ForeachLoop(index<Decl> iterator, index<Expr> target, index<Scope> procedure):Statement{OpID::FOREACH, iterator, target, procedure} {}

      index<Decl>& iterator() { return reinterpret_cast<index<Decl>&>(_lhs); }
      index<const Decl> iterator() const { return _lhs; }
      index<Expr>& target() { return reinterpret_cast<index<Expr>&>(_rhs); }
      index<const Expr> target() const { return _rhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_extras[0]); }
      index<const Scope> procedure() const { return _extras[0]; }
};

namespace mcsl {
   File& write(File&, const clef::ForeachLoop&);
};

#endif //FOREACH_LOOP_HPP