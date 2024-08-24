#pragma once
#ifndef RAW_BUF_STR_HPP
#define RAW_BUF_STR_HPP

#include "MCS.hpp"
#include "alloc.hpp"
#include "char_type.hpp"
#include <cstring>

template<uint _capacity, typename size_t> class mcs::raw_buf_str {
   private:
      char _buf[_capacity];
      uint _size;
   public:
      #pragma region string
      static const bool string_like = true;
      //concatenation
      // raw_buf_str operator+(const raw_buf_str& other) const;
      raw_buf_str& operator+=(const raw_buf_str& other);
      //self-repetition
      // raw_buf_str operator*(const uint repeatCount) const;
      raw_buf_str& operator*=(const uint repeatCount);

      //case modification
      raw_buf_str& alter(char (*const transformer)(const char));       //maybe replace with a macro
      raw_buf_str altered(char (*const transformer)(const char)) const;//maybe replace with a macro

      raw_buf_str& operator+() { return alter(mcs::to_upper); }
      raw_buf_str& operator-() { return alter(mcs::to_lower); }
      raw_buf_str as_upper() const { return altered(mcs::to_upper); }
      raw_buf_str as_lower() const { return altered(mcs::to_lower); }

      //tokenize raw_buf_str by provided delimiters
      dyn_arr<const raw_buf_str> tokenize(const raw_buf_str& delims) const;
      //raw_buf_str slicing
      raw_buf_str substr(const uint stop) const;
      raw_buf_str substr(const uint start, const uint stop) const;
      raw_buf_str substr(const uint start, const uint stop, const uint step);
      #pragma endregion string
      #pragma region charArray
      //constructors
      constexpr raw_buf_str():_buf{0},_size(0) {}
      constexpr raw_buf_str(const char* str, const uint strlen);
      constexpr raw_buf_str(const char* str);

      //properties
      constexpr byte size() const { return _size; }
      constexpr uint capacity() const { return _capacity; } 
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
// template<> mcs::raw_buf_str(const char* str, const uint _capacity) -> mcs::raw_buf_str<_capacity,size_t>;
// template<> mcs::raw_buf_str(const char* str) -> mcs::raw_buf_str<std::strlen(str)>;
#pragma endregion CTAD

#include "string_like.hpp"

template<uint _capacity, typename size_t> constexpr mcs::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str, const uint strsize): _buf{0},_size(strsize) {
   assert(strsize <= _capacity);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}
template<uint _capacity, typename size_t> constexpr mcs::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str): _buf{0}, _size(_capacity) {
   for (uint i = 0; i < _capacity; ++i) {
      if (!str[i]) { _size = i; break; }
      _buf[i] = str[i];
   }
}


template<uint _capacity, typename size_t> constexpr char& mcs::raw_buf_str<_capacity,size_t>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_buf_str of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}
template<uint _capacity, typename size_t> constexpr const char& mcs::raw_buf_str<_capacity,size_t>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "raw_buf_str of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m",_size,i);
   }
   return _buf[i];
}

#endif //RAW_BUF_STR_HPP