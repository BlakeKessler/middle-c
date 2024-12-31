#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "CLEF.hpp"
#include "Expression.hpp"

struct clef::Statement : public clef::Expression {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::STMT; }

      using Expression::Expression;
      // Statement():Expression{} {}
      // Statement(Expression& expr):Expression{expr} {}
};

#endif //STATEMENT_HPP