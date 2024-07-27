#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"
#include "dyn_arr.hpp"

//!public function to run a single pass of the parser
//!returns whether or not changes were made
bool clef::Parser::runPass() {
   if (!_tree->getRoot()) {
      return false;
   }
   
   bool madeChanges = false;
   madeChanges |= handleDelimPairs();

   return madeChanges;
}

#endif //PARSER_CPP