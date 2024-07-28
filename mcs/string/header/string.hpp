#pragma once
#ifndef STRING_HPP
#define STRING_HPP

#include "MCS.hpp"
#include "char_type.hpp"

class mcs::string {
   private:
      uint _bufSize;
      uint _size;
      char* _buf;
   public:
      #pragma region string
      //concatenation
      // string operator+(const string& other) const;
      string& operator+=(const string& other);
      //self-repetition
      // string operator*(const uint repeatCount) const;
      string& operator*=(const uint repeatCount);

      //case modification
      string& alter(char (*const transformer)(const char));       //maybe replace with a macro
      string altered(char (*const transformer)(const char)) const;//maybe replace with a macro

      string& operator+() { return alter(mcs::to_upper); }
      string& operator-() { return alter(mcs::to_lower); }
      string as_upper() const { return altered(mcs::to_upper); }
      string as_lower() const { return altered(mcs::to_lower); }

      //tokenize string by provided delimiters
      dyn_arr<const string> tokenize(const string& delims) const;
      //string slicing
      string substr(const uint stop) const;
      string substr(const uint start, const uint stop) const;
      string substr(const uint start, const uint stop, const uint step);
      #pragma endregion string

      #pragma region charArray
      //constructors
      string();
      string(const uint size);
      string(const char* str, const uint strlen);
      string(const char* str);
      ~string() { if(_buf) { this->free(); } }
      void free() const { std::free(_buf); }

      //properties
      uint size() const { return _size; }
      uint capacity() const { return _bufSize - 1; }

      //element access
      char* const* ptrToBuf() { return &_buf; }
      char* begin() { return _buf; }
      char* end() { return _buf + _size; }
      char& operator[](const uint i) { return _buf[i]; }
      char& at(const uint i);
      char& front() { return _buf[0]; }
      char& back() { return _buf[_size - 1]; }

      const char* const* ptrToBuf() const { return &_buf; }
      const char* begin() const { return _buf; }
      const char* end() const { return _buf + _size; }
      const char& operator[](const uint i) const { return _buf[i]; }
      const char& at(const uint i) const;
      const char& front() const { return _buf[0]; }
      const char& back() const { return _buf[_size - 1]; }
      
      //MODIFIERS
      bool resize(const uint i) { return (i < _bufSize) ? (_size = i), true : false; }
      //!std::realloc buffer to at least the specified size
      bool realloc(const uint newSize) { return realloc_exact(std::bit_ceil(newSize)); }
      bool realloc_exact(const uint newSize);
      char* release() { char* temp = _buf; _buf = nullptr; _size = 0; _bufSize = 0; return temp; }
      bool push_back(const char ch);
      char pop_back();

      //typecasts
      operator bool() { return (bool)_size; }
      #pragma endregion charArray
};

#include "string_like.hpp"

#endif //STRING_HPP