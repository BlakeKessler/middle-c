#pragma once
#ifndef CLASS_HPP
#define CLASS_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Class : public clef::Type {
   private:
      ObjTypeSpec* _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::CLASS; }

      Class():Type{},_spec{} {}
      Class(ObjTypeSpec* spec):Type{},_spec{spec} {}
      Class(ObjTypeSpec* spec, Type* name):Type{name?*name:Type{}},_spec{spec} {}
      Class(Type* name):Type{name?*name:Type{}},_spec{} {}

      ObjTypeSpec*& spec() { return _spec; }
      const ObjTypeSpec* spec() const { return _spec; }
};

#endif //CLASS_HPP