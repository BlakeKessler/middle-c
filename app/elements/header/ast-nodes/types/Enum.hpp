#pragma once
#ifndef ENUM_HPP
#define ENUM_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::Enum : public clef::Type {
   private:
      index<Type> _baseType;
      index<ParameterList> _enumerators;
   public:
      static constexpr NodeType nodeType() { return NodeType::ENUM; }

      Enum():Type{},_baseType{},_enumerators{} {}
      Enum(const Type& name, index<Type> base = {}, index<ParameterList> enums = {}):Type{name},_baseType{base},_enumerators{enums} {}

      index<Type>& baseType() { return _baseType; }
      index<const Type> baseType() const { return _baseType; }

      index<ParameterList>& enumerators() { return _enumerators; }
      index<const ParameterList> enumerators() const { return _enumerators; }
};

namespace mcsl {
   File& write(File&, const clef::Enum&);
};

#endif //ENUM_HPP