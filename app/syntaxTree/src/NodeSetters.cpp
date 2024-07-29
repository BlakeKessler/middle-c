#ifndef NODE_SETTERS_CPP
#define NODE_SETTERS_CPP

#include "Node.hpp"

//!replace next node
//!returns the node that used to be next (sets its _next and _prev to nullptr)
clef::Node* clef::Node::setNext(Node* newNext) {
   if (newNext) {
      newNext->_prev = this;

      newNext->_next = (_next ? _next->_next : nullptr);
      newNext->_next ? (newNext->_next->_prev = _next) : nullptr;
   }

   Node* temp = _next;
   _next = newNext;
   if (temp) {
      temp->_prev = nullptr;
      temp->_next = nullptr;
   }
   return temp;
}
//!replace previous node
//!returns the node that used to be previous (sets its _next and _prev to nullptr)
clef::Node* clef::Node::setPrev(Node* newPrev) {
   if (newPrev) {
      newPrev->_next = this;

      newPrev->_prev = (_prev ? _prev->_prev : nullptr);
      newPrev->_prev ? (newPrev->_prev->_next = _prev) : nullptr;
   }

   Node* temp = _prev;
   _prev = newPrev;
   if (temp) {
      temp->_next = nullptr;
      temp->_prev = nullptr;
   }
   return temp;
}
//!remove links between a node and its next node
//!returns the original next node
inline clef::Node* clef::Node::unlinkNext() {
   Node* orig = _next;
   _next ? (_next->_prev = nullptr) : nullptr;
   _next = nullptr;
   return orig;
}
//!remove links between a node and its previous node
//!returns the original next node
inline clef::Node* clef::Node::unlinkPrev() {
   Node* orig = _prev;
   _prev ? (_prev->_next = nullptr) : nullptr;
   _prev = nullptr;
   return orig;
}

//!replace a child node
//!returns the replaced child
clef::Node* clef::Node::setChild(const uint i, Node* newChild) {
   //bounds check
   if (i >= _children.size()) {
      if (_token) {
         clef::throwError(clef::ErrCode::SEGFAULT, _token->lineNum(), "set child \033[4m%u\1xb[24m of an AST node (MAX_AST_CHILDREN = \033[4m%u\1xb[24m)", i, _children.size());
      } else {
         clef::throwError(clef::ErrCode::SEGFAULT, "set child \033[4m%u\1xb[24m of an AST node (MAX_AST_CHILDREN = \033[4m%u\1xb[24m)", i, _children.size());
      }
      return nullptr;
   }
   //save replaced child
   Node* temp = _children[i];
   //insert child
   if (newChild) {
      newChild->_parent = this;
      newChild->_indexInParent = i;
   }
   _children[i] = newChild;
   //return replaced child
   return temp;
}
//!set token
//!returns the replaced token
clef::Token* clef::Node::setToken(Token* newToken) {
   Token* temp = _token;
   _token = newToken;
   return temp;
}
//!set node type
//!returns the replaced node type
clef::NodeType clef::Node::setType(const NodeType newType) {
   const NodeType temp = _type;
   _type = newType;
   return temp;
}
//!set node status
//!returns the replaced node status
uint clef::Node::setStatus(const uint newStatus) {
   const uint temp = _status;
   _status = newStatus;
   return temp;
}

//!push newNode after the current node
//!alters nodes to maintain consistancy
clef::Node* clef::Node::insertAfter(Node* newNode) {
   newNode->_next = _next;
   newNode->_prev = this;
   _next ? (_next->_prev = newNode) : 0;
   _next = newNode;

   return _next;
}
//!push newNode before the current node
//!alters nodes to maintain consistancy
clef::Node* clef::Node::insertBefore(Node* newNode) {
   newNode->_prev = _prev;
   newNode->_next = this;
   _prev ? (_prev->_next = newNode) : 0;
   _prev = newNode;

   return _prev;
}
//!remove the current node
clef::Node* clef::Node::pop() {
   _next ? (_next->_prev = _prev) : 0;
   _prev ? (_prev->_next = _next) : 0;
   _next = nullptr;
   _prev = nullptr;
   return this;
}

//!removes a sequence of nodes to a new list
clef::Node* clef::Node::removeSegment(Node* open, Node* close) {
   if (!open || !close) {
      return nullptr;
   }

   //set up forward link
   if (open->_prev) {
      open->_prev->_next = close->_next;
      open->_prev = nullptr;
   }
   //set up backward link
   if (close->_next) {
      close->_next->_prev = open->_prev;
      close->_next = nullptr;
   }

   //return new list
   return open;
}

#endif //NODE_SETTERS_CPP