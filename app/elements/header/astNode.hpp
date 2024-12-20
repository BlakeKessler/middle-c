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
   private:
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

   public:
      astNode():_nodeType{NodeType::NONE} {}
      astNode(Expression expr):_expression{std::move(expr)},_nodeType{NodeType::EXPR} {}
      astNode(Statement stmt):_statement{std::move(stmt)},_nodeType{NodeType::STMT} {}
      astNode(StmtSequence stmtSeq):_stmtSeq{std::move(stmtSeq)},_nodeType{NodeType::STMT_SEQ} {}
      astNode(Identifier iden):_identifier{std::move(iden)},_nodeType{NodeType::IDEN} {}
      astNode(Block block):_block{std::move(block)},_nodeType{NodeType::BLOCK} {}
      astNode(Type type):_type{std::move(type)},_nodeType{NodeType::TYPE} {}
      astNode(Literal lit):_literal{std::move(lit)},_nodeType{NodeType::LIT} {}
      astNode(Object obj):_object{std::move(obj)},_nodeType{NodeType::OBJ} {}
      astNode(Function func):_function{std::move(func)},_nodeType{NodeType::FUNC} {}
      astNode(Operator op):_operator{std::move(op)},_nodeType{NodeType::OP} {}
      astNode(Template temp):_template{std::move(temp)},_nodeType{NodeType::TEMPLATE} {}

      ~astNode() {
         switch (_nodeType) {
            case NodeType::EXPR    : _expression.~Expression(); break;
            case NodeType::STMT    : _statement.~Statement();   break;
            case NodeType::STMT_SEQ: _stmtSeq.~StmtSequence();  break;
            case NodeType::IDEN    : _identifier.~Identifier(); break;
            case NodeType::BLOCK   : _block.~Block();           break;
            case NodeType::TYPE    : _type.~Type();             break;
            case NodeType::LIT     : _literal.~Literal();       break;
            case NodeType::OBJ     : _object.~Object();         break;
            case NodeType::FUNC    : _function.~Function();     break;
            case NodeType::OP      : _operator.~Operator();     break;
            case NodeType::TEMPLATE: _template.~Template();     break;
            
            default: break;
         }
      }

      NodeType type() const { return _nodeType; }

      [[noreturn]] void throwCastErr(NodeType target) const { throwError(ErrCode::BAD_NODE_CAST, "%u to %u", (uint)_nodeType, (uint)target); }



      #pragma region cast
      operator Expression&()   { if (_nodeType == NodeType::EXPR    ) { return _expression; } else { throwCastErr(NodeType::EXPR    ); } }
      operator Statement&()    { if (_nodeType == NodeType::STMT    ) { return _statement ; } else { throwCastErr(NodeType::STMT    ); } }
      operator StmtSequence&() { if (_nodeType == NodeType::STMT_SEQ) { return _stmtSeq   ; } else { throwCastErr(NodeType::STMT_SEQ); } }
      operator Identifier&()   { if (_nodeType == NodeType::IDEN    ) { return _identifier; } else { throwCastErr(NodeType::IDEN    ); } }
      operator Block&()        { if (_nodeType == NodeType::BLOCK   ) { return _block     ; } else { throwCastErr(NodeType::BLOCK   ); } }
      operator Type&()         { if (_nodeType == NodeType::TYPE    ) { return _type      ; } else { throwCastErr(NodeType::TYPE    ); } }
      operator Literal&()      { if (_nodeType == NodeType::LIT     ) { return _literal   ; } else { throwCastErr(NodeType::LIT     ); } }
      operator Object&()       { if (_nodeType == NodeType::OBJ     ) { return _object    ; } else { throwCastErr(NodeType::OBJ     ); } }
      operator Function&()     { if (_nodeType == NodeType::FUNC    ) { return _function  ; } else { throwCastErr(NodeType::FUNC    ); } }
      operator Operator&()     { if (_nodeType == NodeType::OP      ) { return _operator  ; } else { throwCastErr(NodeType::OP      ); } }
      operator Template&()     { if (_nodeType == NodeType::TEMPLATE) { return _template  ; } else { throwCastErr(NodeType::TEMPLATE); } }

      operator const Expression&()   const { if (_nodeType == NodeType::EXPR    ) { return _expression; } else { throwCastErr(NodeType::EXPR    ); } }
      operator const Statement&()    const { if (_nodeType == NodeType::STMT    ) { return _statement ; } else { throwCastErr(NodeType::STMT    ); } }
      operator const StmtSequence&() const { if (_nodeType == NodeType::STMT_SEQ) { return _stmtSeq   ; } else { throwCastErr(NodeType::STMT_SEQ); } }
      operator const Identifier&()   const { if (_nodeType == NodeType::IDEN    ) { return _identifier; } else { throwCastErr(NodeType::IDEN    ); } }
      operator const Block&()        const { if (_nodeType == NodeType::BLOCK   ) { return _block     ; } else { throwCastErr(NodeType::BLOCK   ); } }
      operator const Type&()         const { if (_nodeType == NodeType::TYPE    ) { return _type      ; } else { throwCastErr(NodeType::TYPE    ); } }
      operator const Literal&()      const { if (_nodeType == NodeType::LIT     ) { return _literal   ; } else { throwCastErr(NodeType::LIT     ); } }
      operator const Object&()       const { if (_nodeType == NodeType::OBJ     ) { return _object    ; } else { throwCastErr(NodeType::OBJ     ); } }
      operator const Function&()     const { if (_nodeType == NodeType::FUNC    ) { return _function  ; } else { throwCastErr(NodeType::FUNC    ); } }
      operator const Operator&()     const { if (_nodeType == NodeType::OP      ) { return _operator  ; } else { throwCastErr(NodeType::OP      ); } }
      operator const Template&()     const { if (_nodeType == NodeType::TEMPLATE) { return _template  ; } else { throwCastErr(NodeType::TEMPLATE); } }
      #pragma endregion cast
};

#endif //AST_NODE_HPP