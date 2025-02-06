#ifndef AST_NODE_CPP
#define AST_NODE_CPP

#include "astNode.hpp"

#include <cstdio>
#include <cstring>
#include <cmath>

//derived to base
void clef::astNode::downCast(NodeType newType) {
   if (canDownCastTo(_nodeType, newType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to downcast from %hhu to %hhu", +_nodeType, +newType);
   }
}
//base to derived
void clef::astNode::upCast(NodeType newType) {
   if (canDownCastTo(newType, _nodeType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to upcast from %hhu to %hhu", +_nodeType, +newType);
   }
}
//derived to base or base to derived
void clef::astNode::verticalCast(NodeType newType) {
   if (canDownCastTo(_nodeType, newType) || canDownCastTo(newType, _nodeType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to vertical cast from %hhu to %hhu", +_nodeType, +newType);
   }
}
//cast without type checking
void clef::astNode::anyCast(NodeType newType) {
   _nodeType = newType;
}


//sizeof node from node type
#define typecase(type) case type::nodeType(): return sizeof(type)
uint clef::nodeSizeof(NodeType t) {
   switch (t) {
      case NodeType::NONE: return 0;
      case NodeType::ERROR: return 0;

      typecase(Identifier);
      typecase(Variable);
      typecase(Function);
      typecase(Type);
      typecase(VariadicParameter);
      typecase(FundamentalType);
      typecase(FunctionSignature);
      typecase(Enum);
      typecase(Mask);
      typecase(Union);
      typecase(Namespace);
      typecase(Interface);
      typecase(Struct);
      typecase(Class);
      typecase(GenericType);
      typecase(Scope);
      typecase(Literal);
      typecase(Expression);
      typecase(Declaration);
      typecase(ForLoop);
      typecase(ForeachLoop);
      typecase(WhileLoop);
      typecase(DoWhileLoop);
      typecase(If);
      typecase(Switch);
      typecase(Match);
      typecase(TryCatch);
      typecase(Asm);
      typecase(ForLoopParams);
      typecase(ForeachLoopParams);
      typecase(SwitchCases);
      typecase(MatchCases);
      typecase(Statement);
      typecase(StatementSequence);
      typecase(ArgumentList);
      typecase(ParameterList);
   }
}
#undef typecase

//unformatted print
void clef::astNode::print() const {
   const uint byteCount = nodeSizeof(_nodeType);
   const uint intCount = (uint)std::ceilf(((float)byteCount) / sizeof(ulong));
   ulong buf[sizeof(astNode) / sizeof(ulong) + (sizeof(astNode) % sizeof(ulong) != 0)]{};
   std::memcpy(buf, this, byteCount);

   std::printf("%hhu:", +_nodeType);
   for (uint i = 0; i < intCount; ++i) {
      std::printf(" %016lx", buf[i]);
   }
}
//formatted print
#define CASE(var) case decltype(var)::nodeType(): var.printf(); break
void clef::astNode::printf() const {
   switch (_nodeType) {
      CASE(_identifier);
      CASE(_variable);
      CASE(_function);
      CASE(_type);
      CASE(_variadicParameter);
      CASE(_fundamentalType);
      CASE(_functionSignature);
      CASE(_enum);
      CASE(_mask);
      CASE(_union);
      CASE(_namespace);
      CASE(_interface);
      CASE(_struct);
      CASE(_class);
      CASE(_genericType);
      CASE(_scope);
      CASE(_literal);
      CASE(_expression);
      CASE(_declaration);
      CASE(_forLoop);
      CASE(_foreachLoop);
      CASE(_whileLoop);
      CASE(_doWhileLoop);
      CASE(_if);
      CASE(_switch);
      CASE(_match);
      CASE(_tryCatch);
      CASE(_asm);
      CASE(_forLoopParams);
      CASE(_foreachLoopParams);
      CASE(_switchCases);
      CASE(_matchCases);
      CASE(_statement);
      CASE(_statementSequence);
      CASE(_argumentList);
      CASE(_parameterList);

      case NodeType::NONE: std::printf("NULL NODE");
      case NodeType::ERROR: std::printf("ERORR NODE");
   }
}
#undef CASE

#endif //AST_NODE_CPP