#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Function.hpp"
#include "ast-nodes/Literal.hpp"
#include "ast-nodes/Scope.hpp"
#include "ast-nodes/Type.hpp"
#include "ast-nodes/ForLoopParams.hpp"
#include "ast-nodes/ForeachLoopParams.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/Variable.hpp"
#include "ast-nodes/stmts/Declaration.hpp"
#include "ast-nodes/stmts/If.hpp"
#include "ast-nodes/stmts/Loop.hpp"
#include "ast-nodes/stmts/Switch.hpp"
#include "ast-nodes/stmts/Match.hpp"
#include "ast-nodes/stmts/TryCatch.hpp"
// #include "ast-nodes/stmts/Asm.hpp"
#include "ast-nodes/node-lists/ArgumentList.hpp"
#include "ast-nodes/node-lists/MatchCases.hpp"
#include "ast-nodes/node-lists/ParameterList.hpp"
#include "ast-nodes/node-lists/StatementSequence.hpp"
#include "ast-nodes/node-lists/SwitchCases.hpp"
#include "ast-nodes/types/Class.hpp"
#include "ast-nodes/types/Enum.hpp"
#include "ast-nodes/types/Mask.hpp"
#include "ast-nodes/types/FunctionSignature.hpp"
#include "ast-nodes/types/FundamentalType.hpp"
#include "ast-nodes/types/GenericType.hpp"
#include "ast-nodes/types/Interface.hpp"
#include "ast-nodes/types/Namespace.hpp"
#include "ast-nodes/types/Struct.hpp"
#include "ast-nodes/types/Union.hpp"
#include "ast-nodes/types/VariadicParameter.hpp"

#include "InterfaceSpec.hpp"
#include "NamespaceSpec.hpp"
#include "ObjTypeSpec.hpp"

struct clef::astNode {
   private:
      union {
         Identifier _identifier;
         Variable _variable;
         Function _function;
         Type _type;
         VariadicParameter _variadicParameter;
         FundamentalType _fundamentalType;
         FunctionSignature _functionSignature;
         Enum _enum;
         Mask _mask;
         Union _union;
         Namespace _namespace;
         Interface _interface;
         Struct _struct;
         Class _class;
         GenericType _genericType;
         Scope _scope;
         Literal _literal;
         Expression _expression;
         Declaration _declaration;
         Loop _loop;
         If _if;
         Switch _switch;
         Match _match;
         TryCatch _tryCatch;
         // Asm _asm;
         ForLoopParams _forLoopParams;
         ForeachLoopParams _foreachLoopParams;
         SwitchCases _switchCases;
         MatchCases _matchCases;
         Statement _statement;
         StatementSequence _statementSequence;
         ArgumentList _argumentList;
         ParameterList _parameterList;
      };
      NodeType _nodeType;

      friend class SyntaxTree;
   public:
      #pragma region constructor
      /*unsafe<UNINIT_MEM>*/ astNode(const NodeType type = NodeType::NONE):_nodeType{type} {}
      #define _def_ctor(varName) astNode(decltype(varName)& node):varName{node},_nodeType{node.nodeType()} {}
      _def_ctor(_identifier)
      _def_ctor(_variable)
      _def_ctor(_function)
      _def_ctor(_type)
      _def_ctor(_variadicParameter)
      _def_ctor(_fundamentalType)
      _def_ctor(_functionSignature)
      _def_ctor(_enum)
      _def_ctor(_mask)
      _def_ctor(_union)
      _def_ctor(_namespace)
      _def_ctor(_interface)
      _def_ctor(_struct)
      _def_ctor(_class)
      _def_ctor(_genericType)
      _def_ctor(_scope)
      _def_ctor(_literal)
      _def_ctor(_expression)
      _def_ctor(_declaration)
      _def_ctor(_loop)
      _def_ctor(_if)
      _def_ctor(_switch)
      _def_ctor(_match)
      _def_ctor(_tryCatch)
      // _def_ctor(_asm)
      _def_ctor(_forLoopParams)
      _def_ctor(_foreachLoopParams)
      _def_ctor(_switchCases)
      _def_ctor(_matchCases)
      _def_ctor(_statement)
      _def_ctor(_statementSequence)
      _def_ctor(_argumentList)
      _def_ctor(_parameterList)
      #undef _def_ctor
      #pragma endregion constructors

      NodeType nodeType() const { return _nodeType; }
      void downCast(NodeType t);
      void upCast(NodeType t);
      void verticalCast(NodeType t);
      void anyCast(NodeType t);

      [[noreturn]] void throwCastErr(NodeType target) const { throwError(ErrCode::BAD_NODE_CAST, "%u to %u", (uint)_nodeType, (uint)target); }



      #pragma region cast
      #define _def_cast_func(varName) \
         operator decltype(varName)&() { if (_nodeType == decltype(varName)::nodeType()) { return varName; } else { throwCastErr(decltype(varName)::nodeType()); }} \
         operator const decltype(varName)&() const { if (_nodeType == decltype(varName)::nodeType()) { return varName; } else { throwCastErr(decltype(varName)::nodeType()); }}
      _def_cast_func(_identifier)
      _def_cast_func(_variable)
      _def_cast_func(_function)
      _def_cast_func(_type)
      _def_cast_func(_variadicParameter)
      _def_cast_func(_fundamentalType)
      _def_cast_func(_functionSignature)
      _def_cast_func(_enum)
      _def_cast_func(_mask)
      _def_cast_func(_union)
      _def_cast_func(_namespace)
      _def_cast_func(_interface)
      _def_cast_func(_struct)
      _def_cast_func(_class)
      _def_cast_func(_genericType)
      _def_cast_func(_scope)
      _def_cast_func(_literal)
      _def_cast_func(_expression)
      _def_cast_func(_declaration)
      _def_cast_func(_loop)
      _def_cast_func(_if)
      _def_cast_func(_switch)
      _def_cast_func(_match)
      _def_cast_func(_tryCatch)
      // _def_cast_func(_asm)
      _def_cast_func(_forLoopParams)
      _def_cast_func(_foreachLoopParams)
      _def_cast_func(_switchCases)
      _def_cast_func(_matchCases)
      _def_cast_func(_statement)
      _def_cast_func(_statementSequence)
      _def_cast_func(_argumentList)
      _def_cast_func(_parameterList)

      #undef _def_cast_func
      #pragma endregion cast
};

#endif //AST_NODE_HPP