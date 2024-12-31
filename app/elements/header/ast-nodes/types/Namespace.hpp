#pragma once
#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Namespace : public clef::Type {
   private:
      NamespaceSpec* _spec;
   public:
      static constexpr NodeType nodeType() { return NodeType::NAMESPACE; }

      Namespace():Type{},_spec{} {}
      Namespace(NamespaceSpec* spec):Type{},_spec{spec} {}
      Namespace(NamespaceSpec* spec, Type* name):Type{name?*name:Type{}},_spec{spec} {}
      Namespace(Type* name):Type{name?*name:Type{}},_spec{} {}

      NamespaceSpec*& spec() { return _spec; }
      const NamespaceSpec* spec() const { return _spec; }

};

#endif //NAMESPACE_HPP