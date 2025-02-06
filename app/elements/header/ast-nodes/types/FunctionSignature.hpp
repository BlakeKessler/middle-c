#pragma once
#ifndef FUNC_SIG_HPP
#define FUNC_SIG_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::FunctionSignature : public clef::Type {
   private:
      index<Type> _returnType;
      index<ParamList> _paramTypes;
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_SIG; }

      FunctionSignature():Type{},_returnType{},_paramTypes{} {}
      FunctionSignature(index<Type> ret, index<ParamList> params = {}):Type{},_returnType{ret},_paramTypes{params} {}

      index<Type>& returnType() { return _returnType; }
      index<const Type> returnType() const { return _returnType; }
      index<ParamList>& params() { return _paramTypes; }
      index<const ParamList> params() const { return _paramTypes; }

      void printf() const;
};

#endif //FUNC_SIG_HPP