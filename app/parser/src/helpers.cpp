#ifndef PARSER_HELPERS_CPP
#define PARSER_HELPERS_CPP

#include "Parser.hpp"
#include "MidC_Data.hpp"

//!public function to run multiple passes of the parser
//!returns whether or not the final completed pass made any changes
bool clef::Parser::runPasses(const uint maxPasses) {
   bool isIncomplete = true;
   for (uint i = maxPasses; i && isIncomplete; --i) {
      isIncomplete &= runPass();
   }
   return isIncomplete;
}

//!handle block delimiters
//!NOTE: could be rewritten to not allocate duplicate memory for the stack and queue
bool clef::Parser::handleDelimPairs() {
   //allocate memory
   mcs::dyn_arr<Node*> allDelims(_tree->getRoot()->extractByType(NodeType::DELIM_GEN));
   mcs::dyn_arr<Node*> delimStack{0,1};
   bool madeChanges = false;

   //process
   for (uint i = 0; i < allDelims.size(); ++i) {
      //check for empty stack
      if (!delimStack.size()) {
         //ensure that first delim is opening
         if (allDelims[i]->isOpening()) {
            delimStack.push_back(allDelims[i]);
            continue;
         }
         else {
            allDelims[i]->throwError(ErrCode::UNCLOSED_BLOCK);
            return false;
         }
      }
      
      //check for matching delims
      if (delimStack.back()->checkDelimMatch(allDelims[i])) {
         madeChanges |= (bool)Node::makeBlock(delimStack.back(), allDelims[i]);
         delimStack.pop_back();
         continue;
      }

      //push if opening delim
      if (allDelims[i]->isOpening()) {
         delimStack.push_back(allDelims[i]);
         continue;
      }

      //illegal delim - throw error
      allDelims[i]->throwError(ErrCode::UNCLOSED_BLOCK);
      return false;
   }

   //check that all opening delims were closed
   if (delimStack.size()) {
      throwError(ErrCode::UNCLOSED_BLOCK, "\033[4m%u\033[24m unclosed delims", delimStack.size());
      return false;
   }

   //clean up and return
   return madeChanges;
}

#define caseMacro(delimType) case asint(DelimPairType::delimType):                           \
   if (iden && iden->type() == NodeType::IDEN) {                                             \
      current->pop();                                                                        \
      iden->child(0) ? iden->child(0)->back()->setNext(current) : iden->setChild(0, current);\
   }                                                                                         \
   return true;
//!parser function to evaluate a delimiter pair node
bool clef::Parser::evalDelimPair(Node* const current) {
   //check status (pair type)
   Node* iden = current->prev();
   switch (current->status()) {
      //char or str
      case asint(DelimPairType::QUOTES_CHAR):
         current->setType(NodeType::LITERAL);
         current->setStatus(asint(LitType::CHAR));
         return true;
      case asint(DelimPairType::QUOTES_STR):
         current->setType(NodeType::LITERAL);
         current->setStatus(asint(LitType::STRING));
         return true;
      //block delims
      caseMacro(PARENS)
      caseMacro(SUBSCRIPT)
      caseMacro(INIT_LIST)
      caseMacro(SPECIALIZER)

   }

   return false;
}
#undef caseMacro

#endif //PARSER_HELPERS_CPP