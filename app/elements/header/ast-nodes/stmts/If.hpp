#pragma once
#ifndef IF_HPP
#define IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::If : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::IF; }

      If():Statement{} {}
      If(Expr* condition, Scope* procedure, If* elseStmt = {}):Statement{OperatorID::IF,NodeType::EXPR,NodeType::SCOPE,NodeType::IF,condition,procedure,elseStmt} {}

      Expr*& condition() { return (Expr*&)_lhs; }
      const Expr* condition() const { return (Expr*)_lhs; }
      Scope*& procedure() { return (Scope*&)_rhs;}
      const Scope* procedure() const { return (Scope*)_rhs;}
      If*& elseStmt() { return (If*&)_rhs;}
      const If* elseStmt() const { return (If*)_rhs;}
};

#endif //IF_HPP