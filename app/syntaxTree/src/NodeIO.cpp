#ifndef NODE_IO_CPP
#define NODE_IO_CPP

#include "Node.hpp"
#include "Token.hpp"
#include <iostream>

//!recursive formatted print of AST node
void clef::Node::printf(const uint indents) const {
   //print self
   for (uint i = indents; i; --i) {
      std::cout << "   ";
   }
   std::cout << (short)_type << ": " << _token << std::endl;

   //print children
   for (uint i = 0; i < MAX_AST_CHILDREN; ++i) {
      if (_children[i]) {
         _children[i]->printf(indents + 1);
      }
   }
   //print next
   if (_next) {
      _next->printf(indents);
   }
}

//!throw error for node
void clef::Node::throwError(const ErrCode code) const {
   if (_token) {
      clef::throwError(code, _token->lineNum(), "nodeType: %u, token: %.*s", _type, _token->size(), _token->data());
   }
   else {
      clef::throwError(code, "nodeType: %u", _type);
   }
}

#endif