#pragma once
#ifndef OBJ_SPEC_HPP
#define OBJ_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::ObjTypeSpec {
   private:
      mcsl::dyn_arr<Type*> _inheritedTypes; //includes implemented interfaces

      mcsl::dyn_arr<Variable*> _staticVars;
      mcsl::dyn_arr<Function*> _staticFuncs;
      mcsl::dyn_arr<Type*> _memberTypes;

      mcsl::dyn_arr<Variable*> _members; //data members
      mcsl::dyn_arr<Function*> _methods;

      mcsl::dyn_arr<Type*> _friendTypes;

   public:
      ObjTypeSpec():_inheritedTypes{},_staticVars{},_staticFuncs{},_memberTypes{},_members{},_methods{},_friendTypes{} {}


      mcsl::dyn_arr<Type*>& inheritedTypes() { return _inheritedTypes; }
      mcsl::dyn_arr<Variable*>& staticVars() { return _staticVars; }
      mcsl::dyn_arr<Function*>& staticFuncs() { return _staticFuncs; }
      mcsl::dyn_arr<Type*>& memberTypes() { return _memberTypes; }
      mcsl::dyn_arr<Variable*>& members() { return _members; }
      mcsl::dyn_arr<Function*>& methods() { return _methods; }
      mcsl::dyn_arr<Type*>& friendTypes() { return _friendTypes; }

      const mcsl::dyn_arr<Type*>& inheritedTypes() const { return _inheritedTypes; }
      const mcsl::dyn_arr<Variable*>& staticVars() const { return _staticVars; }
      const mcsl::dyn_arr<Function*>& staticFuncs() const { return _staticFuncs; }
      const mcsl::dyn_arr<Type*>& memberTypes() const { return _memberTypes; }
      const mcsl::dyn_arr<Variable*>& members() const { return _members; }
      const mcsl::dyn_arr<Function*>& methods() const { return _methods; }
      const mcsl::dyn_arr<Type*>& friendTypes() const { return _friendTypes; }
};

#endif //OBJ_SPEC_HPP