#pragma once
#ifndef TYPE_HPP
#define TYPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Type : public clef::Identifier {
   private:
      
   public:
      static constexpr NodeType nodeType() { return NodeType::TYPE; }

      using Identifier::Identifier;
};

#endif //TYPE_HPP