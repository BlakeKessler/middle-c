#ifndef NODE_IO_CPP
#define NODE_IO_CPP

#include "Node.hpp"
#include "Token.hpp"

//!recursive formatted print of AST node
void clef::Node::printf(const uint indents) const {
   //print self
   for (uint i = indents; i; --i) {
      std::printf("   ");
   }
   std::printf("%u: ",+_type);
   if (_token) {
      _token->printf();
   }
   std::printf("\n");

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
      clef::throwError(code, _token->lineNum(), "nodeType: %u, token: %.*s", _type, _token->size(), _token->begin());
   }
   else {
      clef::throwError(code, "nodeType: %u", _type);
   }
}

#endif