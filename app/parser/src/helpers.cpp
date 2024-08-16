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
      close->nextID,                               //next
      open->prevID,                                //prev
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

//!make a plaintext block
clef::astIt clef::Parser::makePtxtSeg(astIt& open, astIt& close) {
   //assert that open and close are delims
   assert(open->type == NodeType::PTXT_DELIM);
   assert(close->type == NodeType::PTXT_DELIM);
   //construct new node
   astIt block{&_tree, _tree.emplaceNode(
      close->nextID,                               //next
      open->prevID,                                //prev
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

//!make a statement
clef::astIt clef::Parser::makeStatement(astIt& open, astIt& close) {
   //assert that EOS is an EOS
   assert(close->type == NodeType::EOS);
   //construct new node
   astIt block{&_tree, _tree.emplaceNode(
      close->nextID,                               //next
      open->prevID,                                //prev
      TOK_NIL,                                     //token
      open->indexInParent,                         //index in parent
      NodeType::STATEMENT,                         //type
      0,                                           //status
      open->parentID,                              //parent
      mcs::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         NODE_NIL,                                    //block contents
         close                                        //closing delim
      }
   )};
   block.next()->prevID = +block->nextID ? block.index() : NODE_NIL;
   block.prev()->nextID = +block->prevID ? block.index() : NODE_NIL;
   //unlink contents
   close.prev()->nextID = NODE_NIL;
   close.next()->prevID = NODE_NIL;
   close->prevID = NODE_NIL;
   close->nextID = NODE_NIL;

   //other adjustments
   block.propegate();

   //return
   return block;
}

//!make an operator
clef::astIt clef::Parser::makeOpNode(astIt& op) {
   //assert that passed node is an operator node with no children
   assert(op->type == NodeType::OPERATOR || op->type == NodeType::OP_OR_DELIM);
   for (NodeID_t i : op->childIDs) {
      assert(!i);
   }

   //setup
   const Operator* data = getOpData(op.token(), +op->prevID);
   op->status = data->precedence;

   //get operands
   astIt lhs = op.prev();
   astIt rhs = op.next();
   astIt root = op;

   //handle lhs
   if (+(data->opType & OpType::LEFT_BIN)) {
      //merge into operator tree
      if (lhs->type == NodeType::OPERATOR && data->precedence <= lhs->status) {
         root = lhs;
         do {
            //go to child
            lhs >>= 1;
         } while (lhs->type == NodeType::OPERATOR && data->precedence <= lhs->status);
      }
      //no operator tree to merge into
      else {
         //re-link root
         root.setPrev(lhs->prevID);
      }

      //parent
      if (+lhs->parentID) {
         lhs.parent().setChild(op, lhs->indexInParent);
      }

      //move lhs
      op.setChild(lhs,0);
      lhs->nextID = NODE_NIL;
      lhs->prevID = NODE_NIL;
   }
   //handle rhs
   if (+(data->opType & OpType::RIGHT_BIN)) {
      //merge into rhs operator tree
      while (rhs->type == NodeType::OPERATOR && data->precedence <= rhs->status) {
         //go to child
         rhs >>= 0;
      }

      //move rhs
      op.setChild(rhs,1);
      op.setNext(rhs->nextID);
      root.setNext(rhs->nextID);

      //unlink rhs
      rhs->nextID = NODE_NIL;
      rhs->prevID = NODE_NIL;
   }

   //return
   return op;
}
#endif //PARSER_HELPERS_CPP