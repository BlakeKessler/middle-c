#pragma once
#ifndef FOR_LOOP_PARAMS_HPP
#define FOR_LOOP_PARAMS_HPP

#include "CLEF.hpp"

struct clef::ForLoopParams {
   private:
      Expr* _declExpr;
      Expr* _cond;
      Expr* _inc;
   public:
      static constexpr NodeType nodeType() { return NodeType::FOR_LOOP_PARAMS; }

      ForLoopParams():_declExpr{},_cond{},_inc{} {}
      ForLoopParams(Expr* decl, Expr* cond, Expr* inc):_declExpr{decl},_cond{cond},_inc{inc} {}

      Expr*& decl() { return _declExpr; }
      Expr*& condition() { return _cond; }
      Expr*& increment() { return _inc; }

      const Expr* decl() const { return _declExpr; }
      const Expr* condition() const { return _cond; }
      const Expr* increment() const { return _inc; }
};

#endif //FOR_LOOP_PARAMS_HPP