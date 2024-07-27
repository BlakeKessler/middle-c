#pragma once
#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "Node.hpp"
#include "Token.hpp"

class clef::SyntaxTree {
   private:
      Node* _root;
   public:
      SyntaxTree(Token* tokens, const uint count);
      ~SyntaxTree();

      constexpr Node* getRoot() const {return _root ? (_root->next() ? _root->next() : _root) : nullptr;}
      void printf() const { getRoot()->printf(0); }
};

#endif //SYNTAX_TREE_HPP