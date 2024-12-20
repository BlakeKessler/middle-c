#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Statement {
   private:
      Expression* _contents;
      bool _isClosed;
   public:
      Statement():_contents{},_isClosed{} {}
      Statement(Expression* expr, const bool completeness):_contents{expr},_isClosed{completeness} {}

      Expression*& contents() { return _contents; }
      const Expression* contents() const { return _contents; }
      bool isClosed() const { return _isClosed; }

      void  open() { _isClosed = false; }
      void close() { _isClosed = true; }
}; 

#endif //STATEMENT_HPP