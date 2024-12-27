#pragma once
#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Namespace : public clef::Type {
   private:
      index<Variable*> _vars;
      index<Function*> _funcs;
      index<Type*> _types;
   public:
      Namespace():Type{},_vars{astNode::allocBuf<Variable*>()},_funcs{astNode::allocBuf<Function*>()},_types{astNode::allocBuf<Type*>()} {}
      Namespace(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_vars{astNode::allocBuf<Variable*>()},_funcs{astNode::allocBuf<Function*>()},_types{astNode::allocBuf<Type*>()} {}


      auto& vars() { return astNode::derefBuf(_vars); }
      auto& funcs() { return astNode::derefBuf(_funcs); }
      auto& types() { return astNode::derefBuf(_types); }
      const auto& vars() const { return astNode::derefBuf(_vars); }
      const auto& funcs() const { return astNode::derefBuf(_funcs); }
      const auto& types() const { return astNode::derefBuf(_types); }

};

#endif //NAMESPACE_HPP