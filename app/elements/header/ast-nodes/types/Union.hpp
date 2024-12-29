#pragma once
#ifndef UNION_HPP
#define UNION_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Union : public clef::Type {
   private:
      ParameterList* _members;
   public:
      Union():Type{},_members{} {}
      Union(ParameterList* members):Type{},_members{members} {}
      Union(const char* str, const uint len, Decl* decl = nullptr):Type{str,len,decl},_members{} {}
      Union(ParameterList* members, const char* str, const uint len, Decl* decl = nullptr):Type{str,len,decl},_members{members} {}

      ParameterList*& members() { return _members; }
      const ParameterList* members() const { return _members; }
};

#endif //UNION_HPP