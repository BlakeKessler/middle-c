#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Statement : public clef::Expression {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::STMT; }

      using Expression::Expression;
      Statement(const Expr& other):Expr(other) {}
};

#endif //STATEMENT_HPP