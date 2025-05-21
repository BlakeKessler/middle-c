#pragma once
#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/node-lists/StatementSequence.hpp"

#include "dyn_arr.hpp"

struct clef::Scope : public clef::StatementSequence {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::SCOPE; }

      using StatementSequence::StatementSequence;
};

namespace mcsl {
   File& write(File&, const clef::Scope&);
};

#endif //SCOPE_HPP