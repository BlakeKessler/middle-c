#pragma once
#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"

struct clef::Expr {
   protected:
      TypeSpec* _evalType = nullptr;
      OpID _op;
      NodeType _lhsType;
      NodeType _rhsType;
      NodeType _extraTypes[2];
      uint _lhs;
      uint _rhs;
      uint _extras[2];

      friend class SyntaxTree;

      Expr(OpID op, NodeType lhsType, NodeType rhsType, NodeType extraType, NodeType extraType2, uint lhs, uint rhs, uint extra, uint extra2):_op{op},_lhsType{lhsType},_rhsType{rhsType},_extraTypes{extraType, extraType2},_lhs{lhs},_rhs{rhs},_extras{extra, extra2} {}
      Expr(OpID op, NodeType lhsType, NodeType rhsType, NodeType extraType, uint lhs, uint rhs, uint extra):Expr{op, lhsType, rhsType, extraType, {}, lhs, rhs, extra, {}} {}
      Expr(OpID op, NodeType lhsType, NodeType rhsType, uint lhs, uint rhs):Expr{op, lhsType, rhsType, NodeType::NONE, lhs, rhs, {}} {}
      Expr(OpID op, NodeType lhsType, uint lhs):Expr{op, lhsType, NodeType::NONE, lhs, {}} {}
      template<astNode_t lhs_t> Expr(OpID op, index<lhs_t> lhs):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{},_extraTypes{},
         _lhs{+lhs},_rhs{},_extras{} {}
      template<astNode_t lhs_t, astNode_t rhs_t> Expr(OpID op, index<lhs_t> lhs, index<rhs_t> rhs):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraTypes{},
         _lhs{+lhs},_rhs{+rhs},_extras{} {}
      template<astNode_t lhs_t, astNode_t rhs_t, astNode_t extra_t> Expr(OpID op, index<lhs_t> lhs, index<rhs_t> rhs, index<extra_t> extra):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraTypes{extra_t::nodeType(), {}},
         _lhs{+lhs},_rhs{+rhs},_extras{+extra, 0} {}
      template<astNode_t lhs_t, astNode_t rhs_t, astNode_t extra_t, astNode_t extra2_t> Expr(OpID op, index<lhs_t> lhs, index<rhs_t> rhs, index<extra_t> extra, index<extra2_t> extra2):
         _op{op},_lhsType{lhs_t::nodeType()},_rhsType{rhs_t::nodeType()},_extraTypes{extra_t::nodeType(), extra2_t::nodeType()},
         _lhs{+lhs},_rhs{+rhs},_extras{+extra, +extra2} {}
   
   public:
      static constexpr NodeType nodeType() { return NodeType::EXPR; }

      Expr():_op{},_lhsType{},_rhsType{},_lhs{},_rhs{} {}

      // template<operand_t lhs_t, operand_t rhs_t> Expr(OpID binaryOp, index<lhs_t> lhs, index<rhs_t> rhs):Expr{binaryOp, lhs_t::nodeType(), rhs_t::nodeType(), lhs, rhs} {}
      // template<operand_t lhs_t>Expr(OpID unaryOp, index<lhs_t> lhs):Expr{unaryOp, lhs_t::nodeType(), lhs} {}
      Expr(OpID unaryOp, NodeType t, index<astNode> lhs):Expr{unaryOp, t, +lhs} {}
      Expr(OpID nullaryOp):Expr{nullaryOp, NodeType::NONE, {}} {}

      Expr(KeywordID oplikeKeyword):Expr{toOpID(oplikeKeyword)} {}
      template<operand_t lhs_t>Expr(KeywordID oplikeKeyword, index<lhs_t> operand):Expr{toOpID(oplikeKeyword), operand} {}
      template<operand_t lhs_t, operand_t rhs_t>Expr(KeywordID oplikeKeyword, index<lhs_t> lhs, index<rhs_t> rhs):Expr{toOpID(oplikeKeyword), lhs, rhs} {}
      Expr(KeywordID funclikeKeyword, index<ArgList> args):Expr{toOpID(funclikeKeyword), NodeType::ARG_LIST, +args} {}

      static Expr makeTernary(SyntaxTree& tree, index<astNode> cond, index<astNode> ifExpr, index<astNode> elseExpr);
   public:

      index<Literal> value() const; //evaluate expression
      
      OpID opID() const { return _op; }
      NodeType lhsType() const { return _lhsType; }
      NodeType rhsType() const { return _rhsType; }
      NodeType extraType() const { return _extraTypes[0]; }
      NodeType extraType2() const { return _extraTypes[1]; }

      index<void> lhs() { return _lhs; }
      index<const void> lhs() const { return _lhs; }
      index<void> rhs() { return _rhs; }
      index<const void> rhs() const { return _rhs; }
      index<void> extra() { return _extras[0]; }
      index<const void> extra() const { return _extras[0]; }
      index<void> extra2() { return _extras[1]; }
      index<const void> extra2() const { return _extras[1]; }
      index<void>* extras() { return (index<void>*)_extras; }
      index<const void>* extras() const { return (index<const void>*)_extras; }

      void setLHS(uint lhs, NodeType t) { _lhs = lhs; _lhsType = t; }
      void setRHS(uint rhs, NodeType t) { _rhs = rhs; _rhsType = t; }
      void setExtra(uint extra, NodeType t) { _extras[0] = extra; _extraTypes[0] = t; }
      void setExtra2(uint extra, NodeType t) { _extras[1] = extra; _extraTypes[1] = t; }

      template<astNode_t T> void setLHS(index<T> lhs) { setLHS(lhs, T::nodeType()); }
      template<astNode_t T> void setRHS(index<T> rhs) { setRHS(rhs, T::nodeType()); }
      template<astNode_t T> void setExtra(index<T> extra) { setExtra(extra, T::nodeType()); }
      template<astNode_t T> void setExtra2(index<T> extra) { setExtra2(extra, T::nodeType()); }

      TypeSpec*& evalType() { return _evalType; }
      const TypeSpec* evalType() const { return _evalType; }
};

namespace mcsl {
   File& write(File&, const clef::Expr&);
};

#endif //EXPR_HPP