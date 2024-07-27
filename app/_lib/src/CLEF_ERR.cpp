#ifndef CLEF_ERR_CPP
#define CLEF_ERR_CPP

#include "CLEF_ERR.hpp"
#include "CLEF.hpp"

//!array of CLEF error message prefix strings
const char* clef::ERR_MSG_ARR[] {
   "UNSPECIFIED ERROR: ",
   "PARSER ERROR: ",
   "Unclosed bracket pair/quotes: ",
   "Segmentation fault: ",
   "Allocation failure: "
};

#endif //CLEF_ERR_CPP