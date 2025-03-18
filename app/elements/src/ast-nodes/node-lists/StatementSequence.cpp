#ifndef STMT_SEQ_CPP
#define STMT_SEQ_CPP

#include "ast-nodes/node-lists/StatementSequence.hpp"

#include "io.hpp"

void clef::StmtSeq::printf() const {
   mcsl::printf(mcsl::FMT("statements:"));
   for (uint i = 0; i < size(); ++i) {
      mcsl::printf(mcsl::FMT(" id=%u;"), +self[i]);
   }
}

#endif //STMT_SEQ_CPP