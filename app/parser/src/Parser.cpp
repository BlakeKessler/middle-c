#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

clef::Parser::Parser(mcs::dyn_arr<Token>&& tokens, mcs::array<mcs::dyn_arr_span<Token>>& lines):
   _tree(std::move(tokens),lines) {

}

//!public function to run a single pass of the parser
//!returns whether or not changes were made
bool clef::Parser::runPass() {
   bool madeChanges = false;
   
   assert(!_tree.it(NODE_NIL)->nextID && !_tree.it(NODE_NIL)->prevID && !_tree.it(NODE_NIL)->parentID);
   // madeChanges |= removePtxt();
   // assert(!_tree.it(NODE_NIL)->nextID && !_tree.it(NODE_NIL)->prevID && !_tree.it(NODE_NIL)->parentID);
   madeChanges |= handleDelimPairs();
   assert(!_tree.it(NODE_NIL)->nextID && !_tree.it(NODE_NIL)->prevID && !_tree.it(NODE_NIL)->parentID);

   return madeChanges;
}

//!public function to run multiple passes of the parser
//!returns whether or not changes were made on the final pass
bool clef::Parser::runPasses(const uint passes) {
   for (uint i = passes; i; --i) {
      if (!runPass()) {
         return false;
      }
   }
   return true;
}


#pragma region parsing
//!handle escape characters
//!returns whether or not changes were made
bool clef::Parser::handleDelimPairs() {
   astIt current = _tree.root();
   if (!current) {
      return false;
   }

   mcs::dyn_arr<astIt> ptxtDelims{};
   do {
      switch (current->type) {
         case NodeType::ESCAPE:
         //    escape(current);
            break;
         case NodeType::DELIM_GEN:{
            const DelimPair* delimData = current.token().getDelimPairData();
            if (!delimData) {
               clef::throwError(ErrCode::PARSER_UNSPEC, current.token().lineNum(), "Node %u marked as a block delimiter does not match any block delimiters",current.index());
               continue;
            }
            if (current == delimData->close) {
               if (ptxtDelims.size() && ptxtDelims.back() == delimData->open) {
                  current = makeBlock(ptxtDelims.back(),current);
                  ptxtDelims.pop_back();
               }
            } else if (current == delimData->open) {
               ptxtDelims.push_back(current);
            }
            break;
         }
         default:
            break;
      }

   } while(++current);

   if (ptxtDelims.size()) {
      // for (auto& i : ptxtDelims) {
      //    i.printf(); std::printf("\n");
      // }
      throwError(ErrCode::UNCLOSED_BLOCK, "%u excess delimiters", ptxtDelims.size());
   }

   return true;
}
#pragma endregion parsing

#endif //PARSER_CPP