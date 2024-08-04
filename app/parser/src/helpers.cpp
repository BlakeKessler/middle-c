#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"
#include <cstring>

// //!NOTE: REWRITE (this and MidC_Data.hpp)
// bool clef::Parser::isOpeningDelim(astIt& current) {
//    auto& tok = current.token();
//    if (!tok.size()) { return false; }
//    return !std::strncmp(tok.begin(), BLOCK_DELIMS[+blockDelimType(tok)].open.begin(), MAX_DELIM_LEN);
// }
// //
// bool clef::Parser::delimsMatch(astIt& open, astIt& close) {
//    if (!open->tokenID || !close->tokenID) { return false; }
//    auto t = BLOCK_DELIMS[+blockDelimType(open.token())];
//    return !(!std::strncmp(open.token().begin(), t.open.begin(), open.token().size()) || !std::strncmp(close.token().begin(), t.close.begin(), close.token().size()));
// }

//!make a block delimiter pair
clef::astIt clef::Parser::makeBlock(astIt& open, astIt& close) {
   //assert that open and close are delims
   assert(open->type == NodeType::DELIM_GEN);
   assert(close->type == NodeType::DELIM_GEN);
   //set open and close types
   open->type = NodeType::DELIM_OPEN;
   close->type = NodeType::DELIM_CLOSE;
   //construct new node
   // astIt block{&_tree, _tree.pushNode(Node{0, NodeType::DELIM_PAIR})};
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
   open->prevID = NODE_NIL;
   open.severNext();
   close.severPrev();
   close->nextID = NODE_NIL;
   
   //return
   return block;
}

#endif //PARSER_HELPERS_CPP