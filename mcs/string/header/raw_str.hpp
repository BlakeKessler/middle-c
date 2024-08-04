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
      constexpr raw_str() { std::memset(_buf, 0, _size*sizeof(char)); }
      raw_str(const char* str, const uint strlen);
      raw_str(const char* str);

      //properties
      constexpr uint size() const { return _size; }

      //element access
      char* const* ptrToBuf() { return &_buf; }
      char* begin() { return _buf; }
      char* end() { return _buf + _size; }
      char& operator[](const uint i) { return _buf[i]; }
      char& at(const uint i);
      char& front() { return _buf[0]; }
      char& back() { return _buf[_size - 1]; }

      constexpr const char* const* ptrToBuf() const { return &_buf; }
      constexpr const char* begin() const { return _buf; }
      constexpr const char* end() const { return _buf + _size; }
      constexpr const char& operator[](const uint i) const { return _buf[i]; }
      constexpr const char& at(const uint i) const;
      constexpr const char& front() const { return _buf[0]; }
      constexpr const char& back() const { return _buf[_size - 1]; }

      //typecasts
      operator bool() { return (bool)_size; }
      #pragma endregion charArray
};

#pragma region CTAD
// template<> mcs::raw_str(const char* str, const uint _size) -> mcs::raw_str<_size>;
// template<> mcs::raw_str(const char* str) -> mcs::raw_str<std::strlen(str)>;
#pragma endregion CTAD

#include "string_like.hpp"

template<uint _size>mcs::raw_str<_size>::raw_str(const char* str, const uint strsize) {
   assert(strsize <= _size);
   const uint len = std::strlen(str);
   assert(len <= strsize);
   std::memset(_buf,0,sizeof(_buf));
   std::memcpy(_buf,str,strsize*sizeof(char));
   _buf[_size] = '\0';
}
template<uint _size>mcs::raw_str<_size>::raw_str(const char* str) {
   const uint len = std::strlen(str);
   assert(len <= _size);
   std::memset(_buf,0,sizeof(_buf));
   std::memcpy(_buf,str,_size*sizeof(char));
   _buf[_size] = '\0';
}


template<uint _size>char& mcs::raw_str<_size>::at(const uint i) {
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