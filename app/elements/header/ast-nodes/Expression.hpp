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
      uint _lhs;
      uint _rhs;
      uint _extra;

      friend struct Statement;
      friend struct Declaration;
      friend struct ForLoop;
      friend struct ForeachLoop;
      friend struct WhileLoop;
      friend struct DoWhileLoop;
      friend struct If;
      friend struct Else;
      friend struct Switch;
      friend struct Match;
      friend struct TryCatch;
      friend struct Asm;

      friend class SyntaxTree;

      Expression(OpID op, NodeType lhsType, NodeType rhsType, NodeType extraType, uint lhs, uint rhs, uint extra):_op{op},_lhsType{lhsType},_rhsType{rhsType},_extraType{extraType},_lhs{lhs},_rhs{rhs},_extra{extra} {}
      Expression(OpID op, NodeType lhsType, NodeType rhsType, uint lhs, uint rhs):Expression{op, lhsType, rhsType, NodeType::NONE, lhs, rhs, {}} {}
      Expression(OpID op, NodeType lhsType, uint lhs):Expression{op, lhsType, NodeType::NONE, lhs, {}} {}
      template<astNode_t lhs_t, astNode_t rhs_t, astNode_t extra_t> Expression(OpID op, index<lhs_t> lhs, index<rhs_t> rhs, index<extra_t> extra):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraType{extra_t::nodeType()},
         _lhs{+lhs},_rhs{+rhs},_extra{+extra} {}
   public:
      static constexpr NodeType nodeType() { return NodeType::EXPR; }

      Expression():_op{},_lhsType{},_rhsType{},_lhs{},_rhs{} {}

      template<operand_t lhs_t, operand_t rhs_t> Expression(OpID binaryOp, index<lhs_t> lhs, index<rhs_t> rhs):Expression{binaryOp, lhs_t::nodeType(), rhs_t::nodeType(), lhs, rhs} {}
      template<operand_t lhs_t>Expression(OpID unaryOp, index<lhs_t> lhs):Expression{unaryOp, lhs_t::nodeType(), lhs} {}
      Expression(OpID unaryOp, NodeType t, index<astNode> lhs):Expression{unaryOp, t, +lhs} {}
      Expression(OpID nullaryOp):Expression{nullaryOp, NodeType::NONE, {}} {}

      Expression(OpID op, index<Type> type, index<ArgList> ctorArgs);
      
      Expression(KeywordID oplikeKeyword):Expression{toOpID(oplikeKeyword)} {}
      template<operand_t lhs_t>Expression(KeywordID oplikeKeyword, index<lhs_t> operand):Expression{toOpID(oplikeKeyword), operand} {}
      template<operand_t lhs_t, operand_t rhs_t>Expression(KeywordID oplikeKeyword, index<lhs_t> lhs, index<rhs_t> rhs):Expression{toOpID(oplikeKeyword), lhs, rhs} {}
      Expression(KeywordID funclikeKeyword, index<ArgList> args):Expression{toOpID(funclikeKeyword), NodeType::ARG_LIST, args} {}

      Literal* value() const; //evaluate expression
      
      OpID opID() const { return _op; }
      NodeType lhsType() const { return _lhsType; }
      NodeType rhsType() const { return _rhsType; }
      NodeType extraType() const { return _extraType; }

      index<void> lhs() { return _lhs; }
      index<const void> lhs() const { return _lhs; }
      index<void> rhs() { return _rhs; }
      index<const void> rhs() const { return _rhs; }
      index<void> extra() { return _extra; }
      index<const void> extra() const { return _extra; }

      void setLHS(uint lhs, NodeType t) { _lhs = lhs; _lhsType = t; }
      void setRHS(uint rhs, NodeType t) { _rhs = rhs; _rhsType = t; }
      void setExtra(uint extra, NodeType t) { _extra = extra; _extraType = t; }

      template<astNode_t T> void setLHS(index<T> lhs) { setLHS(lhs, T::nodeType()); }
      template<astNode_t T> void setRHS(index<T> rhs) { setRHS(rhs, T::nodeType()); }
      template<astNode_t T> void setExtra(index<T> extra) { setExtra(extra, T::nodeType()); }
};

#endif //EXPR_HPP