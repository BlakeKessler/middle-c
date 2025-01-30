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
      Loop(OpID loopType, index<Expr> condition, index<Scope> procedure):Statement{loopType,NodeType::EXPR,NodeType::SCOPE,condition,procedure} { debug_assert(isSimpleLoop(_op)); }
      Loop(index<ForLoopParams> params, index<Scope> procedure):Statement{OpID::FOR,NodeType::FOR_LOOP_PARAMS,NodeType::SCOPE,params,procedure} {}
      Loop(index<ForeachLoopParams> params, index<Scope> procedure):Statement{OpID::FOREACH,NodeType::FOREACH_LOOP_PARAMS,NodeType::SCOPE,params,procedure} {}

      Expr*& condition();
      const Expr* condition() const;
      ForLoopParams*& forParams();
      const ForLoopParams* forParams() const;
      ForeachLoopParams*& foreachParams();
      const ForeachLoopParams* foreachParams() const;
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs); }
      index<const Scope> procedure() const { return _rhs; }
};

#endif //LOOP_HPP