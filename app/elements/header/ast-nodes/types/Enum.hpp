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
      mcsl::dyn_arr<mcsl::pair<Identifier*, Expr*>>& _enumerators;
   public:
      Enum():Type{},_baseType{},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>())} {}
      Enum(mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):_baseType{},_enumerators{enumerators...} {}
      Enum(FundType* base, mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):_baseType{base},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>(enumerators...))} {}
      Enum(const char* str, const uint len, Decl* decl = nullptr):Type{str, len, decl},_baseType{},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>())} {}
      Enum(const char* str, const uint len, Decl* decl, mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):Type{str, len, decl},_baseType{},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>())} {}
      Enum(const char* str, const uint len, mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):Type{str, len, decl},_baseType{},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>())} {}
      Enum(const char* str, const uint len, Decl* decl, FundType* base, mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):Type{str, len, decl},_baseType{type},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>(enumerators...))} {}
      Enum(const char* str, const uint len, FundType* base, mcsl::is_t<mcsl::pair<Identifier*, Expr*>> auto... enumerators):Type{str, len, decl},_baseType{type},_enumerators{astNode::derefBuf(astNode::allocBuf<mcsl::pair<Identifier*, Expr*>>(enumerators...))} {}

      FundType*& baseType() { return _baseType; }
      const FundType* baseType() const { return _baseType; }

      auto& enumerators() { return _enumerators; }
      const auto& enumerators() const { return _enumerators; }
};

#endif //ENUM_HPP