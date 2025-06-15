#ifndef SYMBOL_NODE_CPP
#define SYMBOL_NODE_CPP

#include "SymbolNode.hpp"

clef::SymbolNode::operator bool() const {
   return _decl || _def || _aliases.size();
}

#endif //SYMBOL_NODE_CPP