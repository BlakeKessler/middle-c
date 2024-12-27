#pragma once
#ifndef MATCH_CASES_HPP
#define MATCH_CASES_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::MatchCases {
   private:
      mcsl::dyn_arr<mcsl::pair<Expr*,Scope*>>& _cases; //case expression, scope to jump to
   public:
      MatchCases(mcsl::is_t<mcsl::pair<Expr*,Scope*>> auto... cases):_cases{astNode::derefBuf<mcsl::pair<Expr*,Scope*>>(astNode::allocBuf<mcsl::pair<Expr*,Scope*>>(cases...))} {}
      MatchCases(MatchCases& cases):_cases{cases._cases} {}
};

#endif //MATCH_CASES_HPP