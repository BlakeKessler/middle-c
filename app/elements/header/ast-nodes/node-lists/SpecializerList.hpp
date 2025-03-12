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
      };
      bool _isDecl;
   public:
      static constexpr NodeType nodeType() { return NodeType::SPEC_LIST; }
      
      SpecializerList(ParamList other):_declSpecList{other},_isDecl{true} {}
      SpecializerList(ArgList other):_baseSpecList{other},_isDecl{false} {}

      bool isDecl() const { return _isDecl; }

      operator ParamList() { assume(_isDecl); return _declSpecList; }
      operator const ParamList() const { assume(_isDecl); return _declSpecList; }
      
      operator ArgList() { assume(!_isDecl); return _baseSpecList; }
      operator const ArgList() const { assume(!_isDecl); return _baseSpecList; }

      void printf() const {
         if (_isDecl) {
            _declSpecList.printf();
         } else {
            _baseSpecList.printf();
         }
      }
};

#endif //SPEC_LIST_HPP