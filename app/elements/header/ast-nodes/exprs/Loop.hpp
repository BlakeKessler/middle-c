#pragma once
#ifndef LOOP_HPP
#define LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Loop : public clef::Expression {
   private:
   
   public:
      Loop():Expression{} {}
      Loop(Operator* loopType, ForLoopParams* params, Scope* procedure):Expression{loopType,params,procedure} { assert(_op->isForLoop()); }
      Loop(Operator* loopType, Expr* condition, Scope* procedure):Expression{loopType,condition,procedure} { assert(_op->isNonForLoop()); }

      Expr* condition();
      const Expr* condition() const;
      ForLoopParams* params();
      const ForLoopParams* params() const;
      Scope* procedure() { return (Scope*)_rhs; }
      const Scope* procedure() const { return (Scope*)_rhs; }
};

#endif //LOOP_HPP