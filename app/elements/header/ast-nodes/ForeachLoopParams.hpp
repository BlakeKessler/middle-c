#pragma once
#ifndef FOREACH_LOOP_PARAMS_HPP
#define FOREACH_LOOP_PARAMS_HPP

#include "CLEF.hpp"

struct clef::ForeachLoopParams {
   private:
      index<Variable> _it;
      index<Expr> _target;
   public:
      static constexpr NodeType nodeType() { return NodeType::FOREACH_LOOP_PARAMS; }

      ForeachLoopParams():_it{},_target{} {}
      ForeachLoopParams(index<Variable> iterator, index<Expr> target):_it{iterator},_target{target} {}

      index<Variable>& iterator() { return _it; }
      index<Expr>& target() { return _target; }

      index<const Variable> iterator() const { return _it; }
      index<const Expr> target() const { return _target; }
};

namespace mcsl {
   File& write(File&, const clef::ForeachLoopParams);
};

#endif //FOREACH_LOOP_PARAMS_HPP