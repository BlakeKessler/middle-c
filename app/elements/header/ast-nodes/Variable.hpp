#pragma once
#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Variable : public clef::Identifier {
   private:
      Type* _type;
      TypeQualMask _quals;

   public:
      Variable():Identifier{},_type{},_quals{} {}
      Variable(Type* type, TypeQualMask quals = TypeQualMask::NONE):Identifier{},_type{type},_quals{quals} {}
      Variable(Type* type, const char* str, const uint len, Decl* decl = nullptr):Identifier{str, len, decl},_type{type},_quals{} {}
      Variable(Type* type, TypeQualMask quals, const char* str, const uint len, Decl* decl = nullptr):Identifier{str, len, decl},_type{type},_quals{quals} {}

      Type* type() { return _type; }
      const Type* type() const { return _type; }
      TypeQualMask& qualifiers() { return _quals; }
      TypeQualMask qualifiers() const { return _quals; }
};

#endif //VARIABLE_HPP