#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"
#include <cstring>

//!make a block delimiter pair
clef::astIt clef::Parser::makeBlock(astIt& open, astIt& close) {
   //assert that open and close are delims
   assert(open->type == NodeType::DELIM_GEN);
   assert(close->type == NodeType::DELIM_GEN);
   //construct new node
   astIt block{&_tree, _tree.emplaceNode(
      close.next(),                                //next
      open.prev(),                                 //prev
      TOK_NIL,                                     //token
      open->indexInParent,                         //index in parent
      NodeType::DELIM_PAIR,                        //type
      +blockDelimType(open.token()),               //status
      open->parentID,                              //parent
      mcs::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         (open->nextID == close.index() ? NODE_NIL : open->nextID),//block contents
         close                                        //closing delim
      }
   )};
   //set open and close types
   open->type = NodeType::DELIM_OPEN;
   close->type = NodeType::DELIM_CLOSE;
   //unlink contents
   open.prev()->nextID = NODE_NIL;
   open.next()->prevID = NODE_NIL;
   close.prev()->nextID = NODE_NIL;
   close.next()->prevID = NODE_NIL;
   
   open->nextID = NODE_NIL;
   open->prevID = NODE_NIL;
   close->nextID = NODE_NIL;
   close->prevID = NODE_NIL;

   //other adjustments
   block.propegate();

   //return
   return block;
}

//!make a block delimiter pair
clef::astIt clef::Parser::makePtxtSeg(astIt& open, astIt& close) {
   //assert that open and close are delims
   assert(open->type == NodeType::PTXT_DELIM);
   assert(close->type == NodeType::PTXT_DELIM);
   //construct new node
   astIt block{&_tree, _tree.emplaceNode(
      close.next(),                                //next
      open.prev(),                                 //prev
      TOK_NIL,                                     //token
      open->indexInParent,                         //index in parent
      NodeType::PTXT_SEG,                          //type
      +blockDelimType(open.token()),               //status
      open->parentID,                              //parent
      mcs::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         (open->nextID == close.index() ? NODE_NIL : open->nextID),//block contents
         close                                        //closing delim
      }
   )};
   //unlink contents
   open.prev()->nextID = NODE_NIL;
   open.next()->prevID = NODE_NIL;
   close.prev()->nextID = NODE_NIL;
   close.next()->prevID = NODE_NIL;
   
   open->nextID = NODE_NIL;
   open->prevID = NODE_NIL;
   close->nextID = NODE_NIL;
   close->prevID = NODE_NIL;

   //other adjustments
   block.propegate();

   //return
   return block;
}

#endif //PARSER_HELPERS_CPP