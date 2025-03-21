#pragma once
#ifndef VAR_PARAM_HPP
#define VAR_PARAM_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::VariadicParameter : public clef::Type {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::VAR_PARAM; }
      using Type::Type;
};

namespace mcsl {
   File& write(File&, const clef::VariadicParam&);
};

#endif //VAR_PARAM_HPP