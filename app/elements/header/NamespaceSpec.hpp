#pragma once
#ifndef NAMESPACE_SPEC_HPP
#define NAMESPACE_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::NamespaceSpec {
   private:
      mcsl::dyn_arr<Variable*> _vars;
      mcsl::dyn_arr<Function*> _funcs;
      mcsl::dyn_arr<Type*> _types;

   public:
      NamespaceSpec():_vars{},_funcs{},_types{} {}

      mcsl::dyn_arr<Variable*>& vars() {return _vars; }
      mcsl::dyn_arr<Function*>& funcs() {return _funcs; }
      mcsl::dyn_arr<Type*>& types() {return _types; }
      const mcsl::dyn_arr<Variable*>& vars() const {return _vars; }
      const mcsl::dyn_arr<Function*>& funcs() const {return _funcs; }
      const mcsl::dyn_arr<Type*>& types() const {return _types; }
};

#endif //NAMESPACE_SPEC_HPP