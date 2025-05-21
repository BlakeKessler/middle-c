#pragma once
#ifndef SPEC_LIST_HPP
#define SPEC_LIST_HPP

#include "CLEF.hpp"

#include "ParameterList.hpp"
#include "ArgumentList.hpp"

struct clef::SpecializerList {
   private:
      union {
         ParamList _declSpecList;
         ArgList _baseSpecList;
         Type _typeSingle;
      };
      NodeType _specType;
   public:
      static constexpr NodeType nodeType() { return NodeType::SPEC_LIST; }
      
      SpecializerList(ParamList other):_declSpecList{other},_specType{decltype(other)::nodeType()} {}
      SpecializerList(ArgList other):_baseSpecList{other},_specType{decltype(other)::nodeType()} {}
      SpecializerList(Type other):_typeSingle{other},_specType{decltype(other)::nodeType()} {}
      SpecializerList(SpecList& other) {
         switch(other._specType) {
            case NodeType::PARAM_LIST: new (this) SpecList{other._declSpecList}; break;
            case NodeType::ARG_LIST: new (this) SpecList{other._baseSpecList}; break;
            case NodeType::TYPE: new (this) SpecList{other._typeSingle}; break;
            default: UNREACHABLE;
         }
      }

      NodeType specType() const { return _specType; }
      bool isDecl() const { return _specType == NodeType::PARAM_LIST; }
      bool isSingle() const { return _specType == NodeType::TYPE; }

      operator ParamList() { assume(_specType == NodeType::PARAM_LIST); return _declSpecList; }
      operator const ParamList() const { assume(_specType == NodeType::PARAM_LIST); return _declSpecList; }
      
      operator ArgList() { assume(_specType == NodeType::ARG_LIST); return _baseSpecList; }
      operator const ArgList() const { assume(_specType == NodeType::ARG_LIST); return _baseSpecList; }
      
      operator Type() { assume(_specType == NodeType::TYPE); return _typeSingle; }
      operator const Type() const { assume(_specType == NodeType::TYPE); return _typeSingle; }
};

namespace mcsl {
   inline File& write(File& file, const clef::SpecList& obj) {
      using namespace clef;
      switch (obj.specType()) {
         case NodeType::PARAM_LIST:
            write(file, (const clef::ParamList&)obj);
            break;
         case NodeType::ARG_LIST:
            write(file, (const clef::ArgList&)obj);
            break;
         case NodeType::TYPE:
            write(file, (const clef::Type&)obj);
            break;
         default: UNREACHABLE;
      }
      return file;
   }
}

#endif //SPEC_LIST_HPP