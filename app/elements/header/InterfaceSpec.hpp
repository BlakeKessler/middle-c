#pragma once
#ifndef INTERFACE_SPEC_HPP
#define INTERFACE_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::InterfaceSpec {
   private:
      mcsl::dyn_arr<index<Interface>> _inheritedInterfaces;

      mcsl::dyn_arr<index<Function>> _staticFuncs;
      mcsl::dyn_arr<index<Function>> _methods;
   public:
      static const bool IsPseudoNode = true;
      static constexpr NodeType nodeType() { return NodeType::INTERFACE_SPEC; }

      InterfaceSpec():_inheritedInterfaces{},_staticFuncs{},_methods{} {}


      mcsl::dyn_arr<index<Interface>>& inheritedInterfaces() { return _inheritedInterfaces; }
      mcsl::dyn_arr<index<Function>>& staticFuncs() { return _staticFuncs; }
      mcsl::dyn_arr<index<Function>>& methods() { return _methods; }
      const mcsl::dyn_arr<index<Interface>>& inheritedInterfaces() const { return _inheritedInterfaces; }
      const mcsl::dyn_arr<index<Function>>& staticFuncs() const { return _staticFuncs; }
      const mcsl::dyn_arr<index<Function>>& methods() const { return _methods; }
};

#endif //INTERFACE_SPEC_HPP