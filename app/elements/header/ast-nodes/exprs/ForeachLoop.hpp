#pragma once
#ifndef FOREACH_LOOP_HPP
#define FOREACH_LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::ForeachLoop : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FOREACH_LOOP; }

      ForeachLoop():Statement{} {}
      ForeachLoop(index<ForeachLoopParams> params, index<Scope> procedure):Statement{OpID::FOREACH,NodeType::FOREACH_LOOP_PARAMS,NodeType::SCOPE,params,procedure} {}

      index<ForeachLoopParams>& params() { return reinterpret_cast<index<ForeachLoopParams>&>(_lhs); }
      index<const ForeachLoopParams> params() const { return _lhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs); }
      index<const Scope> procedure() const { return _rhs; }
};

#endif //FOREACH_LOOP_HPP