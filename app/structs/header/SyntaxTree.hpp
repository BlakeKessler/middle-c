#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "NameTable.hpp"
#include "astNode.hpp"

struct clef::SyntaxTree {
   private:
      NameTable _names;
      astNode* _root;
   public:
};

#endif //SYNTAX_TREE_HPP