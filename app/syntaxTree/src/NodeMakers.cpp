#ifndef AST_NODE_MAKERS_CPP
#define AST_NODE_MAKERS_CPP

#include "Node.hpp"
#include "Token.hpp"
#include <algorithm>

#pragma region constructors
//!default constructor
//!zero-initializes everything
clef::Node::Node() {
   _type = NodeType::NONE;
   _status = 0;

   _token = nullptr;
   _next = nullptr;
   _prev = nullptr;
   _parent = nullptr;
   _indexInParent = 0;

   std::memset(_children.begin(),0,_children.size());
}
//!construct from token and type
clef::Node::Node(Token* tok, const NodeType type) {
   _type = +type ? type : tok ? tok->nodeType() : NodeType::NONE;
   _status = 0;

   _token = tok;
   _next = nullptr;
   _prev = nullptr;
   _parent = nullptr;
   _indexInParent = 0;

   std::memset(_children.begin(),0,_children.size());
}
//!construct from previous and type
//!sets previous's next to be this
clef::Node::Node(Node* previous, const NodeType type, Token* tok) {
   _type = +type ? type : tok ? tok->nodeType() : NodeType::NONE;
   _status = 0;

   _token = tok;
   _next = nullptr;
   _prev = previous;
   if (previous) {
      delete previous->setNext(this);
   }
   _parent = nullptr;
   _indexInParent = 0;

   std::memset(_children.begin(),0,_children.size());
}
#pragma endregion constructors

#pragma region makers
//!maker function for block nodes
//!_children[0] -> opening delimiter
//!_children[1] -> first token of contents of block
//!_children[2] -> closing delimiter 
clef::Node* clef::Node::makeBlock(Node* open, Node* close, const DelimPairType type) {
   //check for nullptr params
   if (!open || !close) {
      return nullptr;
   }
   //construct node
   Node* block = new Node();
   Node* prev = open->_prev;

   //set up parent
   if (open->parent()) {
      open->parent()->setChild(open->_indexInParent, block);
   }
   

   //set up children
   delete block->setChild(0, Node::removeSegment(open, close));            //opening delim
   if (open->_next != close) {                                             //contents
      delete block->setChild(1, Node::removeSegment(open->_next, close));
   }
   delete block->setChild(2, Node::removeSegment(close, close));           //closing delim

   //add to AST
   prev ? prev->insertAfter(block) : nullptr;

   //set types and statuses
   block->_type = NodeType::DELIM_PAIR;
   open->_type = NodeType::DELIM_OPEN;
   close->_type = NodeType::DELIM_CLOSE;

   block->_status = +type;
   open->_status = +type;
   close->_status = +type;

   //return new block node
   return block;
}

//!maker function for block nodes
//!_children[0] -> opening delimiter
//!_children[1] -> first token of contents of block
//!_children[2] -> closing delimiter 
clef::Node* clef::Node::makeBlock(Node* open, Node* close) {
   assert(open && close);
   return makeBlock(open, close, open->blockDelimEval());
}

//!maker function for statement nodes
//!_children[0] -> beginning of statement
//!_children[1] -> end of statement delim
clef::Node* clef::Node::makeStatement(Node* firstTok, Node* eosTok) {
   if (!eosTok || !eosTok->_prev) {
      return nullptr;
   }
   //find first token in statement
   //!NOTE: make more efficient
   Node* stmntStart = eosTok;
   while (stmntStart->_prev) {
      if (  stmntStart->_prev->type() == NodeType::STATEMENT ||
            stmntStart->_prev->type() == NodeType::EOS ||
            stmntStart->_prev->type() == NodeType::DELIM_CLOSE ||
            stmntStart->_prev->type() == NodeType::DELIM_OPEN ||
            stmntStart->_prev->type() == NodeType::ERROR ||
            stmntStart->_prev->type() == NodeType::NONE) {
         break;
      }
      stmntStart = stmntStart->_prev;
   }


   //construct statement node
   Node* statement = new Node();
   statement->_type = NodeType::STATEMENT;
   statement->_status = eosTok - firstTok;

   //handle before and after the statement
   Node* before = stmntStart->_prev;
   Node* after = eosTok->_next;
   if (before) {
      before->_next = statement;
      statement->_prev = before;
      stmntStart->_prev = nullptr;
   }
   if (after) {
      after->_prev = before;
      statement->_next = after;
      eosTok->_next = nullptr;
   }

   //set up children
   delete statement->setChild(0, stmntStart);
   delete statement->setChild(1, eosTok);
   
   //disconnect EOS
   eosTok->_prev->_next = nullptr;
   eosTok->_prev = nullptr;

   //return
   return statement;
}

#pragma endregion makers

#endif //AST_NODE_MAKERS_CPP