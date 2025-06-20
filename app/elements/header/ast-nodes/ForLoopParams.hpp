#pragma once
#ifndef FOR_LOOP_PARAMS_HPP
#define FOR_LOOP_PARAMS_HPP

#include "CLEF.hpp"

struct clef::ForLoopParams {
   private:
      index<Decl> _declExpr;
      index<Expr> _cond;
      index<Expr> _inc;
   public:
      static constexpr NodeType nodeType() { return NodeType::FOR_LOOP_PARAMS; }

      ForLoopParams():_declExpr{},_cond{},_inc{} {}
      ForLoopParams(index<Decl> decl, index<Expr> cond, index<Expr> inc):_declExpr{decl},_cond{cond},_inc{inc} {}

      index<Decl>& decl() { return _declExpr; }
      index<Expr>& condition() { return _cond; }
      index<Expr>& increment() { return _inc; }

      index<const Decl> decl() const { return _declExpr; }
      index<const Expr> condition() const { return _cond; }
      index<const Expr> increment() const { return _inc; }
};

namespace mcsl {
   File& write(File&, const clef::ForLoopParams&);
};

#endif //FOR_LOOP_PARAMS_HPP