#pragma once
#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"

struct clef::Expression {
   private:
      OperatorID _op;
      NodeType _lhsType;
      NodeType _rhsType;
      NodeType _extraType;
      void* _lhs;
      void* _rhs;
      void* _extra;

      friend class Declaration;
      friend class Loop;
      friend class If;
      friend class Else;
      friend class Match;
      friend class TryCatch;
      friend class Asm;
      
      Expression(OperatorID op, NodeType lhsType, NodeType rhsType, NodeType extraType, void* lhs, void* rhs, void* extra):_op{op},_lhsType{lhsType},_rhsType{rhsType},_extraType{extraType},_lhs{lhs},_rhs{rhs},_extra{extra} {}
   public:
      static constexpr NodeType nodeType() { return NodeType::EXPR; }

      Expression():_op{},_lhsType{},_rhsType{},_lhs{},_rhs{} {}
      Expression(OperatorID op, Expr* lhs, Expr* rhs);
      Expression(OperatorID op, Identifier* lhs, Expr* rhs);
      Expression(OperatorID op, Expr* lhs, Identifier* rhs);
      Expression(OperatorID op, Identifier* lhs, Identifier* rhs);
      Literal* value() const; //evaluate expression
      
      OperatorID opID() const { return _op; }
      NodeType lhsType() const { return _lhsType; }
      NodeType rhsType() const { return _rhsType; }
      NodeType extraType() const { return _extraType; }

      void* lhs() { return _lhs; }
      const void* lhs() const { return _lhs; }
      void* rhs() { return _rhs; }
      const void* rhs() const { return _rhs; }
      void* extra() { return _extra; }
      const void* extra() const { return _extra; }

      template<typename T> void setLHS(T* lhs) requires requires { {T::nodeType()} -> mcsl::is_t<NodeType>; } { _lhs = lhs; _lhsType = T::nodeType(); }
      template<typename T> void setRHS(T* rhs) requires requires { {T::nodeType()} -> mcsl::is_t<NodeType>; } { _rhs = rhs; _rhsType = T::nodeType(); }
      template<typename T> void setExtra(T* extra) requires requires { {T::nodeType()} -> mcsl::is_t<NodeType>; } { _extra = extra; _extraType = T::nodeType(); }

      void setLHS(astNode* lhs) { _lhs = lhs; _lhsType = lhs ? lhs->nodeType() : NodeType::NONE; }
      void setRHS(astNode* rhs) { _rhs = rhs; _rhsType = rhs ? rhs->nodeType() : NodeType::NONE; }
      void setExtra(astNode* extra) { _extra = extra; _extraType = extra ? extra->nodeType() : NodeType::NONE; }
};

#endif //EXPR_HPP