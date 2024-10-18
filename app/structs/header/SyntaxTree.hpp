#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "SymbolTable.hpp"
#include "astNode.hpp"

struct clef::SyntaxTree {
   private:
      SymbolTable _names;
      astNode* _root;
   public:
};

#endif //SYNTAX_TREE_HPP