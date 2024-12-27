#pragma once
#ifndef SWITCH_CASES_HPP
#define SWITCH_CASES_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::SwitchCases {
   private:
      Scope* _proc;
      mcsl::dyn_arr<mcsl::pair<Expr*,Stmt*>>& _cases; //case expression, statement to jump to
   public:
      SwitchCases():_proc{},_cases{astNode::derefBuf<mcsl::pair<Expr*,Stmt*>>(astNode::allocBuf<mcsl::pair<Expr*,Stmt*>>())} {}
      SwitchCases(Scope* proc, mcsl::is_t<mcsl::pair<Expr*,Stmt*>> auto... cases):_proc{proc},_cases{astNode::derefBuf<mcsl::pair<Expr*,Stmt*>>(astNode::allocBuf<mcsl::pair<Expr*,Stmt*>>(cases...))} {}
      SwitchCases(SwitchCases& cases):_cases{cases._cases} {}
};

#endif //SWITCH_CASES_HPP