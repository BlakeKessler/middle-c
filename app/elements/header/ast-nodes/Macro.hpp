#pragma once
#ifndef MACRO_HPP
#define MACRO_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Macro : public clef::Identifier {
   private:
      index<FuncSig> _signature;
      index<Scope> _procedure;
   public:
      static constexpr NodeType nodeType() { return NodeType::MACRO; }

      Macro():Identifier{},_signature{},_procedure{} {}
      Macro(index<FuncSig> sig, index<Scope> procedure = {}):Identifier{},_signature{sig},_procedure{procedure} {}
      Macro(index<FuncSig> sig, Identifier name):Identifier{name},_signature{sig},_procedure{} {}
      Macro(index<FuncSig> sig, index<Scope> procedure, Identifier name):Identifier{name},_signature{sig},_procedure{procedure} {}

      index<FuncSig>& signature() { return _signature; }
      index<const FuncSig> signature() const { return _signature; }
      index<Scope>& procedure() { return _procedure; }
      index<const Scope> procedure() const { return _procedure; }
};

namespace mcsl {
   File& write(File&, const clef::Macro&);
};

#endif //MACRO_HPP