#pragma once
#ifndef IF_HPP
#define IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::If : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::IF; }
      static constexpr OpID pseudoOpID() { return OpID::IF; }

      If():Statement{} {}
      If(index<Expr> condition, index<Scope> procedure, index<If> elseStmt = {}):Statement{
         OpID::IF,NodeType::EXPR,NodeType::SCOPE,NodeType::IF,
         condition,procedure,elseStmt
      } {}

      index<Expr>& condition() { return reinterpret_cast<index<Expr>&>(_lhs); }
      index<const Expr> condition() const { return _lhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs);}
      index<const Scope> procedure() const { return _rhs;}
      index<If>& elseStmt() { return reinterpret_cast<index<If>&>(_extras[0]);}
      index<const If> elseStmt() const { return _extras[0];}
};

namespace mcsl {
   File& write(File&, const clef::If&);
};

#endif //IF_HPP