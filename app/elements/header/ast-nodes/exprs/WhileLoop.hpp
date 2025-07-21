#pragma once
#ifndef WHILE_LOOP_HPP
#define WHILE_LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Stmt.hpp"

struct clef::WhileLoop : public clef::Stmt {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::WHILE_LOOP; }
      static constexpr OpID pseudoOpID() { return OpID::WHILE; }

      WhileLoop():Stmt{} {}
      WhileLoop(index<Expr> condition, index<Scope> procedure):Stmt{OpID::WHILE,NodeType::EXPR,NodeType::SCOPE,condition,procedure} {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs); }
      index<const Scope> procedure() const { return _rhs; }
};

namespace mcsl {
   File& write(File&, const clef::WhileLoop&);
};

#endif //WHILE_LOOP_HPP