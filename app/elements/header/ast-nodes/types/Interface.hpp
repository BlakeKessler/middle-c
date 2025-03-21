#pragma once
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Interface : public clef::Type {
   private:
      index<InterfaceSpec> _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::INTERFACE; }

      Interface():Type{},_spec{} {}
      Interface(index<InterfaceSpec> spec):Type{},_spec{spec} {}
      Interface(const Type& name):Type{name},_spec{} {}
      Interface(index<InterfaceSpec> spec, const Type& name):Type{name},_spec{spec} {}

      index<InterfaceSpec>& spec() { return _spec; }
      index<const InterfaceSpec> spec() const { return _spec; }
};

namespace mcsl {
   File& write(File&, const clef::Interface&);
};

#endif //INTERFACE_HPP