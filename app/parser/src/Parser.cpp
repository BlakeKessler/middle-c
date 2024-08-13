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

   madeChanges |= escape();
   madeChanges |= removePtxt();
   madeChanges |= handleBlockDelims();
   madeChanges |= handleStatements();

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
            current->status = +TokenType::NUM;
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

   updateRoot();
   return madeChanges;
}

//!handle plaintext segments
//!returns whether or not changes were made
bool clef::Parser::removePtxt() {
   astIt current = _tree.root();
   if (!current) {
      return false;
   }

   const DelimPair* data = nullptr;
   astIt open;
   do {
      //skip if not block delimiter
      if (current->type != NodeType::PTXT_DELIM) {
         continue;
      }

      //closing delim
      if (data) {
         if (current == data->close) {
            current = makePtxtSeg(open,current);
            data = nullptr;
         }
         continue;
      }

      //opening delim
      open = current;
      data = current.token().getPtxtData();
      //verify validity
      if (!data) { 
         clef::throwError(ErrCode::PARSER_UNSPEC, current.token().lineNum(), "Node %u marked as a plaintext delimiter does not match any plaintext delimiters",current.index());
         continue;
      }
   } while(++current);

   //check that all delims were consumed
   if (data) {
      throwError(ErrCode::UNCLOSED_BLOCK, "plaintext delimiter %.*s %.*s",data->open.size(),data->open.begin(),data->close.size(),data->close.begin());
   }

   updateRoot();
   return true;
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

   updateRoot();
   return true;
}

//!handle statements
//!returns whether or not changes were made
bool clef::Parser::handleStatements() {
   bool madeChanges = false;

   astIt current = _tree.root();
   astIt statementStart = current;
   mcs::dyn_arr<NodeID_t> stack{};

   while (current) {
      //push children
      for (NodeID_t i : current->childIDs) {
         if (+i) {
            stack.push_back(i);
         }
      }
      //check for EOS
      if (current->type == NodeType::EOS) {
         current = makeStatement(statementStart, current);
         statementStart.setIndex(current->nextID);
         madeChanges = true;
      }
      //iterate
      if (!++current && stack.size()) {
         current.setIndex(stack.pop_back());
         statementStart = current;
      }
   }

   updateRoot();
   return madeChanges;
}
#pragma endregion parsing

#endif //PARSER_CPP