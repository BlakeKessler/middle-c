#ifndef PROC_HPP
#define PROC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"
#include "map.hpp"

class clef::Proc {
   public:
      using SyntaxNode_t = void;
   private:
      mcsl::dyn_arr<Expr*> _exprs;
      //!TODO: symbols
      //!TODO: labels
   public:
      Proc():_exprs{} {}
   
      mcsl::arr_span<Expr*> exprs() { return _exprs.span(); }
      void addExpr(Expr* expr) { _exprs.push_back(expr); }
      void shrinkToFit() { _exprs.reserve_exact(_exprs.size()); }
};

#endif