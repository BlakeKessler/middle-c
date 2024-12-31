#pragma once
#ifndef STRUCT_HPP
#define STRUCT_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Struct : public clef::Type {
   private:
      ObjTypeSpec* _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::STRUCT; }

      Struct():Type{},_spec{} {}
      Struct(ObjTypeSpec* spec):Type{},_spec{spec} {}
      Struct(ObjTypeSpec* spec, Type* name):Type{name?*name:Type{}},_spec{spec} {}
      Struct(Type* name):Type{name?*name:Type{}},_spec{} {}

      ObjTypeSpec*& spec() { return _spec; }
      const ObjTypeSpec* spec() const { return _spec; }
};

#endif //STRUCT_HPP