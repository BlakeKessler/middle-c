#pragma once
#ifndef LOOP_HPP
#define LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Loop : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::LOOP; }

      Loop():Statement{} {}
      Loop(OperatorID loopType, Expr* condition, Scope* procedure):Statement{loopType,condition,procedure} { assert(isSimpleLoop(_op)); }
      Loop(ForLoopParams* params, Scope* procedure):Statement{OperatorID::FOR,params,procedure} {}
      Loop(ForeachLoopParams* params, Scope* procedure):Statement{OperatorID::FOREACH,params,procedure} {}

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