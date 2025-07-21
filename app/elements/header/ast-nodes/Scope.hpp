#pragma once
#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/node-lists/StmtSeq.hpp"

#include "dyn_arr.hpp"

struct clef::Scope : public clef::StmtSeq {
   private:
      //!TODO: SymbolNode* _symbolNode;
   public:
      static constexpr NodeType nodeType() { return NodeType::SCOPE; }

      using StmtSeq::StmtSeq;
};

namespace mcsl {
   File& write(File&, const clef::Scope&);
};

#endif //SCOPE_HPP