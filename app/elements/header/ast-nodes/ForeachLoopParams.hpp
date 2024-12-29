#pragma once
#ifndef FOR_LOOP_PARAMS_HPP
#define FOR_LOOP_PARAMS_HPP

#include "CLEF.hpp"

struct clef::ForeachLoopParams {
   private:
      Decl* _decl;
      Expr* _target;
   public:
      ForeachLoopParams():_decl{},_target{} {}
      ForeachLoopParams(Decl* decl, Expr* target):_decl{decl},_target{target} {}

      Decl*& decl() { return _decl; }
      Expr*& target() { return _target; }

      const Decl* decl() const { return _decl; }
      const Expr* target() const { return _target; }
};

#endif //FOR_LOOP_PARAMS_HPP