#pragma once
#ifndef EXPRESISON_HPP
#define EXPRESSION_HPP

#include "CLEF.hpp"

#include "astNode.hpp"
#include "ast-nodes/Type.hpp"

#include "static_arr.hpp"

struct clef::Expression {
   private:
      mcsl::static_arr<astNode*, 3> _nodes;
      Type* _returnType;
   public:
      Expression(Type* type, astNode* first = nullptr, astNode* second = nullptr, astNode* third = nullptr)
         :_nodes{mcsl::static_arr<astNode*,3>{first, second, third}},_returnType{type} {}
      Expression(Type* type, mcsl::static_arr<astNode*,3> nodes):_nodes{nodes},_returnType{type} {}

      astNode*& operator[](const uint i) { return _nodes[i]; }
      const astNode* operator[](const uint i) const { return _nodes[i]; }

      Type*& type() { return _returnType; }
      const Type* type() const { return _returnType; }
};

#endif //EXPRESSION_HPP