#pragma once
#ifndef ELSE_IF_HPP
#define ELSE_IF_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::ElseIf : public clef::Expression {
   private:

   public:
      ElseIf():Expression{} {}
      ElseIf(Operator* op, If* parentIf, If* elseif):Expression{op,parentIf,elseif} { assert(_op->isElseIf()); }

      If*& parentIf() { return (If*&)_lhs; }
      const If* parentIf() const { return (If*)_lhs; }
      If*& condition() { return (If*&)_rhs;}
      const If* condition() const { return (If*)_rhs;}
};

#endif //ELSE_IF_HPP