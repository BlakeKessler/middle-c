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
#include "ast-nodes/exprs/Declaration.hpp"
#include "ast-nodes/exprs/If.hpp"
#include "ast-nodes/exprs/ForLoop.hpp"
#include "ast-nodes/exprs/ForeachLoop.hpp"
#include "ast-nodes/exprs/WhileLoop.hpp"
#include "ast-nodes/exprs/DoWhileLoop.hpp"
#include "ast-nodes/exprs/Switch.hpp"
#include "ast-nodes/exprs/Match.hpp"
#include "ast-nodes/exprs/TryCatch.hpp"
#include "ast-nodes/exprs/Asm.hpp"
#include "ast-nodes/node-lists/ArgumentList.hpp"
#include "ast-nodes/node-lists/MatchCases.hpp"
#include "ast-nodes/node-lists/ParameterList.hpp"
#include "ast-nodes/node-lists/SpecializerList.hpp"
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


#include "MAP_MACRO.h"
#define CLEF_ALL_AST_NODE_UNION_MEMBS \
   _identifier, _variable, _function, _type, _variadicParameter, _fundamentalType, _functionSignature, _enum,           \
   _mask, _union, _namespace, _interface, _struct, _class, _genericType, _scope, _literal, _expression, _declaration,   \
   _forLoop, _foreachLoop, _whileLoop, _doWhileLoop, _if, _switch, _match, _tryCatch, _asm, _forLoopParams,             \
   _foreachLoopParams, _switchCases, _matchCases, _statement, _statementSequence, _argumentList, _parameterList, _specializerList

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
         ForLoop _forLoop;
         ForeachLoop _foreachLoop;
         WhileLoop _whileLoop;
         DoWhileLoop _doWhileLoop;
         If _if;
         Switch _switch;
         Match _match;
         TryCatch _tryCatch;
         Asm _asm;
         ForLoopParams _forLoopParams;
         ForeachLoopParams _foreachLoopParams;
         SwitchCases _switchCases;
         MatchCases _matchCases;
         Statement _statement;
         StatementSequence _statementSequence;
         ArgumentList _argumentList;
         ParameterList _parameterList;
         SpecializerList _specializerList;
      };
      NodeType _nodeType;

      friend class SyntaxTree;
   public:
      #pragma region constructor
      /*unsafe<UNINIT_MEM>*/ astNode(const NodeType type = NodeType::NONE):_nodeType{type} {
         #if defined SAFE_MODE || !defined(NDEBUG)
         std::memset(this, 0, sizeof(astNode) - sizeof(NodeType));
         #endif
      }
      astNode(const astNode& other) { std::memcpy(this, &other, sizeof(self)); }
      astNode& operator=(const astNode& other) { new (this) astNode{other}; return self; }
      #define _def_ctor(varName) astNode(decltype(varName)& node):varName{node},_nodeType{decltype(varName)::nodeType()} {}
      MCSL_MAP(_def_ctor, CLEF_ALL_AST_NODE_UNION_MEMBS)
      #undef _def_ctor
      #pragma endregion constructors

      NodeType nodeType() const { return _nodeType; }
      void downCast(NodeType);
      void upCast(NodeType);
      void verticalCast(NodeType);
      void anyCast(NodeType);

      [[noreturn]] void throwCastErr(NodeType target) const { throwError(ErrCode::BAD_NODE_CAST, mcsl::FMT("%u to %u"), +_nodeType, +target); }



      #pragma region cast
      #define _def_cast_func(varName) \
         operator decltype(varName)&() { if (_nodeType == decltype(varName)::nodeType()) { return varName; } else { throwCastErr(decltype(varName)::nodeType()); }} \
         operator const decltype(varName)&() const { if (_nodeType == decltype(varName)::nodeType()) { return varName; } else { throwCastErr(decltype(varName)::nodeType()); }}
      MCSL_MAP(_def_cast_func, CLEF_ALL_AST_NODE_UNION_MEMBS)
      #undef _def_cast_func
      #pragma endregion cast

      void print() const;
      void writeTo(mcsl::File&) const;
};

namespace mcsl {
   inline File& write(File& file, const clef::astNode& obj) { obj.writeTo(file); return file; }
};

namespace clef {
   uint nodeSizeof(NodeType);
};

#include "MAP_MACRO_UNDEF.h"

#endif //AST_NODE_HPP