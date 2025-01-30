#pragma once
#ifndef MASK_HPP
#define MASK_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::Mask : public clef::Type {
   private:
      index<Type> _baseType;
      index<ParameterList> _enumerators;
   public:
      static constexpr NodeType nodeType() { return NodeType::MASK; }

      Mask():Type{},_baseType{},_enumerators{} {}
      Mask(Type& name, index<Type> base = {}, index<ParameterList> enums = {}):Type{name},_baseType{base},_enumerators{enums} {}

      index<Type>& baseType() { return _baseType; }
      index<const Type> baseType() const { return _baseType; }

      index<ParameterList>& enumerators() { return _enumerators; }
      index<const ParameterList> enumerators() const { return _enumerators; }
};

#endif //MASK_HPP