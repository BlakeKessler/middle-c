#ifndef MCS_ALLOC_CPP
#define MCS_ALLOC_CPP

#include "alloc.hpp"

void mcs::free(void* ptr) {
   if(ptr) {
      std::free(ptr);
   }
}

#endif //MCS_ALLOC_CPP