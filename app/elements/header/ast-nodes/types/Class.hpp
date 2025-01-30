#pragma once
#ifndef CLASS_HPP
#define CLASS_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Class : public clef::Type {
   private:
      index<ObjTypeSpec> _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::CLASS; }

      Class():Type{},_spec{} {}
      Class(index<ObjTypeSpec> spec):Type{},_spec{spec} {}
      Class(index<ObjTypeSpec> spec, Type& name):Type{name},_spec{spec} {}
      Class(Type& name):Type{name},_spec{} {}

      index<ObjTypeSpec>& spec() { return _spec; }
      index<const ObjTypeSpec> spec() const { return _spec; }
};

#endif //CLASS_HPP