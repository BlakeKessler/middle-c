#pragma once
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Function : public clef::Identifier {
   private:
      FuncSig* _signature;
   public:
      Function():Identifier{},_signature{} {}
      Function(FuncSig* sig, const char* str, const uint len, Decl* decl = nullptr):Identifier{str, len, decl},_signature{sig} {}

      FuncSig* signature() { return _signature; }
      const FuncSig* signature() const { return _signature; }
};

#endif //FUNCTION_HPP