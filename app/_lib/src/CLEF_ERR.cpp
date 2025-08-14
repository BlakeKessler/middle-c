#ifndef CLEF_ERR_CPP
#define CLEF_ERR_CPP

#include "CLEF_ERR.hpp"
#include "CLEF.hpp"

//!array of CLEF error message prefix strings
const mcsl::str_slice clef::ERR_MSG_ARR[] {
   FMT("UNSPECIFIED ERROR"),

   FMT("BAD COMMENT"),
   FMT("BAD LITERAL"),
   FMT("UNRECOGNIZED SOURCE CHAR"),

   FMT("MISSING KEYWORED"),
   FMT("BAD KEYWORD"),
   FMT("MISSING OPERATOR"),
   FMT("BAD OPERATOR")
};

#endif //CLEF_ERR_CPP