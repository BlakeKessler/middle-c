#ifndef NODE_EVAL_CPP
#define NODE_EVAL_CPP

#include "Node.hpp"

bool clef::Node::checkDelimMatch(Node* close) const {
   return (
      !_token || !close || !close->token()) ? false :
      (*close->token() == BLOCK_DELIMS[+_token->blockDelimEval()].close
   );
}

#endif //NODE_EVAL_CPP