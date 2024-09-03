#ifndef MCSL_ALLOC_CPP
#define MCSL_ALLOC_CPP

#include "alloc.hpp"

void mcsl::free(void* &&ptr) {
   if(ptr) {
      std::free(ptr);
      ptr = nullptr;
   }
}

#endif //MCSL_ALLOC_CPP