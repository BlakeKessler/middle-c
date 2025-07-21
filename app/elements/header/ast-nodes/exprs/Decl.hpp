#pragma once
#ifndef DECL_HPP
#define DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expr.hpp"
#include "ast-nodes/Identifier.hpp"

struct clef::Decl : public clef::Expr {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::DECL; }
      static constexpr OpID pseudoOpID() { return OpID::LET; }

      Decl():Expr{} {}
      Decl(index<Identifier> type, index<Identifier> name):Expr{OpID::LET,type,name} {}
      Decl(index<Identifier> type, index<Identifier> name, index<Expr> value):Expr{OpID::LET,type,name,value} {}


      index<Identifier>& type() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> type() const { return _lhs; }
      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_rhs); }
      index<const Identifier> name() const { return _rhs; }
      index<Expr>& value() { return reinterpret_cast<index<Expr>&>(_extras[0]); }
      index<const Expr> value() const { return _extras[0]; }
};

namespace mcsl {
   File& write(File&, const clef::Decl&);
};

#endif //DECL_HPP