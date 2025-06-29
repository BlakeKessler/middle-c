#ifndef AST_MERGE_CPP
#define AST_MERGE_CPP

#include "SyntaxTree.hpp"

#if PARALLEL_COMPILE_FILES
void clef::SyntaxTree::merge(SyntaxTree&& other) {
   for (SymbolNode& symbol : other._symbolBuf) {
      
      TODO;
   }
   
   TODO;
}
#endif

#endif //AST_MERGE_CPP