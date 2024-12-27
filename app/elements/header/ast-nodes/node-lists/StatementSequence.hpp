#pragma once
#ifndef STMT_SEQ_HPP
#define STMT_SEQ_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"

struct clef::StatementSequence {
   private:
      mcsl::dyn_arr<Stmt*>& _stmts;
   public:
      StatementSequence(mcsl::is_t<Stmt*> auto... stmts):_stmts{astNode::derefBuf<Stmt*>(astNode::allocBuf<Stmt*>(stmts...))} {}
      StatementSequence(StatementSequence& stmts):_stmts{stmts._stmts} {}
};

#endif //STMT_SEQ_HPP