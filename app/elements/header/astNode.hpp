#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Expr.hpp"
#include "ast-nodes/Literal.hpp"
#include "ast-nodes/Scope.hpp"
#include "ast-nodes/RawIdentifier.hpp"
#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Stmt.hpp"
#include "ast-nodes/exprs/Decl.hpp"
#include "ast-nodes/exprs/FuncDef.hpp"
#include "ast-nodes/exprs/MacroDef.hpp"
#include "ast-nodes/exprs/TypeDecl.hpp"
#include "ast-nodes/exprs/If.hpp"
#include "ast-nodes/exprs/ForLoop.hpp"
#include "ast-nodes/exprs/ForeachLoop.hpp"
#include "ast-nodes/exprs/WhileLoop.hpp"
#include "ast-nodes/exprs/DoWhileLoop.hpp"
#include "ast-nodes/exprs/Switch.hpp"
#include "ast-nodes/exprs/Match.hpp"
#include "ast-nodes/exprs/Asm.hpp"
#include "ast-nodes/node-lists/ArgList.hpp"
#include "ast-nodes/node-lists/MatchCases.hpp"
#include "ast-nodes/node-lists/StmtSeq.hpp"
#include "ast-nodes/node-lists/SwitchCases.hpp"


#include "MAP_MACRO.h"
#define CLEF_ALL_AST_NODE_UNION_MEMBS \
   _rawIden, _identifier, _scope, _literal, _expr, _decl, _typeDecl, _funcDef, \
   _macroDef, _forLoop, _foreachLoop, _whileLoop, _doWhileLoop, _if, _switch, \
   _match, _asm, _switchCases, _matchCases, _stmt, _stmtSeq, _argList

struct clef::astNode {
   private:
      union {
         RawIdentifier _rawIden;
         Identifier _identifier;
         Scope _scope;
         Literal _literal;
         Expr _expr;
         Decl _decl;
         FuncDef _funcDef;
         MacroDef _macroDef;
         TypeDecl _typeDecl;
         ForLoop _forLoop;
         ForeachLoop _foreachLoop;
         WhileLoop _whileLoop;
         DoWhileLoop _doWhileLoop;
         If _if;
         Switch _switch;
         Match _match;
         Asm _asm;
         SwitchCases _switchCases;
         MatchCases _matchCases;
         Stmt _stmt;
         StmtSeq _stmtSeq;
         ArgList _argList;
      };
      NodeType _nodeType;

      friend class SyntaxTree;
   public:
      #pragma region constructor
      /*unsafe<UNINIT_MEM>*/ astNode(const NodeType type = NodeType::NONE):_nodeType{type} {
         #if defined SAFE_MODE || !defined(NDEBUG)
         std::memset((void*)this, 0, sizeof(astNode) - sizeof(NodeType));
         #endif
      }
      astNode(const astNode& other) { std::memcpy((void*)this, &other, sizeof(self)); }
      astNode& operator=(const astNode& other) { new (this) astNode{other}; return self; }
      #define _def_ctor(varName) \
         astNode(const decltype(varName)& node):varName{node},_nodeType{decltype(varName)::nodeType()} {} \
         astNode(decltype(varName)&& node):varName{std::forward<decltype(node)>(node)},_nodeType{decltype(varName)::nodeType()} {}
      MCSL_MAP(_def_ctor, CLEF_ALL_AST_NODE_UNION_MEMBS)
      #undef _def_ctor
      #pragma endregion constructors

      NodeType nodeType() const { return _nodeType; }
      void downCast(NodeType);
      void upCast(NodeType);
      void verticalCast(NodeType);
      void anyCast(NodeType);

      [[noreturn]] void throwCastErr(NodeType target) const { internalError(ErrCode::BAD_NODE_CAST, mcsl::FMT("%u to %u"), +_nodeType, +target); }



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