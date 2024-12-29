#pragma once
#ifndef STRUCT_HPP
#define STRUCT_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Struct : public clef::Type {
   private:
      ObjTypeSpec* _spec;
   public:
      Struct():Type{},_spec{} {}
      Struct(ObjTypeSpec* spec):Type{},_spec{spec} {}
      Struct(ObjTypeSpec* spec, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{spec} {}
      Struct(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{} {}

      ObjTypeSpec*& spec() { return _spec; }
      const ObjTypeSpec* spec() const { return _spec; }
};

#endif //STRUCT_HPP