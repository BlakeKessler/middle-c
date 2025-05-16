#pragma once
#ifndef NAMESPACE_SPEC_HPP
#define NAMESPACE_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::NamespaceSpec {
   private:
      mcsl::dyn_arr<index<Variable>> _vars;
      mcsl::dyn_arr<index<Function>> _funcs;
      mcsl::dyn_arr<index<Type>> _types;

   public:
      static const bool IsPseudoNode = true;
      static constexpr NodeType nodeType() { return NodeType::NAMESPACE_SPEC; }

      NamespaceSpec():_vars{},_funcs{},_types{} {}

      mcsl::dyn_arr<index<Variable>>& vars() {return _vars; }
      mcsl::dyn_arr<index<Function>>& funcs() {return _funcs; }
      mcsl::dyn_arr<index<Type>>& types() {return _types; }
      const mcsl::dyn_arr<index<Variable>>& vars() const {return _vars; }
      const mcsl::dyn_arr<index<Function>>& funcs() const {return _funcs; }
      const mcsl::dyn_arr<index<Type>>& types() const {return _types; }
};

#endif //NAMESPACE_SPEC_HPP