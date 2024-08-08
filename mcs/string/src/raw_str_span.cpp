#ifndef RAW_STRING_SPAN_CPP
#define RAW_STRING_SPAN_CPP

#include "raw_str_span.hpp"
#include <cassert>

#pragma region string_span

//!transform the raw_str_span by passing each char through a function (char => char)
mcs::raw_str_span& mcs::raw_str_span::alter(char (*transformer)(const char)) {
   for (uint i = 0; i < _size; ++i) {
      begin()[i] = transformer(begin()[i]);
   }
   return *this;
}
//!transform a copy of the raw_str_span by passing each char through a function (char => char)
mcs::string mcs::raw_str_span::altered(char (*transformer)(const char)) const {
   string alteredStr{};
   alteredStr.realloc_exact(_size);
   for (uint i = 0; i < _size; ++i) {
      alteredStr[i] = transformer(begin()[i]);
   }
   return alteredStr;
}
#pragma endregion string_span

#pragma region charArray
// //!default constructor
// constexpr mcs::raw_str_span::raw_str_span():
// _buf(nullptr),_size(0) {

// }
// //!constructor from c-raw_str_span and length
// constexpr mcs::raw_str_span::raw_str_span(char* str, const uint size):
// _buf(str),_size(size) {
   
// }
//!constructor from c-raw_str_span and length
constexpr mcs::raw_str_span::raw_str_span(string& str):
_buf(str.begin()),_size(str.size()) {
   
}
//!constructor from c-raw_str_span and length
constexpr mcs::raw_str_span::raw_str_span(string& str, const uint size):
_buf(str.begin()),_size(size) {
   assert(size <= str.size());
}
//!constructor from c-raw_str_span and length
constexpr mcs::raw_str_span::raw_str_span(string& str, const uint begin, const uint size):
_buf(str.begin()),_size(size) {
   assert((begin + size) <= str.size());
}

//!bounds-checked element access
constexpr char& mcs::raw_str_span::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}
//!bounds-checked element access
constexpr const char& mcs::raw_str_span::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}

#pragma endregion charArray

#endif //RAW_STRING_SPAN_CPP