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

   // madeChanges |= escape();
   // madeChanges |= removePtxt();
   madeChanges |= handleBlockDelims();

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
bool clef::Parser::escape() {
   astIt current = _tree.root();
   if (!current) {
      return false;
   }

   bool madeChanges = false;

   do {
      if (current->type == NodeType::ESCAPE) {
         Token* tok = &_tree.token(current->tokenID);
         Token* nextTok = &_tree.token(current.next()->tokenID);
         assert(tok->end() == nextTok->begin() && nextTok->size());

         madeChanges = true;

         //next token is numeric
         if (+(nextTok->typeNum() & TokenType::NUM)) {
            tok->inc_end(nextTok->size());
            nextTok->inc_begin(nextTok->size());
            current->status = 0;
         }
         //next token is not numeric
         else {
            tok->inc_end(1);
            nextTok->inc_begin(1);
            current->status = (*tok)[1];
         }
         current->type = NodeType::LITERAL;

         if (!nextTok->size()) {
            current.next().pop();
         }
      }
   } while(++current);

   return madeChanges;
}
//!handle block delimiter pairs
//!returns whether or not changes were made
bool clef::Parser::handleBlockDelims() {
   astIt current = _tree.root();
   if (!current) {
      return false;
   }

   mcs::dyn_arr<astIt> delims{};
   do {
      //skip if not block delimiter
      if (current->type != NodeType::DELIM_GEN) {
         continue;
      }

      //get delimiter pair data
      const DelimPair* delimData = current.token().getDelimPairData();
      //verify block delimiter validity
      if (!delimData) { 
         clef::throwError(ErrCode::PARSER_UNSPEC, current.token().lineNum(), "Node %u marked as a block delimiter does not match any block delimiters",current.index());
         continue;
      }

      //closing delimiter
      if (delims.size() && current == delimData->close && delims.back() == delimData->open) {
         current = makeBlock(delims.back(),current);
         delims.pop_back();
         continue;
      }
      //opening delimiter
      if (current == delimData->open) {
         delims.push_back(current);
         continue;
      }

      //not opening or closing (invalid state)
      clef::throwError(ErrCode::PARSER_UNSPEC, current.token().lineNum(),"Node %u does not match either opening or closing delim");
   } while(++current);

   //check that all delims were consumed
   if (delims.size()) {
      throwError(ErrCode::UNCLOSED_BLOCK, "%u excess delimiters", delims.size());
   }

   return true;
}
#pragma endregion parsing

#endif //PARSER_CPP