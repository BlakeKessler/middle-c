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
      Interface(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{} {}
      Interface(InterfaceSpec* spec, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{spec} {}

      InterfaceSpec*& spec() { return _spec; }
      const InterfaceSpec* spec() const { return _spec; }
};

#endif //INTERFACE_HPP