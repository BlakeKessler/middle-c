#pragma once
#ifndef ASM_HPP
#define ASM_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::Asm : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::ASM; }
      static constexpr OpID pseudoOpID() { return OpID::ASM; }
      //qualifiers - encode in operator?
      //instructions - encode as string literal?
      //vars
         //in expressions
         //out vars
         //inout vars
      //clobbers
      //labels
};

namespace mcsl {
   File& write(File&, const clef::Asm&);
};

#endif //ASM_HPP