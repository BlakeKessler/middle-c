#pragma once
#ifndef FUNC_SIG_HPP
#define FUNC_SIG_HPP

#include "CLEF.hpp"
#include "ast-nodes/types/FundamentalType.hpp"

struct clef::FunctionSignature : public clef::FundamentalType {
   private:
      index<Type> _returnType;
      index<ParamList> _paramTypes;
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_SIG; }

      FunctionSignature():FundamentalType{FundTypeID::FUNCTION_SIGNATURE},_returnType{},_paramTypes{} {}
      FunctionSignature(index<Type> ret, index<ParamList> params = {}):FundamentalType{FundTypeID::FUNCTION_SIGNATURE},_returnType{ret},_paramTypes{params} {}

      index<Type>& returnType() { return _returnType; }
      index<const Type> returnType() const { return _returnType; }
      index<ParamList>& params() { return _paramTypes; }
      index<const ParamList> params() const { return _paramTypes; }
};

namespace mcsl {
   File& write(File&, const clef::FuncSig&);
};

#endif //FUNC_SIG_HPP