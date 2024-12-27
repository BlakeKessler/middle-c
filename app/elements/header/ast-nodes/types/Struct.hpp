#pragma once
#ifndef STRUCT_HPP
#define STRUCT_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Struct : public clef::Type {
   private:
      index<Variable*> _dataMembers;
      index<Function*> _methods;

      index<Variable*> _staticMembers;
   public:
      Struct():Type{},_dataMembers{astNode::allocBuf<Variable*>()},_methods{astNode::allocBuf<Function*>()},_staticMembers{astNode::allocBuf<Variable*>()} {}
      Struct(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_dataMembers{astNode::allocBuf<Variable*>()},_methods{astNode::allocBuf<Function*>()},_staticMembers{astNode::allocBuf<Variable*>()} {}

      
      auto& dataMembers() { return astNode::derefBuf(_dataMembers); }
      auto& methods() { return astNode::derefBuf(_methods); }
      auto& staticMembers() { return astNode::derefBuf(_staticMembers); }

      const auto& dataMembers() const { return astNode::derefBuf(_dataMembers); }
      const auto& methods() const { return astNode::derefBuf(_methods); }
      const auto& staticMembers() const { return astNode::derefBuf(_staticMembers); }
};

#endif //STRUCT_HPP