#pragma once
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Function : public clef::Identifier {
   private:
      index<FuncSig> _signature;
      index<Scope> _procedure;
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC; }

      Function():Identifier{},_signature{},_procedure{} {}
      Function(index<FuncSig> sig, index<Scope> procedure = {}):Identifier{},_signature{sig},_procedure{procedure} {}
      Function(index<FuncSig> sig, Identifier name):Identifier{name},_signature{sig},_procedure{} {}
      Function(index<FuncSig> sig, index<Scope> procedure, Identifier name):Identifier{name},_signature{sig},_procedure{procedure} {}

      index<FuncSig>& signature() { return _signature; }
      index<const FuncSig> signature() const { return _signature; }
      index<Scope>& procedure() { return _procedure; }
      index<const Scope> procedure() const { return _procedure; }
};

#endif //FUNCTION_HPP