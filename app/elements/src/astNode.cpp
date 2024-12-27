#ifndef AST_NODE_CPP
#define AST_NODE_CPP

#include "astNode.hpp"

template<typename T> clef::index<T> clef::astNode::allocBuf(mcsl::is_t<T> auto... initList) {
   return astNode::_CURR_ALLOC->emplaceBuf(initList...);
}

#endif //AST_NODE_CPP