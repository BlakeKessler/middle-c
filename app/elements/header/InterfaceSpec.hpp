#pragma once
#ifndef INTERFACE_SPEC_HPP
#define INTERFACE_SPEC_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::InterfaceSpec {
   private:
      mcsl::dyn_arr<Interface*> _inheritedInterfaces;

      mcsl::dyn_arr<Function*> _staticFuncs;
      mcsl::dyn_arr<Function*> _methods;
   public:
      InterfaceSpec():_inheritedInterfaces{},_staticFuncs{},_methods{} {}


      mcsl::dyn_arr<Interface*>& inheritedInterfaces() { return _inheritedInterfaces; }
      mcsl::dyn_arr<Function*>& staticFuncs() { return _staticFuncs; }
      mcsl::dyn_arr<Function*>& methods() { return _methods; }
      const mcsl::dyn_arr<Interface*>& inheritedInterfaces() const { return _inheritedInterfaces; }
      const mcsl::dyn_arr<Function*>& staticFuncs() const { return _staticFuncs; }
      const mcsl::dyn_arr<Function*>& methods() const { return _methods; }
};

#endif //INTERFACE_SPEC_HPP