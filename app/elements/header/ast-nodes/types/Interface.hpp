#pragma once
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Interface : public clef::Type {
   private:
      InterfaceSpec* _spec;
   public:
      Interface():Type{},_spec{} {}
      Interface(InterfaceSpec* spec):Type{},_spec{spec} {}
      Interface(Type* name):Type{name?*name:Type{}},_spec{} {}
      Interface(InterfaceSpec* spec, Type* name):Type{name?*name:Type{}},_spec{spec} {}

      InterfaceSpec*& spec() { return _spec; }
      const InterfaceSpec* spec() const { return _spec; }
};

#endif //INTERFACE_HPP