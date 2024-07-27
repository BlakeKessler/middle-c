#ifndef NODE_EVAL_CPP
#define NODE_EVAL_CPP

#include "Node.hpp"

bool clef::Node::checkDelimMatch(Node* close) const {
   return (
      !_token || !close || !close->token()) ? false :
      (*close->token() == BLOCK_DELIMS[(uint)(_token->blockDelimEval())][1]
   );

   // if (!_token || !close || !close->token()) {
   //    return false;
   // }

   // const DelimPairType type = _token->blockDelimEval();
   // return *close->token() == BLOCK_DELIMS[(uint)type][1];
}

#endif //NODE_EVAL_CPP