#pragma once
#ifndef FUNC_DECL_HPP
#define FUNC_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/node-lists/ArgumentList.hpp"
#include "ast-nodes/Scope.hpp"

struct clef::FunctionDefinition : public clef::Expression {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::FUNC_DEF; }
      static constexpr OpID pseudoOpID() { return OpID::DEF_FUNC_PARAMS; }

      FunctionDefinition():Expression{} {}
      FunctionDefinition(index<Identifier> name, index<ArgList> params):Expression{OpID::DEF_FUNC_PARAMS, name, params} {}
      FunctionDefinition(index<Identifier> name, index<ArgList> params, index<Scope> proc):Expression{OpID::DEF_FUNC_PARAMS, name, params, proc} {}


      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> name() const { return _lhs; }
      index<ArgList>& params() { return reinterpret_cast<index<ArgList>&>(_rhs); }
      index<const ArgList> params() const { return _rhs; }
      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_extras[0]); }
      index<const Scope> procedure() const { return _extras[0]; }
};

namespace mcsl {
   File& write(File&, const clef::FuncDef&);
};

#endif //FUNC_DECL_HPP