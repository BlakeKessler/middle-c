#pragma once
#ifndef FUNC_DECL_HPP
#define FUNC_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/node-lists/ArgumentList.hpp"

struct clef::FunctionDefinition : public clef::Expression {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_DEF; }
      static constexpr OpID pseudoOpID() { return OpID::DEF_FUNC_PARAMS; }

      FunctionDefinition():Expression{} {}
      FunctionDefinition(index<Identifier> name, index<void> overloadIndex, index<ArgList> params):Expression{OpID::DEF_FUNC_PARAMS, NodeType::IDEN, NodeType::NONE, NodeType::ARG_LIST, name, overloadIndex, params} {}
      FunctionDefinition(index<Identifier> name, index<void> overloadIndex, index<ArgList> params, index<Scope> proc):Expression{OpID::DEF_FUNC_PARAMS, NodeType::IDEN, NodeType::NONE, NodeType::ARG_LIST, NodeType::SCOPE, name, overloadIndex, params, proc} {}


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
   File& write(File&, const clef::FuncDef&);
};

#endif //FUNC_DECL_HPP