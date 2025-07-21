#pragma once
#ifndef FUNC_DECL_HPP
#define FUNC_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expr.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/node-lists/ArgList.hpp"
#include "ast-nodes/Scope.hpp"

struct clef::FuncDef : public clef::Expr {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_DEF; }
      static constexpr OpID pseudoOpID() { return OpID::DEF_FUNC_PARAMS; }

      FuncDef():Expr{} {}
      FuncDef(index<Identifier> name, index<ArgList> params):Expr{OpID::DEF_FUNC_PARAMS, name, params} {}
      FuncDef(index<Identifier> name, index<ArgList> params, index<Scope> proc):Expr{OpID::DEF_FUNC_PARAMS, name, params, proc} {}
      FuncDef(index<Identifier> name, index<ArgList> params, index<Expr> attrs):Expr{OpID::DEF_FUNC_PARAMS, name, params, index<Scope>(0), attrs} {}
      FuncDef(index<Identifier> name, index<ArgList> params, index<Scope> proc, index<Expr> attrs):Expr{OpID::DEF_FUNC_PARAMS, name, params, proc, attrs} {}


      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> name() const { return _lhs; }
      index<ArgList>& params() { return reinterpret_cast<index<ArgList>&>(_rhs); }
      index<const ArgList> params() const { return _rhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_extras[0]); }
      index<const Scope> procedure() const { return _extras[0]; }
      index<Expr>& attrs() { return reinterpret_cast<index<Expr>&>(_extras[1]); }
      index<const Expr> attrs() const { return _extras[1]; }
};

namespace mcsl {
   File& write(File&, const clef::FuncDef&);
};

#endif //FUNC_DECL_HPP