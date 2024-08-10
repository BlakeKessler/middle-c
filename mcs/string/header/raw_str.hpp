#pragma once
#ifndef RAW_STR_HPP
#define RAW_STR_HPP

#include "MCS.hpp"
#include "alloc.hpp"
#include "char_type.hpp"
#include <cstring>

template<uint _size> class mcs::raw_str {
   private:
      char _buf[_size+1];
   public:
      #pragma region string
      static const bool string_like = true;
      //concatenation
      // raw_str operator+(const raw_str& other) const;
      raw_str& operator+=(const raw_str& other);
      //self-repetition
      // raw_str operator*(const uint repeatCount) const;
      raw_str& operator*=(const uint repeatCount);

      //case modification
      raw_str& alter(char (*const transformer)(const char));       //maybe replace with a macro
      raw_str altered(char (*const transformer)(const char)) const;//maybe replace with a macro

      raw_str& operator+() { return alter(mcs::to_upper); }
      raw_str& operator-() { return alter(mcs::to_lower); }
      raw_str as_upper() const { return altered(mcs::to_upper); }
      raw_str as_lower() const { return altered(mcs::to_lower); }

      //tokenize raw_str by provided delimiters
      dyn_arr<const raw_str> tokenize(const raw_str& delims) const;
      //raw_str slicing
      raw_str substr(const uint stop) const;
      raw_str substr(const uint start, const uint stop) const;
      raw_str substr(const uint start, const uint stop, const uint step);
      #pragma endregion string
      #pragma region charArray
      //constructors
      constexpr raw_str():_buf{0} {}
      constexpr raw_str(const char* str, const uint strlen);
      constexpr raw_str(const char* str);

      //properties
      constexpr uint size() const { return _size; }
      constexpr uint strlen() const { const uint len = std::strlen(_buf); return (len <= _size) ? len : _size; }
      // template<typename T> strcmp(T& other) const {}

      //element access
      constexpr char* const* ptrToBuf() { return &_buf; }
      constexpr char* begin() { return _buf; }
      constexpr char* end() { return _buf + _size; }
      constexpr char& operator[](const uint i) { return _buf[i]; }
      constexpr char& at(const uint i);
      constexpr char& front() { return _buf[0]; }
      constexpr char& back() { return _buf[_size - 1]; }

      constexpr const char* const* ptrToBuf() const { return &_buf; }
      constexpr const char* begin() const { return _buf; }
      constexpr const char* end() const { return _buf + _size; }
      constexpr const char& operator[](const uint i) const { return _buf[i]; }
      constexpr const char& at(const uint i) const;
      constexpr const char& front() const { return _buf[0]; }
      constexpr const char& back() const { return _buf[_size - 1]; }

      //typecasts
      constexpr operator bool() { return (bool)_size; }
      #pragma endregion charArray
};

#pragma region CTAD
// template<> mcs::raw_str(const char* str, const uint _size) -> mcs::raw_str<_size>;
// template<> mcs::raw_str(const char* str) -> mcs::raw_str<std::strlen(str)>;
#pragma endregion CTAD

#include "string_like.hpp"

template<uint _size> constexpr mcs::raw_str<_size>::raw_str(const char* str, const uint strsize): _buf{0} {
   assert(strsize <= _size);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}
template<uint _size> constexpr mcs::raw_str<_size>::raw_str(const char* str): _buf{0} {
   for (uint i = 0; i < _size; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}


template<uint _size> constexpr char& mcs::raw_str<_size>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_str of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}
template<uint _size>constexpr const char& mcs::raw_str<_size>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_str of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}

#endif //RAW_STR_HPP