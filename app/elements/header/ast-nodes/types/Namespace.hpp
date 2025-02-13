#pragma once
#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Namespace : public clef::Type {
   private:
      index<NamespaceSpec> _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::NAMESPACE; }

      Namespace():Type{},_spec{} {}
      Namespace(index<NamespaceSpec> spec):Type{},_spec{spec} {}
      Namespace(index<NamespaceSpec> spec, const Type& name):Type{name},_spec{spec} {}
      Namespace(const Type& name):Type{name},_spec{} {}

      index<NamespaceSpec>& spec() { return _spec; }
      index<const NamespaceSpec> spec() const { return _spec; }

      void printf() const;
};

#endif //NAMESPACE_HPP