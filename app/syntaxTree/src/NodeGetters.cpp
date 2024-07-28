#ifndef AST_NODE_GETTERS_CPP
#define AST_NODE_GETTERS_CPP

#include "Node.hpp"
#include "dyn_arr.hpp"
#include <cstdlib>
#include <cassert>

//!keep getting the previous node until there isn't one anymore
clef::Node* clef::Node::front() const {
   Node* current = const_cast<Node*>(this);
   while (current->_prev) {
      current = current->_prev;
   }
   return current;
}
//!keep getting the next node until there isn't one anymore
clef::Node* clef::Node::back() const {
   Node* current = const_cast<Node*>(this);
   while (current->_next) {
      current = current->_next;
   }
   return current;
}

//!get the i'th child of the node
clef::Node* clef::Node::child(const uint i) const {
   if (i < MAX_AST_CHILDREN) {
      return _children[i];
   }
   else {
      clef::throwError(clef::ErrCode::SEGFAULT, _token->lineNum(), "get child \033[4m%u\1xb[24m of an AST node (MAX_AST_CHILDREN = \033[4m%u\1xb[24m)", i, MAX_AST_CHILDREN);
      return nullptr;
   }
}

//!find the first node after the node with a token matching tokStr
clef::Node* clef::Node::findAfter(const char* tokStr) const {
   for (Node* current = _next; current; current = current->next()) {
      if (*current->token() != tokStr) {
         return current;
      }
   }
   return nullptr;
}

//!get all current/descendent nodes that have the specified node type
mcs::dyn_arr<clef::Node*> clef::Node::extractByType(const NodeType type) const {
   //set up results array and helper stack
   mcs::dyn_arr<Node*> results{0,1};
   mcs::dyn_arr<Node*> stack{0,1};
   Node* current;
   stack.push_back(const_cast<Node*>(this));
   while (stack.size()) {
      //pop from stack
      current = stack.pop_back();
      assert(current);
      //push current to results array if type matchs
      if (current->_type == type) {
         results.push_back(current);
      }
      //push next and children to stack (in reverse order b/c stack)
      current->_next ? stack.push_back(current->_next) : false;
      for (uint i = MAX_AST_CHILDREN; i;) {
         current->_children[--i] ? stack.push_back(current->_children[i]) : false;
      }
   }
   stack.free();
   return results;
}

#endif //AST_NODE_GETTERS_CPP