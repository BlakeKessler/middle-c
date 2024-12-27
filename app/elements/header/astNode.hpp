#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"
#include "allocator.hpp"

#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Function.hpp"
#include "ast-nodes/Literal.hpp"
#include "ast-nodes/Scope.hpp"
#include "ast-nodes/Type.hpp"
#include "ast-nodes/ForLoopParams.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Operator.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/Variable.hpp"
#include "ast-nodes/exprs/Declaration.hpp"
#include "ast-nodes/exprs/Else.hpp"
#include "ast-nodes/exprs/ElseIf.hpp"
#include "ast-nodes/exprs/If.hpp"
#include "ast-nodes/exprs/Loop.hpp"
#include "ast-nodes/exprs/Match.hpp"
#include "ast-nodes/exprs/Switch.hpp"
#include "ast-nodes/node-lists/ArgumentList.hpp"
#include "ast-nodes/node-lists/MatchCases.hpp"
#include "ast-nodes/node-lists/ParameterList.hpp"
#include "ast-nodes/node-lists/StatementSequence.hpp"
#include "ast-nodes/node-lists/SwitchCases.hpp"
#include "ast-nodes/types/Class.hpp"
#include "ast-nodes/types/Enum.hpp"
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
      static allocator* _CURR_ALLOC;

      union {
         Identifier _identifier;
         Variable _variable;
         Function _function;
         Type _type;
         VariadicParameter _variadicParameter;
         FundamentalType _fundamentalType;
         FunctionSignature _functionSignature;
         Enum _enum;
         Union _union;
         Namespace _namespace;
         Interface _interface;
         Struct _struct;
         Class _class;
         GenericType _genericType;
         Scope _scope;
         Literal _literal;
         Operator _operator;
         Expression _expression;
         Declaration _declaration;
         Loop _loop;
         If _if;
         Else _else;
         ElseIf _elseIf;
         Switch _switch;
         Match _match;
         ForLoopParams _forLoopParams;
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
      astNode(Identifier& node):_identifier{node},_nodeType{NodeType::IDEN} {}
      astNode(Variable& node):_variable{node},_nodeType{NodeType::VAR} {}
      astNode(Function& node):_function{node},_nodeType{NodeType::FUNC} {}
      astNode(Type& node):_type{node},_nodeType{NodeType::TYPE} {}
      astNode(VariadicParameter& node):_variadicParameter{node},_nodeType{NodeType::VAR_PARAM} {}
      astNode(FundamentalType& node):_fundamentalType{node},_nodeType{NodeType::FUND_TYPE} {}
      astNode(FunctionSignature& node):_functionSignature{node},_nodeType{NodeType::FUNC_SIG} {}
      astNode(Enum& node):_enum{node},_nodeType{NodeType::ENUM} {}
      astNode(Union& node):_union{node},_nodeType{NodeType::UNION} {}
      astNode(Namespace& node):_namespace{node},_nodeType{NodeType::NAMESPACE} {}
      astNode(Interface& node):_interface{node},_nodeType{NodeType::INTERFACE} {}
      astNode(Struct& node):_struct{node},_nodeType{NodeType::STRUCT} {}
      astNode(Class& node):_class{node},_nodeType{NodeType::CLASS} {}
      astNode(GenericType& node):_genericType{node},_nodeType{NodeType::GENERIC} {}
      astNode(Scope& node):_scope{node},_nodeType{NodeType::SCOPE} {}
      astNode(Literal& node):_literal{node},_nodeType{NodeType::LITERAL} {}
      astNode(Operator& node):_operator{node},_nodeType{NodeType::OPERATOR} {}
      astNode(Expression& node):_expression{node},_nodeType{NodeType::EXPR} {}
      astNode(Declaration& node):_declaration{node},_nodeType{NodeType::DECL} {}
      astNode(Loop& node):_loop{node},_nodeType{NodeType::LOOP} {}
      astNode(If& node):_if{node},_nodeType{NodeType::IF} {}
      astNode(Else& node):_else{node},_nodeType{NodeType::ELSE} {}
      astNode(ElseIf& node):_elseIf{node},_nodeType{NodeType::ELSE_IF} {}
      astNode(Switch& node):_switch{node},_nodeType{NodeType::SWITCH} {}
      astNode(Match& node):_match{node},_nodeType{NodeType::MATCH} {}
      astNode(ForLoopParams& node):_forLoopParams{node},_nodeType{NodeType::FOR_LOOP_PARAMS} {}
      astNode(SwitchCases& node):_switchCases{node},_nodeType{NodeType::SWITCH_CASES} {}
      astNode(MatchCases& node):_matchCases{node},_nodeType{NodeType::MATCH_CASES} {}
      astNode(Statement& node):_statement{node},_nodeType{NodeType::STMT} {}
      astNode(StatementSequence& node):_statementSequence{node},_nodeType{NodeType::STMT_SEQ} {}
      astNode(ArgumentList& node):_argumentList{node},_nodeType{NodeType::ARG_LIST} {}
      astNode(ParameterList& node):_parameterList{node},_nodeType{NodeType::PARAM_LIST} {}
      #pragma endregion constructors

      NodeType type() const { return _nodeType; }
      void downCast(NodeType t);
      void upCast(NodeType t);
      void verticalCast(NodeType t);
      void anyCast(NodeType t);

      template<typename T> static index<T> allocBuf(mcsl::is_t<T> auto... initList);
      template<typename T> static mcsl::dyn_arr<T>& derefBuf(const index<T> i) { return _CURR_ALLOC->at(i); }
      template<typename T> static mcsl::dyn_arr<T>* moveBuf(mcsl::dyn_arr<T>&&);

      [[noreturn]] void throwCastErr(NodeType target) const { throwError(ErrCode::BAD_NODE_CAST, "%u to %u", (uint)_nodeType, (uint)target); }



      #pragma region cast
      #define _def_cast_func(type, nodetype, varName) \
         operator type&() { if (_nodeType == NodeType::nodetype) { return varName; } else { throwCastErr(NodeType::nodetype); }} \
         operator const type&() const { if (_nodeType == NodeType::nodetype) { return varName; } else { throwCastErr(NodeType::nodetype); }}
      _def_cast_func(Identifier, IDEN, _identifier)
      _def_cast_func(Variable, VAR, _variable)
      _def_cast_func(Function, FUNC, _function)
      _def_cast_func(Type, TYPE, _type)
      _def_cast_func(VariadicParameter, VAR_PARAM, _variadicParameter)
      _def_cast_func(FundamentalType, FUND_TYPE, _fundamentalType)
      _def_cast_func(FunctionSignature, FUNC_SIG, _functionSignature)
      _def_cast_func(Enum, ENUM, _enum)
      _def_cast_func(Union, UNION, _union)
      _def_cast_func(Namespace, NAMESPACE, _namespace)
      _def_cast_func(Interface, INTERFACE, _interface)
      _def_cast_func(Struct, STRUCT, _struct)
      _def_cast_func(Class, CLASS, _class)
      _def_cast_func(GenericType, GENERIC, _genericType)
      _def_cast_func(Scope, SCOPE, _scope)
      _def_cast_func(Literal, LITERAL, _literal)
      _def_cast_func(Operator, OPERATOR, _operator)
      _def_cast_func(Expression, EXPR, _expression)
      _def_cast_func(Declaration, DECL, _declaration)
      _def_cast_func(Loop, LOOP, _loop)
      _def_cast_func(If, IF, _if)
      _def_cast_func(Else, ELSE, _else)
      _def_cast_func(ElseIf, ELSE_IF, _elseIf)
      _def_cast_func(Switch, SWITCH, _switch)
      _def_cast_func(Match, MATCH, _match)
      _def_cast_func(ForLoopParams, FOR_LOOP_PARAMS, _forLoopParams)
      _def_cast_func(SwitchCases, SWITCH_CASES, _switchCases)
      _def_cast_func(MatchCases, MATCH_CASES, _matchCases)
      _def_cast_func(Statement, STMT, _statement)
      _def_cast_func(StatementSequence, STMT_SEQ, _statementSequence)
      _def_cast_func(ArgumentList, ARG_LIST, _argumentList)
      _def_cast_func(ParameterList, PARAM_LIST, _parameterList)

      #undef _def_cast_func
      #pragma endregion cast
};

#endif //AST_NODE_HPP