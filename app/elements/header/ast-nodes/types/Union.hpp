#pragma once
#ifndef UNION_HPP
#define UNION_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Union : public clef::Type {
   private:
      mcsl::dyn_arr<Variable*>& _members;
   public:
      Union():Type{},_members{astNode::derefBuf(astNode::allocBuf<Variable*>())} {}
      Union(const char* str, const uint len, Decl* decl = nullptr):Type{str,len,decl},_members{astNode::derefBuf(astNode::allocBuf<Variable*>())} {}
      Union(const char* str, const uint len, mcsl::is_t<Variable*> auto... members):Type{str,len},_members{astNode::derefBuf(astNode::allocBuf<Variable*>(members...))} {}
      Union(const char* str, const uint len, Decl* decl, mcsl::is_t<Variable*> auto... members):Type{str,len,decl},_members{astNode::derefBuf(astNode::allocBuf<Variable*>(members...))} {}

      auto& members() { return _members; }
      const auto& members() const { return _members; }
};

#endif //UNION_HPP