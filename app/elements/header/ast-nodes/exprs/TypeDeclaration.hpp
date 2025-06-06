#pragma once
#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/types/FundamentalType.hpp"
#include "ast-nodes/node-lists/ParameterList.hpp"
#include "ObjTypeSpec.hpp"
#include "InterfaceSpec.hpp"
#include "NamespaceSpec.hpp"

struct clef::TypeDeclaration : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::MAKE_TYPE; }
      static constexpr OpID pseudoOpID() { return OpID::MAKE_TYPE; }

      TypeDeclaration():Statement{} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name):Statement{OpID::MAKE_TYPE,objType,name} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<ObjTypeSpec> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<InterfaceSpec> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<NamespaceSpec> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<Function> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<Macro> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}
      TypeDeclaration(index<FundType> objType, index<Identifier> name, index<ParameterList> spec):Statement{OpID::MAKE_TYPE,objType,name,spec} {}


      index<FundType>& objType() { return reinterpret_cast<index<FundType>&>(_lhs); }
      index<const FundType> objType() const { return _lhs; }
      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_rhs); }
      index<const Identifier> name() const { return _rhs; }
      
      NodeType specType() const { return _extraType; }
      index<void> spec() const { return _extra; }
      index<ObjTypeSpec>& objSpec() { return reinterpret_cast<index<ObjTypeSpec>&>(_extra); }
      index<const ObjTypeSpec> objSpec() const { return _extra; }
      index<InterfaceSpec>& ifaceSpec() { return reinterpret_cast<index<InterfaceSpec>&>(_extra); }
      index<const InterfaceSpec> ifaceSpec() const { return _extra; }
      index<NamespaceSpec>& nsSpec() { return reinterpret_cast<index<NamespaceSpec>&>(_extra); }
      index<const NamespaceSpec> nsSpec() const { return _extra; }
      index<Function>& funcSpec() { return reinterpret_cast<index<Function>&>(_extra); }
      index<const Function> funcSpec() const { return _extra; }
      index<Macro>& macroSpec() { return reinterpret_cast<index<Macro>&>(_extra); }
      index<const Macro> macroSpec() const { return _extra; }
      index<ParameterList>& otherSpec() { return reinterpret_cast<index<ParameterList>&>(_extra); }
      index<const ParameterList> otherSpec() const { return _extra; }
};

namespace mcsl {
   File& write(File&, const clef::Decl&);
};

#endif //TYPE_DECL_HPP