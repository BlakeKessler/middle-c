#ifndef FUNCTION_CPP
#define FUNCTION_CPP

#include "ast-nodes/Function.hpp"

clef::Function::Function(Identifier* name, Type* returnType, Block* params)
   :_name{name},_returnType{returnType},_params{params} {
      //check parameter block syntax
      if (_params && _params->type() != BlockType::PARENS) {
         throwError(ErrCode::BAD_PARAM_BLOCK, "attempt to construct Function AST node with an invalid parameter block (%u)", +_params->type());
      }
}

bool clef::Function::sameSignature(const Function& other) const {
   if ( _params ==  other._params) { return  true; } //compare parameter block addresses
   if (!_params || !other._params) { return false; } //check that parameter block addresses are non-null

   //compare parameter block lengths
   if (_params->size() != other._params->size()) { return false; }
   
   //compare types in parameter blocks
   const auto& params = *_params;
   const auto& otherParams = *other._params;
   for (uint i = 0; i < params.size(); ++i) {
      //same address
      if (params[i] == otherParams[i]) { continue; }
      //types have same address
      if (params[i]->type() == otherParams[i]->type()) { continue; }
      //ensure that addresses of type are non-null
      if (!params[i]->type() || !otherParams[i]->type()) { return false; }
      //identical typename strings
      if (*params[i]->type() == *otherParams[i]->type()) { continue; }
      //all possible ways for the parameter type to match failed
      return false;
   }
   return true;
}

#endif //FUNCTION_CPP