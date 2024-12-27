#pragma once
#ifndef PARAM_LIST_HPP
#define PARAM_LIST_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"

struct clef::ParameterList {
   private:
      mcsl::dyn_arr<Variable*>& _params;
   public:
      ParameterList(mcsl::is_t<Variable*> auto... params):_params{astNode::derefBuf<Variable*>(astNode::allocBuf<Variable*>(exprs...))} {}
      ParameterList(ParameterList& params):_params{params._params} {}
};

#endif //PARAM_LIST_HPP