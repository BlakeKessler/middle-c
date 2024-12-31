#pragma once
#ifndef ELSE_HPP
#define ELSE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"

struct clef::Else : public clef::Expression {
   private:

   public:
      Else():Expression{} {}
      Else(If* ifptr, Scope* proc):Expression{OperatorID::ELSE,ifptr,proc} {}

      If*& parentIf() { return (If*&)_lhs; }
      const If* parentIf() const { return (If*)_lhs; }
      Scope*& procedure() { return (Scope*&)_rhs;}
      const Scope* procedure() const { return (Scope*)_rhs;}
};

#endif //ELSE_HPP