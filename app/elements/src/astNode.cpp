#ifndef AST_NODE_CPP
#define AST_NODE_CPP

#include "astNode.hpp"
#include "pretty-print.hpp"

#include <cstring>
#include "math.hpp"

#include "MAP_MACRO.h"

//derived to base
void clef::astNode::downCast(NodeType newType) {
   if (canDownCastTo(_nodeType, newType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, mcsl::FMT("attempt to downcast from %s to %s"), toString(_nodeType), toString(newType));
   }
}
//base to derived
void clef::astNode::upCast(NodeType newType) {
   if (canDownCastTo(newType, _nodeType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, mcsl::FMT("attempt to upcast from %s to %s"), toString(_nodeType), toString(newType));
   }
}
//derived to base or base to derived
void clef::astNode::verticalCast(NodeType newType) {
   if (canDownCastTo(_nodeType, newType) || canDownCastTo(newType, _nodeType)) {
      _nodeType = newType;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, mcsl::FMT("attempt to vertical cast from %s to %s"), toString(_nodeType), toString(newType));
   }
}
//cast without type checking
void clef::astNode::anyCast(NodeType newType) {
   _nodeType = newType;
}


//sizeof node from node type
#define typecase(type) case type::nodeType(): return sizeof(type);
uint clef::nodeSizeof(NodeType t) {
   switch (t) {
      case NodeType::NONE: return 0;
      case NodeType::ERROR: return 0;

      MCSL_MAP(typecase, ALL_AST_NODE_T)
   }
}
#undef typecase

//unformatted print
void clef::astNode::print() const {
   const uint byteCount = nodeSizeof(_nodeType);
   const uint intCount = (uint)mcsl::ceil(((float)byteCount) / sizeof(ulong));
   ulong buf[sizeof(astNode) / sizeof(ulong) + (sizeof(astNode) % sizeof(ulong) != 0)]{};
   std::memcpy(buf, this, byteCount);

   mcsl::printf(mcsl::FMT("%u:"), +_nodeType);
   for (uint i = 0; i < intCount; ++i) {
      mcsl::printf(mcsl::FMT(" %016x"), buf[i]);
   }
}
//formatted print
#define CASE(var) case decltype(var)::nodeType(): var.printf(); break;
void clef::astNode::printf() const {
   switch (_nodeType) {
      case NodeType::NONE: mcsl::printf(mcsl::FMT("NULL NODE")); break;
      case NodeType::ERROR: mcsl::printf(mcsl::FMT("ERORR NODE")); break;
      
      MCSL_MAP(CASE, CLEF_ALL_AST_NODE_UNION_MEMBS)
   }
}
#undef CASE

#include "MAP_MACRO_UNDEF.h"

#endif //AST_NODE_CPP