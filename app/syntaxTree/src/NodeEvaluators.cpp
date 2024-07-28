#ifndef NODE_EVAL_CPP
#define NODE_EVAL_CPP

#include "Node.hpp"

bool clef::Node::checkDelimMatch(Node* close) const {
   return (
      !_token || !close || !close->token()) ? false :
      (*close->token() == arr[+_token->blockDelimEval()][1]
   );
}

#endif //NODE_EVAL_CPP