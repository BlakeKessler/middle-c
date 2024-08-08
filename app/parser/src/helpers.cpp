#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"
#include <cstring>

//!make a block delimiter pair
clef::astIt clef::Parser::makeBlock(astIt& open, astIt& close) {
   //assert that open and close are delims
   assert(open->type == NodeType::DELIM_GEN);
   assert(close->type == NodeType::DELIM_GEN);
   //set open and close types
   open->type = NodeType::DELIM_OPEN;
   close->type = NodeType::DELIM_CLOSE;
   //construct new node
   astIt block{&_tree, _tree.emplaceNode(TOK_NIL, NodeType::DELIM_PAIR)};
   //set status
   block->status = +blockDelimType(_tree.token(open->tokenID));
   open->status = block->status;
   close->status = block->status;
   assert(close->status == +blockDelimType(_tree.token(close->tokenID)));
   //set up children
   block.setChild(open,0);
   block.setChild(open->nextID,1);
   block.setChild(close,2);
   //set previous and next
   block.setPrev(open->prevID);
   block.setNext(close->nextID);
   //set parent
   open.parent().setChild(block, open->indexInParent);
   //disconnect open and close
   open->prevID = NODE_NIL;   //pointers to the block should already be handled
   open.severNext();
   close.severPrev();
   close->nextID = NODE_NIL;  //pointers to the block should already be handled
   
   //return
   return block;
}

#endif //PARSER_HELPERS_CPP