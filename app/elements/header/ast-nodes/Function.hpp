#pragma once
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Function : public clef::Identifier {
   private:
      FuncSig* _signature;
      Scope* _procedure;
   public:
      Function():Identifier{},_signature{},_procedure{} {}
      Function(FuncSig* sig, Scope* procedure = nullptr):Identifier{},_signature{sig},_procedure{procedure} {}
      Function(FuncSig* sig, Identifier* name):Identifier{name?*name:Identifier{}},_signature{sig},_procedure{} {}
      Function(FuncSig* sig, Scope* procedure, Identifier* name):Identifier{name?*name:Identifier{}},_signature{sig},_procedure{procedure} {}

      FuncSig*& signature() { return _signature; }
      const FuncSig* signature() const { return _signature; }
      Scope*& procedure() { return _procedure; }
      const Scope* procedure() const { return _procedure; }
};

#endif //FUNCTION_HPP