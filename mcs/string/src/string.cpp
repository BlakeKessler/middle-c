#ifndef STRING_CPP
#define STRING_CPP

#include "string.hpp"
#include <bit>
#include <cassert>
#include <cstring>

#pragma region string

//!concatenate other onto the end of this
mcs::string& mcs::string::operator+=(const string& other) {
   _size += other._size;
   realloc(_size);
   //copy other
   std::memcpy(end(), other.begin(), other.size()*sizeof(char));
   //return
   return *this;
}
//!repeat string
mcs::string& mcs::string::operator*=(const uint repeatCount) {
   const uint oldSize = _size;
   realloc_exact(repeatCount * _size);
   for (uint pos = oldSize; pos < _size; pos+=oldSize) {
      std::memcpy(_buf + pos, _buf, oldSize*sizeof(char));
   }
   _buf[_size] = '\0';
   return *this;
}

//!transform the string by passing each char through a function (char => char)
mcs::string& mcs::string::alter(char (*transformer)(const char)) {
   for (uint i = 0; i < _size; ++i) {
      _buf[i] = transformer(_buf[i]);
   }
   return *this;
}
//!transform a copy of the string by passing each char through a function (char => char)
mcs::string mcs::string::altered(char (*transformer)(const char)) const {
   string alteredStr{};
   alteredStr.realloc_exact(_bufSize);
   for (uint i = 0; i < _size; ++i) {
      alteredStr._buf[i] = transformer(_buf[i]);
   }
   return alteredStr;
}
#pragma endregion string

#pragma region charArray
//!default constructor
mcs::string::string():
_bufSize(0),_size(0),_buf(nullptr) {

}
//!construct with buffer size
mcs::string::string(const uint size):
_bufSize(size+1),_size(0),_buf((char*)std::calloc(_bufSize,sizeof(char))) {
   
}
//!constructor from c-string and length
mcs::string::string(const char* str, const uint strlen):
_bufSize(std::bit_floor(strlen + 1)),_size(strlen),
_buf((char*)malloc(_bufSize * sizeof(char))) {
   assert(std::strlen(str) <= strlen);
   std::strncpy(_buf,str,strlen);
   _buf[_size] = '\0';
   _buf[_bufSize] = '\0';
}
//!constructor from null-terminated c-string
mcs::string::string(const char* str):
_bufSize(std::bit_floor((uint)std::strlen(str) + 1)),_size(std::strlen(str)),
_buf((char*)malloc(_bufSize * sizeof(char))) {
   std::strncpy(_buf,str,_size);
   _buf[_size] = '\0';
   _buf[_bufSize] = '\0';
}

//!bounds-checked element access
char& mcs::string::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "string of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}
//!bounds-checked element access
const char& mcs::string::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "string of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}


//!std::realloc buffer to the specified size
bool mcs::string::realloc_exact(const uint newSize) {
   char* temp = (char*)std::realloc(_buf, (newSize + 1) * sizeof(char));
   if (!temp) {
      mcs_throw(ErrCode::ALLOC_FAIL, "failed string realloc (buffer size %u -> %u, string size %u)", _bufSize, newSize, _size);
      return false;
   }
   _buf = temp;
   _bufSize = newSize + 1;
   _buf[_size] = '\0';
   return true;
}

//!push to the back of the the array
//!returns if a reallocation was required
bool mcs::string::push_back(const char ch) {
   bool realloced = false;
   if ((_size + 1) >= _bufSize) {
      realloced = realloc(_size ? std::bit_floor(_size) << 1 : 1);
   }
   _buf[_size++] = ch;
   _buf[_size] = '\0';
   return realloced;
}
//!remove last element of array
//!returns the removed element
//!zeroes out the index in the array
char mcs::string::pop_back() {
   char temp = _buf[--_size];
   _buf[_size] = '\0';
   return temp;
}
#pragma endregion charArray

#endif //STRING_CPP