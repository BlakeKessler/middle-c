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
#include "ast-nodes/stmts/Else.hpp"
#include "ast-nodes/stmts/ElseIf.hpp"
#include "ast-nodes/stmts/If.hpp"
#include "ast-nodes/stmts/Loop.hpp"
#include "ast-nodes/stmts/Switch.hpp"
#include "ast-nodes/stmts/Match.hpp"
#include "ast-nodes/stmts/TryCatch.hpp"
#include "ast-nodes/stmts/Asm.hpp"
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
         Else _else;
         ElseIf _elseIf;
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
      };
      NodeType _nodeType;

      friend class SyntaxTree;
   public:
      #pragma region constructor
      astNode():_nodeType{NodeType::NONE} {}
      /*unsafe<UNINIT_MEM>*/ astNode(const NodeType type):_nodeType{type} {}
      astNode(Identifier& node):_identifier{node},_nodeType{NodeType::IDEN} {}
      astNode(Variable& node):_variable{node},_nodeType{NodeType::VAR} {}
      astNode(Function& node):_function{node},_nodeType{NodeType::FUNC} {}
      astNode(Type& node):_type{node},_nodeType{NodeType::TYPE} {}
      astNode(VariadicParameter& node):_variadicParameter{node},_nodeType{NodeType::VAR_PARAM} {}
      astNode(FundamentalType& node):_fundamentalType{node},_nodeType{NodeType::FUND_TYPE} {}
      astNode(FunctionSignature& node):_functionSignature{node},_nodeType{NodeType::FUNC_SIG} {}
      astNode(Enum& node):_enum{node},_nodeType{NodeType::ENUM} {}
      astNode(Mask& node):_mask{node},_nodeType{NodeType::MASK} {}
      astNode(Union& node):_union{node},_nodeType{NodeType::UNION} {}
      astNode(Namespace& node):_namespace{node},_nodeType{NodeType::NAMESPACE} {}
      astNode(Interface& node):_interface{node},_nodeType{NodeType::INTERFACE} {}
      astNode(Struct& node):_struct{node},_nodeType{NodeType::STRUCT} {}
      astNode(Class& node):_class{node},_nodeType{NodeType::CLASS} {}
      astNode(GenericType& node):_genericType{node},_nodeType{NodeType::GENERIC} {}
      astNode(Scope& node):_scope{node},_nodeType{NodeType::SCOPE} {}
      astNode(Literal& node):_literal{node},_nodeType{NodeType::LITERAL} {}
      astNode(Expression& node):_expression{node},_nodeType{NodeType::EXPR} {}
      astNode(Declaration& node):_declaration{node},_nodeType{NodeType::DECL} {}
      astNode(Loop& node):_loop{node},_nodeType{NodeType::LOOP} {}
      astNode(If& node):_if{node},_nodeType{NodeType::IF} {}
      astNode(Else& node):_else{node},_nodeType{NodeType::ELSE} {}
      astNode(ElseIf& node):_elseIf{node},_nodeType{NodeType::ELSE_IF} {}
      astNode(Switch& node):_switch{node},_nodeType{NodeType::SWITCH} {}
      astNode(Match& node):_match{node},_nodeType{NodeType::MATCH} {}
      astNode(TryCatch& node):_tryCatch{node},_nodeType{NodeType::TRY_CATCH} {}
      astNode(Asm& node):_asm{node},_nodeType{NodeType::ASM} {}
      astNode(ForLoopParams& node):_forLoopParams{node},_nodeType{NodeType::FOR_LOOP_PARAMS} {}
      astNode(ForeachLoopParams& node):_foreachLoopParams{node},_nodeType{NodeType::FOREACH_LOOP_PARAMS} {}
      astNode(SwitchCases& node):_switchCases{node},_nodeType{NodeType::SWITCH_CASES} {}
      astNode(MatchCases& node):_matchCases{node},_nodeType{NodeType::MATCH_CASES} {}
      astNode(Statement& node):_statement{node},_nodeType{NodeType::STMT} {}
      astNode(StatementSequence& node):_statementSequence{node},_nodeType{NodeType::STMT_SEQ} {}
      astNode(ArgumentList& node):_argumentList{node},_nodeType{NodeType::ARG_LIST} {}
      astNode(ParameterList& node):_parameterList{node},_nodeType{NodeType::PARAM_LIST} {}
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
      _def_cast_func(_else)
      _def_cast_func(_elseIf)
      _def_cast_func(_switch)
      _def_cast_func(_match)
      _def_cast_func(_tryCatch)
      _def_cast_func(_asm)
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