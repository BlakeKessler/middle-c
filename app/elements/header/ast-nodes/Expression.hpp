#pragma once
#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"

struct clef::Expression {
   private:
      OperatorID _op;
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
      Expression(OperatorID op, void* lhs, void* rhs):_op{op},_lhs{lhs},_rhs{rhs} {}
   public:
      Expression(OperatorID op, Expr* lhs, Expr* rhs);
      Expression(OperatorID op, Identifier* lhs, Expr* rhs);
      Expression(OperatorID op, Expr* lhs, Identifier* rhs);
      Expression(OperatorID op, Identifier* lhs, Identifier* rhs);
      Literal* value() const; //evaluate expression
};

#endif //EXPR_HPP