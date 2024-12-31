#pragma once
#ifndef FUNC_SIG_HPP
#define FUNC_SIG_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::FunctionSignature : public clef::Type {
   private:
      Type* _returnType;
      ParamList* _paramTypes;
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_SIG; }

      FunctionSignature():Type{},_returnType{},_paramTypes{} {}
      FunctionSignature(Type* ret, ParamList* params = nullptr):Type{},_returnType{ret},_paramTypes{params} {}

      Type*& returnType() { return _returnType; }
      const Type* returnType() const { return _returnType; }
      ParamList*& params() { return _paramTypes; }
      const ParamList* params() const { return _paramTypes; }
};

#endif //FUNC_SIG_HPP