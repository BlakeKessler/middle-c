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
      Expression(OpID op, NodeType lhsType, NodeType rhsType, void* lhs, void* rhs):Expression{op, lhsType, rhsType, NodeType::NONE, lhs, rhs, nullptr} {}
      Expression(OpID op, NodeType lhsType, void* lhs):Expression{op, lhsType, NodeType::NONE, lhs, nullptr} {}
      template<astNode_t lhs_t, astNode_t rhs_t, astNode_t extra_t> Expression(OpID op, lhs_t* lhs, rhs_t* rhs, extra_t* extra):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraType{extra_t::nodeType()},
         _lhs{lhs},_rhs{rhs},_extra{extra} {}
   public:
      static constexpr NodeType nodeType() { return NodeType::EXPR; }

      Expression():_op{},_lhsType{},_rhsType{},_lhs{},_rhs{} {}

      template<operand_t lhs_t, operand_t rhs_t> Expression(OpID binaryOp, lhs_t* lhs, rhs_t* rhs):Expression{binaryOp, lhs_t::nodeType(), rhs_t::nodeType(), lhs, rhs} {}
      template<operand_t lhs_t>Expression(OpID unaryOp, lhs_t* lhs):Expression{unaryOp, lhs_t::nodeType(), lhs} {}
      Expression(OpID nullaryOp):Expression{nullaryOp, NodeType::NONE, nullptr} {}

      Expression(OpID op, Type* type, ArgList* ctorArgs);
      
      Expression(KeywordID oplikeKeyword):Expression{toOpID(oplikeKeyword)} {}
      template<operand_t lhs_t>Expression(KeywordID oplikeKeyword, lhs_t* operand):Expression{toOpID(oplikeKeyword), operand} {}
      template<operand_t lhs_t, operand_t rhs_t>Expression(KeywordID oplikeKeyword, lhs_t* lhs, rhs_t* rhs):Expression{toOpID(oplikeKeyword), lhs, rhs} {}
      Expression(KeywordID funclikeKeyword, ArgList* args):Expression{toOpID(funclikeKeyword), NodeType::ARG_LIST, args} {}
      
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