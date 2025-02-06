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

      void printf() const { Expr::printf(); }
};

#endif //STATEMENT_HPP