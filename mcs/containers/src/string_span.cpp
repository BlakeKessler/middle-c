#ifndef STRING_SPAN_CPP
#define STRING_SPAN_CPP

#include "string_span.hpp"
#include <bit>
#include <cassert>
#include <cstring>

#pragma region string_span
// //!create new dyn_str_span containing the result of concatenating other to the end of this
// mcs::string mcs::dyn_str_span::operator+(const dyn_str_span& other) const {
//    //copy this
//    string outStr(begin(),_size + other._size);
//    //copy other
//    std::memcpy(outStr.end(), other.begin(), other.size()*sizeof(char));
//    //return
//    return outStr;
// }
// //!repeat dyn_str_span
// mcs::string mcs::dyn_str_span::operator*(const uint repeatCount) const {
//    string outStr{};
//    outStr.realloc_exact(repeatCount * _size);
//    for (uint pos = 0; pos < outStr.size(); pos+=_size) {
//       std::memcpy(outStr.begin() + pos, *_ptrToBuf + _begin, _size*sizeof(char));
//    }
//    *outStr.end() = '\0';
//    return outStr;
// }

// //!compare string_spans for exact equality
// bool mcs::dyn_str_span::operator==(const dyn_str_span& other) const {
//    //check size
//    if (_size != other._size) {
//       return false;
//    }
//    //compare
//    return !std::memcmp(begin(), other.begin(), _size*sizeof(char));
// }
// //!compare string_spans for exact equality
// bool mcs::dyn_str_span::operator!=(const dyn_str_span& other) const {
//    //check size
//    if (_size != other._size) {
//       return true;
//    }
//    //compare
//    return std::memcmp(begin(), other.begin(), _size*sizeof(char));
// }
// //!spaceship operator: magnitude of return value is index of first difference
// sint mcs::dyn_str_span::operator<=>(const dyn_str_span& other) const {
//    for (uint i = 0; i < _size; ++i) {
//       if ((*this)[i] != other[i]) {
//          return i * signofdif((*this)[i], other[i]);
//       }
//    }
//    return 0;
// }
// //!comparison by "subtraction" (basically just strcmp)
// sint mcs::dyn_str_span::operator-(const dyn_str_span& other) const {
//    for (uint i = 0; i < _size; ++i) {
//       if ((*this)[i] != other[i]) {
//          return (*this)[i] - other[i];
//       }
//    }
//    return 0;
// }
// // //!normalized three-way comparison
// // sint mcs::dyn_str_span::operator<|>(const dyn_str_span& other) const {
// //    for (uint i = 0; i < _size; ++i) {
// //       if (_buf[i] != other._buf[i]) {
// //          return signofdif(_buf[i], other._buf[i]);
// //       }
// //    }
// //    return 0;
// // }
// //!NOTE: use <|> once in Middle C
// bool mcs::dyn_str_span::operator<(const dyn_str_span& other) const {
//    return (other<=>*this) < 0;
// }
// bool mcs::dyn_str_span::operator>(const dyn_str_span& other) const {
//    return (other<=>*this) > 0;
// }
// bool mcs::dyn_str_span::operator<=(const dyn_str_span& other) const {
//    return (other<=>*this) <= 0;
// }
// bool mcs::dyn_str_span::operator>=(const dyn_str_span& other) const {
//    return (other<=>*this) >= 0;
// }

// uint mcs::dyn_str_span::operator&(const dyn_str_span& other) const {
//    return std::strspn(begin(), other.begin());
// }
// uint mcs::dyn_str_span::operator^(const dyn_str_span& other) const {
//    return std::strcspn(begin(), other.begin());
// }
// //!   {largest n∊{uint} : (other * n) == *this}
// //!   |S|; S = {∀n∊{uint} : (other * n) == *this}
// uint mcs::dyn_str_span::operator%(const dyn_str_span& other) const {
//    if (_size < other._size) {
//       return 0;
//    }
//    uint modulus = 0;
//    while (
//       (other._size * (modulus + 1) > _size) &&
//       !(std::memcmp(begin() + modulus * _size, other.begin(), other._size*sizeof(char)))
//       ) {
//          ++modulus;
//    }
//    return modulus;
// }

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
//!default constructor
mcs::dyn_str_span::dyn_str_span():
_ptrToBuf(nullptr),_begin(0),_size(0) {

}
//!constructor from c-dyn_str_span and length
mcs::dyn_str_span::dyn_str_span(string& str):
_ptrToBuf(str.ptrToBuf()),_begin(0),_size(str.size()) {
   
}
//!constructor from c-dyn_str_span and length
mcs::dyn_str_span::dyn_str_span(string& str, const uint size):
_ptrToBuf(str.ptrToBuf()),_begin(0),_size(size) {
   assert(size <= str.size());
}
//!constructor from c-dyn_str_span and length
mcs::dyn_str_span::dyn_str_span(string& str, const uint begin, const uint size):
_ptrToBuf(str.ptrToBuf()),_begin(begin),_size(size) {
   assert((begin + size) <= str.size());
}

//!bounds-checked element access
char& mcs::dyn_str_span::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return (*_ptrToBuf)[_begin + i];
}
//!bounds-checked element access
const char& mcs::dyn_str_span::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_str_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return (*_ptrToBuf)[_begin + i];
}

#pragma endregion charArray

#endif //STRING_SPAN_CPP