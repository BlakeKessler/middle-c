#pragma once
#ifndef ELSE_HPP
#define ELSE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Else : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::ELSE; }

      Else():Statement{} {}
      Else(If* ifStmt, Scope* proc):Statement{OperatorID::ELSE,ifStmt,proc} {}
      Else(ElseIf* ifStmt, Scope* proc):Statement{OperatorID::ELSE,ifStmt,proc} {}

      Stmt*& parentIf() { return (Stmt*&)_lhs; }
      const Stmt* parentIf() const { return (Stmt*)_lhs; }
      Scope*& procedure() { return (Scope*&)_rhs;}
      const Scope* procedure() const { return (Scope*)_rhs;}
};

#endif //ELSE_HPP