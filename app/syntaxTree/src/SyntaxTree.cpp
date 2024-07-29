#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"
#include "dyn_arr.hpp"

//constructor
//!NOTE: rewrite to use a dyn_arr as a stack to ensure contiguitiy
clef::SyntaxTree::SyntaxTree(Token* tokens, const uint count)
:_root(new Node()){
   Node* current = _root;
   const Token* const end = tokens + count;
   //push tokens
   for (Token* it = tokens ; it < end; ++it) {
      current = new Node(current, NodeType::NONE, it);
   }
}
//destructor - frees all nodes in the tree
clef::SyntaxTree::~SyntaxTree() {
   mcs::dyn_arr<Node*> stack{};
   Node* scout = _root;
   Node* backup;
   while (scout) {
      backup = scout->next();
      //push children to stack
      for (uint i = 0; i < MAX_AST_CHILDREN; ++i) {
         if (scout->child(i)) {
            stack.push_back(scout->child(i));
         }
      }
      //free scout
      delete scout;
      scout = backup;

      //pop from stack
      if (!scout && stack.size()) {
         scout = stack.pop_back();
      }
   }
}

#endif //SYNTAX_TREE_CPP