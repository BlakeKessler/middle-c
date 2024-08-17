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


/* the Sliding Operands algorithm:
   PSEUDOCODE:
      find leftmost node of lhs
      find leftmost node of rhs


      swap lhs and op
      move lhs to op[0]
      move rhs to op[1]

      recursively slide rhs ancestors onto the operator tree until a non-operator node is reached
         go to parent (P)
         find its location on the LHS tree (by precedence as always)
         move P to that location
         move the node that used to be at that location to P[0]
*/

   //setup
   astIt opRoot;
   astIt lhs;
   astIt rhs;

   const Operator* data = getOpData(op.token(), +op->prevID);
   assert(data);
   op->status = data->precedence;

   //handle lhs
   if (+data->opType & +OpType::LEFT_BIN) {
      lhs = op.prev();
      if (+lhs->parentID) { lhs.parent().setChild(op,lhs->indexInParent); }

      //handle simple case (non-operator operands)
      if (lhs->type != NodeType::OPERATOR || lhs->status > data->precedence) {
         opRoot = op;

         op.setPrev(lhs->prevID);
         lhs.setNext(NODE_NIL);
      }
      //slide op down the right side of lhs
      else {
         opRoot = lhs; 
         while (lhs->type == NodeType::OPERATOR && lhs->status < data->precedence) {
            lhs >>= 1;
         }
         assert(lhs);
         op.setNext(NODE_NIL);
         lhs.setChild(op,1);
      }
      op.setChild(lhs,0);
   } else { opRoot = op; }

   //handle rhs
   if (+data->opType & +OpType::RIGHT_BIN) {
      rhs = op.next();
      opRoot.setNext(rhs->nextID);
      //handle simple case (non-operator operands)
      if (rhs->type != NodeType::OPERATOR || rhs->status <= data->precedence) {
         op.setChild(rhs,1);
         rhs->nextID = NODE_NIL;
         rhs->prevID = NODE_NIL;
      }
      //slide rhs down the tree
      else {
         //handle initial slide
         NodeID_t temp;
         lhs = op;
         while (rhs->type == NodeType::OPERATOR && rhs->status > data->precedence) {
            rhs >>= 0;
         }
         temp = rhs->parentID;
         op.setChild(rhs,1);
         rhs.goTo(temp);

         //handle rest of slide
         while (true) {
            lhs = opRoot;
            while (lhs->type == NodeType::OPERATOR && lhs->status > rhs->status) {
               lhs >>= 1;
            }
            temp = lhs->parentID;
            rhs.setChild(lhs,0);
            lhs.goTo(temp);
            if (!lhs) {
               break;
            }
            temp = rhs->parentID;
            lhs.setChild(rhs,1);
            rhs.goTo(temp);
            
            while (rhs->type == NodeType::OPERATOR && lhs->status > rhs->status) {
               rhs.toParent();
            }
            if (rhs->type != NodeType::OPERATOR) { break; }
            // rhs->nextID = NODE_NIL;
            // rhs->prevID = NODE_NIL;
            temp = rhs->parentID;
            lhs.setChild(rhs, 1);
            rhs.goTo(temp);
         }
      }

      
   }
   return op;
}
#endif //PARSER_HELPERS_CPP