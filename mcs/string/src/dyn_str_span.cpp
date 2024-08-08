#ifndef DYN_STRING_SPAN_CPP
#define DYN_STRING_SPAN_CPP

#include "dyn_str_span.hpp"
#include <cassert>

#pragma region string_span

//!transform the dyn_str_span by passing each char through a function (char => char)
mcs::dyn_str_span& mcs::dyn_str_span::alter(char (*transformer)(const char)) {
   for (uint i = 0; i < _size; ++i) {
      begin()[i] = transformer(begin()[i]);
   }
   return *this;
}
//!transform a copy of the dyn_str_span by passing each char through a function (char => char)
mcs::string mcs::dyn_str_span::altered(char (*transformer)(const char)) const {
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
// mcs::dyn_str_span::dyn_str_span():
// _ptrToBuf(nullptr),_begin(0),_size(0) {

// }
//!constructor from c-dyn_str_span and length
constexpr mcs::dyn_str_span::dyn_str_span(string& str):
_ptrToBuf(str.ptrToBuf()),_begin(0),_size(str.size()) {
   
}
//!constructor from c-dyn_str_span and length
constexpr mcs::dyn_str_span::dyn_str_span(string& str, const uint size):
_ptrToBuf(str.ptrToBuf()),_begin(0),_size(size) {
   assert(size <= str.size());
}
//!constructor from c-dyn_str_span and length
constexpr mcs::dyn_str_span::dyn_str_span(string& str, const uint begin, const uint size):
_ptrToBuf(str.ptrToBuf()),_begin(begin),_size(size) {
   assert((begin + size) <= str.size());
}

//!bounds-checked element access
constexpr char& mcs::dyn_str_span::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return (*_ptrToBuf)[_begin + i];
}
//!bounds-checked element access
constexpr const char& mcs::dyn_str_span::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return (*_ptrToBuf)[_begin + i];
}

#pragma endregion charArray

#endif //DYN_STRING_SPAN_CPP