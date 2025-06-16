#ifndef SYMBOL_NODE_CPP
#define SYMBOL_NODE_CPP

#include "SymbolNode.hpp"

clef::SymbolNode::operator bool() const {
   static_assert(sizeof(_asBytes) == sizeof(uoverlong));
   return _aliases.size() || *((uoverlong*)_asBytes);
}

#endif //SYMBOL_NODE_CPP