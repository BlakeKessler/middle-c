#pragma once
#ifndef ARG_LIST_HPP
#define ARG_LIST_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"

struct clef::ArgumentList {
   private:
      mcsl::dyn_arr<Expr*>& _exprs;
   public:
      ArgumentList(mcsl::is_t<Expr*> auto... exprs):_exprs{astNode::derefBuf<Expr*>(astNode::allocBuf<Expr*>(exprs...))} {}
      ArgumentList(ArgumentList& exprs):_exprs{exprs._exprs} {}
};

#endif //ARG_LIST_HPP