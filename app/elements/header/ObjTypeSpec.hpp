#pragma once
#ifndef OBJ_SPEC_HPP
#define OBJ_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::ObjTypeSpec {
   private:
      mcsl::dyn_arr<index<Type>> _inheritedTypes; //includes implemented interfaces

      mcsl::dyn_arr<index<Variable>> _staticVars;
      mcsl::dyn_arr<index<Function>> _staticFuncs;
      mcsl::dyn_arr<index<Type>> _memberTypes;

      mcsl::dyn_arr<index<Variable>> _members; //data members
      mcsl::dyn_arr<index<Function>> _methods;

      mcsl::dyn_arr<index<Type>> _friendTypes;

   public:
      static const bool IsPseudoNode = true;
      static constexpr NodeType nodeType() { return NodeType::OBJ_TYPE_SPEC; }

      ObjTypeSpec():_inheritedTypes{},_staticVars{},_staticFuncs{},_memberTypes{},_members{},_methods{},_friendTypes{} {}


      mcsl::dyn_arr<index<Type>>& inheritedTypes() { return _inheritedTypes; }
      mcsl::dyn_arr<index<Variable>>& staticVars() { return _staticVars; }
      mcsl::dyn_arr<index<Function>>& staticFuncs() { return _staticFuncs; }
      mcsl::dyn_arr<index<Type>>& memberTypes() { return _memberTypes; }
      mcsl::dyn_arr<index<Variable>>& members() { return _members; }
      mcsl::dyn_arr<index<Function>>& methods() { return _methods; }
      mcsl::dyn_arr<index<Type>>& friendTypes() { return _friendTypes; }

      const mcsl::dyn_arr<index<Type>>& inheritedTypes() const { return _inheritedTypes; }
      const mcsl::dyn_arr<index<Variable>>& staticVars() const { return _staticVars; }
      const mcsl::dyn_arr<index<Function>>& staticFuncs() const { return _staticFuncs; }
      const mcsl::dyn_arr<index<Type>>& memberTypes() const { return _memberTypes; }
      const mcsl::dyn_arr<index<Variable>>& members() const { return _members; }
      const mcsl::dyn_arr<index<Function>>& methods() const { return _methods; }
      const mcsl::dyn_arr<index<Type>>& friendTypes() const { return _friendTypes; }
};

#endif //OBJ_SPEC_HPP