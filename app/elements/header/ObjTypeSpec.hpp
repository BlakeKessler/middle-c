#pragma once
#ifndef OBJ_SPEC_HPP
#define OBJ_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::ObjTypeSpec {
   private:
      mcsl::dyn_arr<index<Type>> _inheritedTypes; //includes implemented interfaces

      mcsl::dyn_arr<index<Decl>> _staticVars;
      mcsl::dyn_arr<index<Function>> _staticFuncs;
      mcsl::dyn_arr<index<TypeDecl>> _memberTypes;

      mcsl::dyn_arr<index<Decl>> _members; //data members
      mcsl::dyn_arr<index<Function>> _methods;

      mcsl::dyn_arr<index<Type>> _friendTypes;

   public:
      static const bool IsPseudoNode = true;
      static constexpr NodeType nodeType() { return NodeType::OBJ_TYPE_SPEC; }

      ObjTypeSpec():_inheritedTypes{},_staticVars{},_staticFuncs{},_memberTypes{},_members{},_methods{},_friendTypes{} {}


      mcsl::dyn_arr<index<Type>>& inheritedTypes() { return _inheritedTypes; }
      mcsl::dyn_arr<index<Decl>>& staticVars() { return _staticVars; }
      mcsl::dyn_arr<index<Function>>& staticFuncs() { return _staticFuncs; }
      mcsl::dyn_arr<index<TypeDecl>>& memberTypes() { return _memberTypes; }
      mcsl::dyn_arr<index<Decl>>& members() { return _members; }
      mcsl::dyn_arr<index<Function>>& methods() { return _methods; }
      mcsl::dyn_arr<index<Type>>& friendTypes() { return _friendTypes; }

      const mcsl::arr_span<index<Type>> inheritedTypes() const { return _inheritedTypes.span(); }
      const mcsl::arr_span<index<Decl>> staticVars() const { return _staticVars.span(); }
      const mcsl::arr_span<index<Function>> staticFuncs() const { return _staticFuncs.span(); }
      const mcsl::arr_span<index<TypeDecl>> memberTypes() const { return _memberTypes.span(); }
      const mcsl::arr_span<index<Decl>> members() const { return _members.span(); }
      const mcsl::arr_span<index<Function>> methods() const { return _methods.span(); }
      const mcsl::arr_span<index<Type>> friendTypes() const { return _friendTypes.span(); }
};

#endif //OBJ_SPEC_HPP