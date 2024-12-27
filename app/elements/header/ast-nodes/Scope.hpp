#pragma once
#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/node-lists/StatementSequence.hpp"

#include "dyn_arr.hpp"

struct clef::Scope : public clef::StatementSequence {
   private:
   
   public:
      using StatementSequence::StatementSequence;
      // Scope():StatementSequence{} {}
      // Scope(StatementSequence& stmts):StatementSequence{stmts} {}
};

#endif //SCOPE_HPP