#ifndef STMT_SEQ_CPP
#define STMT_SEQ_CPP

#include "ast-nodes/node-lists/StmtSeq.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::StmtSeq& obj) {
   file.write(mcsl::FMT("statements:"));
   for (uint i = 0; i < obj.size(); ++i) {
      file.printf(FMT(" id=%u;"), +obj[i]);
   }
   return file;
}

#endif //STMT_SEQ_CPP