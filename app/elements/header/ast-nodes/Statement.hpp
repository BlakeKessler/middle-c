#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Statement {
   private:
      Expression* _contents;
   public:
      Statement():_contents{} {}
      Statement(Expression* expr):_contents{expr} {}

      Expression*& contents() { return _contents; }
      const Expression* contents() const { return _contents; }
}; 

#endif //STATEMENT_HPP