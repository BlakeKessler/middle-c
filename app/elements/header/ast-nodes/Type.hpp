#pragma once
#ifndef TYPE_HPP
#define TYPE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Identifier.hpp"

struct clef::Type {
   private:
      Identifier* _typeName;
      TypeQualMask _qualifiers;
   public:
      Type(Identifier* iden):_typeName{iden},_qualifiers{} {}
      Type(Identifier* iden, const TypeQualMask quals):_typeName{iden},_qualifiers{quals} {}

      Identifier*& name() { return _typeName; }
      const Identifier* name() const { return _typeName; }
      TypeQualMask& quals() { return _qualifiers; }
      TypeQualMask quals() const { return _qualifiers; }

      bool operator==(const Type& other) const {
         if (_qualifiers !=  other._qualifiers) { return false; }
         if ( _typeName  ==  other._typeName)   { return  true; }
         if (!_typeName  || !other._typeName)   { return false; }
         return *_typeName == *other._typeName;
      }
};

#endif //TYPE_HPP