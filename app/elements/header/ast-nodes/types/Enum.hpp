#pragma once
#ifndef ENUM_HPP
#define ENUM_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::Enum : public clef::Type {
   private:
      FundType* _baseType;
      ParameterList* _enumerators;
   public:
      Enum():Type{},_baseType{},_enumerators{} {}
      Enum(ParameterList* enums):_baseType{},_enumerators{enums} {}
      Enum(FundType* base, ParameterList* enums = nullptr):_baseType{base},_enumerators{enums} {}
      Enum(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_baseType{},_enumerators{} {}
      Enum(ParameterList* enums, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_baseType{},_enumerators{enums} {}
      Enum(FundType* base, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_baseType{base},_enumerators{} {}
      Enum(FundType* base, ParameterList* enums, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_baseType{base},_enumerators{enums} {}

      FundType*& baseType() { return _baseType; }
      const FundType* baseType() const { return _baseType; }

      ParameterList*& enumerators() { return _enumerators; }
      const ParameterList* enumerators() const { return _enumerators; }
};

#endif //ENUM_HPP