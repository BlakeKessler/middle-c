#pragma once
#ifndef ELSE_IF_HPP
#define ELSE_IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::ElseIf : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::ELSE_IF; }

      ElseIf():Statement{} {}
      ElseIf(If* parentIf, If* elseif):Statement{OperatorID::ELIF,parentIf,elseif} {}

      Stmt*& parentIf() { return (Stmt*&)_lhs; }
      const Stmt* parentIf() const { return (Stmt*)_lhs; }
      If*& ifStmt() { return (If*&)_rhs;}
      const If* ifStmt() const { return (If*)_rhs;}
};

#endif //ELSE_IF_HPP