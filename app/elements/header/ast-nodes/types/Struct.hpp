#pragma once
#ifndef STRUCT_HPP
#define STRUCT_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Struct : public clef::Type {
   private:
      index<ObjTypeSpec> _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::STRUCT; }

      Struct():Type{},_spec{} {}
      Struct(index<ObjTypeSpec> spec):Type{},_spec{spec} {}
      Struct(index<ObjTypeSpec> spec, const Type& name):Type{name},_spec{spec} {}
      Struct(const Type& name):Type{name},_spec{} {}

      index<ObjTypeSpec>& spec() { return _spec; }
      index<const ObjTypeSpec> spec() const { return _spec; }
};

namespace mcsl {
   File& write(File&, const clef::Struct&);
};

#endif //STRUCT_HPP