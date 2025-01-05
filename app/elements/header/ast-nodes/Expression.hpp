#pragma once
#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"

struct clef::Expression {
   private:
      OpID _op;
      NodeType _lhsType;
      NodeType _rhsType;
      NodeType _extraType;
      void* _lhs;
      void* _rhs;
      void* _extra;

      friend struct Statement;
      friend struct Declaration;
      friend struct Loop;
      friend struct If;
      friend struct Else;
      friend struct Switch;
      friend struct Match;
      friend struct TryCatch;
      // friend struct Asm;

      Expression(OpID op, NodeType lhsType, NodeType rhsType, NodeType extraType, void* lhs, void* rhs, void* extra):_op{op},_lhsType{lhsType},_rhsType{rhsType},_extraType{extraType},_lhs{lhs},_rhs{rhs},_extra{extra} {}
      Expression(OpID op, NodeType lhsType, NodeType rhsType, void* lhs, void* rhs):_op{op},_lhsType{lhsType},_rhsType{rhsType},_extraType{NodeType::NONE},_lhs{lhs},_rhs{rhs},_extra{nullptr} {}
      template<astNode_t lhs_t, astNode_t rhs_t, astNode_t extra_t> Expression(OpID op, lhs_t* lhs, rhs_t* rhs, rhs_t* extra):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraType{extra_t::nodeType()},
         _lhs{lhs},_rhs{rhs},_extra{extra} {}
   public:
      static constexpr NodeType nodeType() { return NodeType::EXPR; }

      Expression():_op{},_lhsType{},_rhsType{},_lhs{},_rhs{} {}
      Expression(OpID op, Expr* lhs, Expr* rhs);
      Expression(OpID op, Identifier* lhs, Expr* rhs);
      Expression(OpID op, Expr* lhs, Identifier* rhs);
      Expression(OpID op, Identifier* lhs, Identifier* rhs);
      
      Expression(OpID op, astNode* lhs, astNode* rhs);
      Expression(OpID op, astNode* lhs, astNode* rhs, astNode* extra);
      Literal* value() const; //evaluate expression
      
      OpID opID() const { return _op; }
      NodeType lhsType() const { return _lhsType; }
      NodeType rhsType() const { return _rhsType; }
      NodeType extraType() const { return _extraType; }

      void* lhs() { return _lhs; }
      const void* lhs() const { return _lhs; }
      void* rhs() { return _rhs; }
      const void* rhs() const { return _rhs; }
      void* extra() { return _extra; }
      const void* extra() const { return _extra; }

      template<astNode_t T> void setLHS(T* lhs) { _lhs = lhs; _lhsType = T::nodeType(); }
      template<astNode_t T> void setRHS(T* rhs) { _rhs = rhs; _rhsType = T::nodeType(); }
      template<astNode_t T> void setExtra(T* extra) { _extra = extra; _extraType = T::nodeType(); }
      void setLHS(astNode* lhs);
      void setRHS(astNode* rhs);
      void setExtra(astNode* extra);
};

#endif //EXPR_HPP