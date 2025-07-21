#pragma once
#ifndef DO_WHILE_HPP
#define DO_WHILE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Stmt.hpp"

struct clef::DoWhileLoop : public clef::Stmt {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::DO_WHILE_LOOP; }
      static constexpr OpID pseudoOpID() { return OpID::DO_WHILE; }

      DoWhileLoop():Stmt{} {}
      DoWhileLoop(index<Expr> condition, index<Scope> procedure):Stmt{OpID::DO_WHILE,NodeType::EXPR,NodeType::SCOPE,condition,procedure} {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs); }
      index<const Scope> procedure() const { return _rhs; }
};

namespace mcsl {
   File& write(File&, const clef::DoWhileLoop&);
};

#endif //DO_WHILE_HPP