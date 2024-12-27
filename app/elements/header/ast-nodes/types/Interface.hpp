#pragma once
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Interface : public clef::Type {
   private:
      mcsl::dyn_arr<Function*>& _methods;
   public:
      Interface():Type{},_methods{astNode::derefBuf(astNode::allocBuf<Function*>())} {}
      Interface(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_methods{astNode::derefBuf(astNode::allocBuf<Function*>())} {}
      Interface(const char* str, const uint len, mcsl::is_t<Function*> auto... methods):Type{str, len},_methods{astNode::derefBuf(astNode::allocBuf<Function*>(methods...))} {}
      Interface(const char* str, const uint len, Decl* decl, mcsl::is_t<Function*> auto... methods):Type{str, len, decl},_methods{astNode::derefBuf(astNode::allocBuf<Function*>(methods...))} {}

      auto& methods() { return _methods; }
      const auto& methods() const { return _methods; }
};

#endif //INTERFACE_HPP