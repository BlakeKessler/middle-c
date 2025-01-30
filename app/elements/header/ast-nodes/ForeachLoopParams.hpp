#pragma once
#ifndef FOREACH_LOOP_PARAMS_HPP
#define FOREACH_LOOP_PARAMS_HPP

#include "CLEF.hpp"

struct clef::ForeachLoopParams {
   private:
      index<Decl> _decl;
      index<Expr> _target;
   public:
      static constexpr NodeType nodeType() { return NodeType::FOREACH_LOOP_PARAMS; }

      ForeachLoopParams():_decl{},_target{} {}
      ForeachLoopParams(index<Decl> decl, index<Expr> target):_decl{decl},_target{target} {}

      index<Decl>& decl() { return _decl; }
      index<Expr>& target() { return _target; }

      index<const Decl> decl() const { return _decl; }
      index<const Expr> target() const { return _target; }
};

#endif //FOREACH_LOOP_PARAMS_HPP