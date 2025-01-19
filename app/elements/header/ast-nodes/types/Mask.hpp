#pragma once
#ifndef MASK_HPP
#define MASK_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::Mask : public clef::Type {
   private:
      Type* _baseType;
      ParameterList* _enumerators;
   public:
      static constexpr NodeType nodeType() { return NodeType::MASK; }

      Mask():Type{},_baseType{},_enumerators{} {}
      Mask(Type* name, Type* base = {}, ParameterList* enums = {}):Type{name?*name:Type{}},_baseType{base},_enumerators{enums} {}

      Type*& baseType() { return _baseType; }
      const Type* baseType() const { return _baseType; }

      ParameterList*& enumerators() { return _enumerators; }
      const ParameterList* enumerators() const { return _enumerators; }
};

#endif //MASK_HPP