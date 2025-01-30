#pragma once
#ifndef UNION_HPP
#define UNION_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::Union : public clef::Type {
   private:
      index<ParameterList> _members;
   public:
      static constexpr NodeType nodeType() { return NodeType::UNION; }

      Union():Type{},_members{} {}
      Union(index<ParameterList> members):Type{},_members{members} {}
      Union(Type& name):Type{name},_members{} {}
      Union(index<ParameterList> members, Type& name):Type{name},_members{members} {}
      // Union(Identifier name, index<ParameterList> members = {}):Type{name},_members{members} {}

      index<ParameterList>& members() { return _members; }
      index<const ParameterList> members() const { return _members; }
};

#endif //UNION_HPP