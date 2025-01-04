#ifndef AST_NODE_CPP
#define AST_NODE_CPP

#include "astNode.hpp"

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

#endif //AST_NODE_CPP