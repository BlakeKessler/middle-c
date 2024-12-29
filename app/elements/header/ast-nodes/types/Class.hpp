#pragma once
#ifndef CLASS_HPP
#define CLASS_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Class : public clef::Type {
   private:
      ObjTypeSpec* _spec;
   public:
      Class():Type{},_spec{} {}
      Class(ObjTypeSpec* spec):Type{},_spec{spec} {}
      Class(ObjTypeSpec* spec, const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{spec} {}
      Class(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_spec{} {}

      ObjTypeSpec*& spec() { return _spec; }
      const ObjTypeSpec* spec() const { return _spec; }
};

#endif //CLASS_HPP