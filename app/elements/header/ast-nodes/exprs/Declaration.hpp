#pragma once
#ifndef DECL_HPP
#define DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Type.hpp"

struct clef::Declaration : public clef::Expression {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::DECL; }

      Declaration():Expression{} {}
      Declaration(index<Type> type, index<Identifier> name):Expression{OpID::LET,type,name} {}
      Declaration(index<Type> type, index<Identifier> name, index<Expr> value):Expression{OpID::LET,type,name,value} {}


      index<Type>& type() { return reinterpret_cast<index<Type>&>(_lhs); }
      index<const Type> type() const { return _lhs; }
      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_rhs); }
      index<const Identifier> name() const { return _rhs; }
      index<Expr>& value() { return reinterpret_cast<index<Expr>&>(_extra); }
      index<const Expr> value() const { return _extra; }
};

namespace mcsl {
   File& write(File&, const clef::Decl&);
};

#endif //DECL_HPP