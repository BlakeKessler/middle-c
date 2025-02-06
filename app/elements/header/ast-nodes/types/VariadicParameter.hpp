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

      void printf() const;
};

#endif //VAR_PARAM_HPP