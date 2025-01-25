#pragma once
#ifndef ASM_HPP
#define ASM_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Asm : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::ASM; }
      //qualifiers - encode in operator?
      //instructions - encode as string literal?
      //vars
         //in expressions
         //out vars
         //inout vars
      //clobbers
      //labels
};

#endif //ASM_HPP