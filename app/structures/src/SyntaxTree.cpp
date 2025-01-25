#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include <cstdio>

void clef::SyntaxTree::print() const {
   std::printf("ROOT: index = %ld, addr = %p", _buf.begin() - _root, (void*)_root);
   for (uint i = 0; i < _buf.size(); ++i) {
      std::printf("\n\t");
      _buf[i].print();
   }
   std::printf("\n");
}

void clef::SyntaxTree::printf() const {
   std::printf("HEY YOU! PROGRAMMING THIS! MAKE THIS ACTUALLY FORMATTED!\n");
   print();
}

#endif //SYNTAX_TREE_CPP