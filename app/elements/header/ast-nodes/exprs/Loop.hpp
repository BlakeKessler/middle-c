#pragma once
#ifndef LOOP_HPP
#define LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Loop : public clef::Expression {
   private:
   
   public:
      Loop():Expression{} {}
      Loop(OperatorID loopType, Expr* condition, Scope* procedure):Expression{loopType,condition,procedure} { assert(isSimpleLoop(_op)); }
      Loop(ForLoopParams* params, Scope* procedure):Expression{OperatorID::FOR,params,procedure} {}
      Loop(ForeachLoopParams* params, Scope* procedure):Expression{OperatorID::FOREACH,params,procedure} {}

      Expr*& condition();
      const Expr* condition() const;
      ForLoopParams*& forParams();
      const ForLoopParams* forParams() const;
      ForeachLoopParams*& foreachParams();
      const ForeachLoopParams* foreachParams() const;
      Scope*& procedure() { return (Scope*&)_rhs; }
      const Scope* procedure() const { return (Scope*)_rhs; }
};

#endif //LOOP_HPP