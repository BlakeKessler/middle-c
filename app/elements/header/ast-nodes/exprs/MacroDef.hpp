#pragma once
#ifndef MACRO_DECL_HPP
#define MACRO_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expr.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/node-lists/ArgList.hpp"

struct clef::MacroDef : public clef::Expr {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::MACRO_DEF; }
      static constexpr OpID pseudoOpID() { return OpID::DEF_MACRO_PARAMS; }

      MacroDef():Expr{} {}
      MacroDef(index<Identifier> name, index<void> overloadIndex, index<ArgList> params):Expr{OpID::DEF_MACRO_PARAMS, NodeType::IDEN, NodeType::NONE, NodeType::ARG_LIST, name, overloadIndex, params} {}
      MacroDef(index<Identifier> name, index<void> overloadIndex, index<ArgList> params, index<Scope> proc):Expr{OpID::DEF_MACRO_PARAMS, NodeType::IDEN, NodeType::NONE, NodeType::ARG_LIST, NodeType::SCOPE, name, overloadIndex, params, proc} {}


      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> name() const { return _lhs; }
      index<void>& overloadIndex() { return reinterpret_cast<index<void>&>(_rhs); }
      index<const void> overloadIndex() const { return _rhs; }
      index<ArgList>& params() { return reinterpret_cast<index<ArgList>&>(_extras[0]); }
      index<const ArgList> params() const { return _extras[0]; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_extras[1]); }
      index<const Scope> procedure() const { return _extras[1]; }
};

namespace mcsl {
   File& write(File&, const clef::MacroDef&);
};

#endif //MACRO_DECL_HPP