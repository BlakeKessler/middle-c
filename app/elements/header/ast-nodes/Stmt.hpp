#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expr.hpp"

struct clef::Stmt : public clef::Expr {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::STMT; }

      using Expr::Expr;
      Stmt(const Expr& other):Expr(other) {}
};

#endif //STATEMENT_HPP