#pragma once
#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"
#include "ast-nodes/Operator.hpp"

struct clef::Expression {
   private:
      Operator* _op;
      void* _lhs;
      void* _rhs;
      // union { //!NOTE: if I use this, move it before _op to facilitate type punning
      //    mcsl::pair<void*,void*> _operands;
      //    Literal _lit;
      // };

      friend class Declaration;
      friend class Loop;
      friend class If;
      friend class Else;
      friend class ElseIf;
      friend class Switch;
      friend class Match;
      
      Expression():_op{},_lhs{},_rhs{} {}
      Expression(Operator* op, void* lhs, void* rhs):_op{op},_lhs{lhs},_rhs{rhs} {}
   public:
      Literal* value() const; //evaluate expression
};

#endif //EXPR_HPP