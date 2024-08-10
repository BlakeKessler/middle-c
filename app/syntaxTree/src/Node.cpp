#ifndef AST_NODE_CPP
#define AST_NODE_CPP

#include "Node.hpp"

clef::Node::Node(const TokenID_t tok, const NodeType t):
   nextID(NODE_NIL),
   prevID(NODE_NIL),
   tokenID(tok),
   indexInParent(0),
   type(t),
   status(0),
   parentID(NODE_NIL) ,
   childIDs() {
      
}
clef::Node::Node(const NodeID_t previous, const NodeType t, const TokenID_t tok):
   nextID(NODE_NIL),
   prevID(previous),
   tokenID(tok),
   indexInParent(0),
   type(t),
   status(0),
   parentID(NODE_NIL) ,
   childIDs() {
      
}
clef::Node::Node(const NodeID_t next, const NodeID_t previous, const NodeType t, const TokenID_t tok):
   nextID(next),
   prevID(previous),
   tokenID(tok),
   indexInParent(0),
   type(t),
   status(0),
   parentID(NODE_NIL) ,
   childIDs() {
      
}
clef::Node::Node(NodeID_t next, NodeID_t prev, TokenID_t tok, byte IIP, NodeType t, unsigned short s, NodeID_t parent, mcs::static_arr<NodeID_t,MAX_AST_CHILDREN> children):
   nextID(next),
   prevID(prev),
   tokenID(tok),
   indexInParent(IIP),
   type(t),
   status(s),
   parentID(parent) ,
   childIDs(children) {
      
}

#endif //AST_NODE_CPP