#pragma once
#ifndef FOR_LOOP_HPP
#define FOR_LOOP_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::ForLoop : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FOR_LOOP; }

      ForLoop():Statement{} {}
      ForLoop(index<ForLoopParams> params, index<Scope> procedure):Statement{OpID::FOR,NodeType::FOR_LOOP_PARAMS,NodeType::SCOPE,params,procedure} {}

      index<ForLoopParams>& params() { return reinterpret_cast<index<ForLoopParams>&>(_lhs); }
      index<const ForLoopParams> params() const { return _lhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_rhs); }
      index<const Scope> procedure() const { return _rhs; }
};

#endif //FOR_LOOP_HPP