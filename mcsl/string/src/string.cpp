#ifndef MCSL_STRING_CPP
#define MCSL_STRING_CPP

#include "string.hpp"
#include "alloc.hpp"
#include <bit>
#include <cassert>
#include <cstring>


//!concatenate other onto the end of this
template<typename s,typename c> mcsl::string& mcsl::string::operator+=(const str_base<s,c>& other) {
   char* addr = end();
   realloc(size() + other.size());
   //copy other
   std::memcpy(addr, other.begin(), other.size()*sizeof(char));
   //return
   return self;
}
//!repeat string
mcsl::string& mcsl::string::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   realloc_exact(repeatCount * size());
   for (uint pos = oldSize; pos < size(); pos+=oldSize) {
      std::memcpy(begin() + pos, begin(), oldSize*sizeof(char));
   }
   _buf[_buf.size()] = '\0';
   return self;
}


//!construct with buffer size
mcsl::string::string(const uint size):
_buf(size+1) {
   
}
//!constructor from c-string and length
mcsl::string::string(const char* str, const uint strlen):
_buf(strlen+1) {
   std::strncpy(_buf.data(),str,strlen*sizeof(str[0]));
   _buf.back() = '\0';
}
//!constructor from null-terminated c-string
mcsl::string::string(const char* str):
string(str,std::strlen(str)) {
   
}

#endif //MCSL_STRING_CPP