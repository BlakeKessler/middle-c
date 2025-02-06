#ifndef STMT_SEQ_CPP
#define STMT_SEQ_CPP

#include "ast-nodes/node-lists/StatementSequence.hpp"

#include <cstdio>

void clef::StmtSeq::printf() const {
   std::printf("statements:");
   for (uint i = 0; i < size(); ++i) {
      std::printf(" id=%u;", +self[i]);
   }
}

#endif //STMT_SEQ_CPP