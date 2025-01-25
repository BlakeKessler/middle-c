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
uint clef::nodeSizeof(NodeType t) {
   switch (t) {
      case NodeType::NONE: return 0;
      case NodeType::ERROR: return 0;
      case NodeType::OPERATOR: UNREACHABLE;

      case Keyword::nodeType(): return sizeof(Keyword);
      case Identifier::nodeType(): return sizeof(Identifier);
      case Variable::nodeType(): return sizeof(Variable);
      case Function::nodeType(): return sizeof(Function);
      case Type::nodeType(): return sizeof(Type);
      case VariadicParameter::nodeType(): return sizeof(VariadicParameter);
      case FundamentalType::nodeType(): return sizeof(FundamentalType);
      case FunctionSignature::nodeType(): return sizeof(FunctionSignature);
      case Enum::nodeType(): return sizeof(Enum);
      case Mask::nodeType(): return sizeof(Mask);
      case Union::nodeType(): return sizeof(Union);
      case Namespace::nodeType(): return sizeof(Namespace);
      case Interface::nodeType(): return sizeof(Interface);
      case Struct::nodeType(): return sizeof(Struct);
      case Class::nodeType(): return sizeof(Class);
      case GenericType::nodeType(): return sizeof(GenericType);
      case Scope::nodeType(): return sizeof(Scope);
      case Literal::nodeType(): return sizeof(Literal);
      case Expression::nodeType(): return sizeof(Expression);
      case Declaration::nodeType(): return sizeof(Declaration);
      case Loop::nodeType(): return sizeof(Loop);
      case If::nodeType(): return sizeof(If);
      case Switch::nodeType(): return sizeof(Switch);
      case Match::nodeType(): return sizeof(Match);
      case TryCatch::nodeType(): return sizeof(TryCatch);
      case Asm::nodeType(): return sizeof(Asm);
      case ForLoopParams::nodeType(): return sizeof(ForLoopParams);
      case ForeachLoopParams::nodeType(): return sizeof(ForeachLoopParams);
      case SwitchCases::nodeType(): return sizeof(SwitchCases);
      case MatchCases::nodeType(): return sizeof(MatchCases);
      case Statement::nodeType(): return sizeof(Statement);
      case StatementSequence::nodeType(): return sizeof(StatementSequence);
      case ArgumentList::nodeType(): return sizeof(ArgumentList);
      case ParameterList::nodeType(): return sizeof(ParameterList);
   }
}

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

#endif //AST_NODE_CPP