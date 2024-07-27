#ifndef MCS_ERR_CPP
#define MCS_ERR_CPP

#include "mcs_throw.hpp"

//!array of MCS error message prefix strings
const char* mcs::ERR_MSG_ARR[] {
   "UNSPECIFIED ERROR: ",
   "Segmentation fault: ",
   "Allocation failure: "
};

#endif //MCS_ERR_CPP