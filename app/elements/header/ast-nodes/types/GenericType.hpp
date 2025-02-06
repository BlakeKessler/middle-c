#pragma once
#ifndef GENERIC_TYPE_HPP
#define GENERIC_TYPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::GenericType : public clef::Type {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::GENERIC; }

      using Type::Type;

      void printf() const;
};

#endif //GENERIC_TYPE_HPP