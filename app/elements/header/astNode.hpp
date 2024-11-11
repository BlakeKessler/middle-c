#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Expression.hpp"
#include "ast-nodes/Statement.hpp"
#include "ast-nodes/StmtSequence.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Block.hpp"
#include "ast-nodes/Type.hpp"
#include "ast-nodes/Literal.hpp"
#include "ast-nodes/Object.hpp"
#include "ast-nodes/Function.hpp"
#include "ast-nodes/Operator.hpp"
#include "ast-nodes/Template.hpp"

struct clef::astNode {
   union {
      Expression _expression;
      Statement _statement;
      StmtSequence _stmtSeq;
      Identifier _identifier;
      Block _block;
      Type _type;
      Literal _literal;
      Object _object;
      Function _function;
      Operator _operator;
      Template _template;
   };
   NodeType _nodeType;
};

#endif //AST_NODE_HPP