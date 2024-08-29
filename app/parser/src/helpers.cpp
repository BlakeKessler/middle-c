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
      mcsl::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         (open->nextID == close.index() ? NODE_NIL : open->nextID),//block contents
         close                                        //closing delim
      }
   )};
   //set open and close types
   open->type = NodeType::DELIM_OPEN;
   close->type = NodeType::DELIM_CLOSE;
   //unlink contents
   open.sever();
   close.sever();

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
      mcsl::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         (open->nextID == close.index() ? NODE_NIL : open->nextID),//block contents
         close                                        //closing delim
      }
   )};
   //unlink contents
   open.sever();
   close.sever();

   //other adjustments
   block.propegate();

   //return
   return block;
}

// //!make a function
// clef::astIt clef::Parser::makeFunction(astIt& iden, astIt& operands) {
//    //assert that iden is an identifier without operands and operands is a parens block
//    assert(iden->type == NodeType::IDEN);
//    assert(iden->childIDs[1] == NODE_NIL);
//    assert(operands->type == NodeType::DELIM_PAIR);
//    assert(operands->status == +DelimPairType::PARENS);

//    //adjust tree
//    iden.setChild(operands.pop(),1);
   
//    return iden;
   
// }

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
      mcsl::static_arr<NodeID_t,MAX_AST_CHILDREN>{  //children
         open,                                        //opening delim
         NODE_NIL,                                    //block contents
         close                                        //closing delim
      }
   )};
   //unlink contents
   close.sever();
   open.severPrev();

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
   if (!data) {
      throwError(ErrCode::PARSER_UNSPEC, "WARNING: operator %.*s not found in operator data",op.token().size(),op.token().begin());
      return op;
   }
   if (+data->opType & +SPEC) { std::printf("NOTE: special operator being skipped\n"); return op; }

   //operands
   astIt lhs{&_tree, +data->opType & +LEFT_BIN  ? op->prevID : NODE_NIL};
   astIt rhs{&_tree, +data->opType & +RIGHT_BIN ? op->nextID : NODE_NIL};
   
   //misc
   astIt root{&_tree, (lhs->type == NodeType::OPERATOR && lhs->status < data->precedence && lhs[1]) ? lhs.index() : op.index()};
   NodeID_t prev = +data->opType & +LEFT_BIN  ? lhs->prevID : NODE_NIL;
   NodeID_t next = +data->opType & +RIGHT_BIN ? rhs->nextID : NODE_NIL;
   astIt parent = lhs.parent();
   byte iip = lhs->indexInParent;

   //go down lhs op tree
   //!NOTE: make precedence comparison determine between < and <= based on operator associativity
   while (lhs->type == NodeType::OPERATOR && lhs->status < data->precedence && lhs[1]) {
      lhs >>= 1;
   }
   //go down rhs op tree
   //!NOTE: make precedence comparison determine between < and <= based on operator associativity
   while (rhs->type == NodeType::OPERATOR && rhs->status < data->precedence && rhs[0]) {
      rhs >>= 0;
   }

   //unlink operands
   lhs.sever();
   rhs.sever();

   //place operator and operands
   lhs.parent().setChild(op,lhs->indexInParent);
   op.setChild(lhs, 0);
   op.setChild(rhs, 1);

   //adjust tree around root
   root.setPrev(prev);
   root.setNext(next);
   parent.setChild(root, iip);

   return root;
}
#endif //PARSER_HELPERS_CPP