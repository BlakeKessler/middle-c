#ifndef MCSL_ERR_CPP
#define MCSL_ERR_CPP

#include "mcsl_throw.hpp"

//!array of MCSL error message prefix strings
const char* mcsl::ERR_MSG_ARR[] {
   "UNSPECIFIED ERROR: ",
   "Segmentation fault: ",
   "Allocation failure: "
};

#endif //MCSL_ERR_CPP