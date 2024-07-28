#pragma once
#ifndef RAW_STRING_SPAN_HPP
#define RAW_STRING_SPAN_HPP

#include "MCS.hpp"
#include "string.hpp"
#include "string_like.hpp"
#include "char_type.hpp"

class mcs::raw_str_span {
   private:
      char* _buf;
      uint _size;
   public:
      #pragma region string
      //case modification
      raw_str_span& alter(char (*const transformer)(const char));       //maybe replace with a macro
      string altered(char (*const transformer)(const char)) const;//maybe replace with a macro

      raw_str_span& operator+() { return alter(mcs::to_upper); }
      raw_str_span& operator-() { return alter(mcs::to_lower); }
      string as_upper() const { return altered(mcs::to_upper); }
      string as_lower() const { return altered(mcs::to_lower); }

      //tokenize raw_str_span by provided delimiters
      dyn_arr<const raw_str_span> tokenize(const raw_str_span& delims) const;
      //raw_str_span slicing
      string substr(const uint stop) const;
      string substr(const uint start, const uint stop) const;
      string substr(const uint start, const uint stop, const uint step);


      //comparison to C-style string
      bool operator==(const char* str) const;

      #pragma endregion string

      #pragma region charArray
      //constructors
      raw_str_span();
      raw_str_span(char* str, const uint strlen);
      raw_str_span(string& str);
      raw_str_span(string& str, const uint size);
      raw_str_span(string& str, const uint begin, const uint size);

      //properties
      uint size() const { return _size; }
      raw_str_span& inc_begin(const sint i) { _size -= i; _buf += i; return *this; }
      raw_str_span& set_size(const uint i) { _size = i; return *this; }
      raw_str_span& inc_size(const sint i) { _size += i; return *this; }

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

      //typecasts
      operator bool() { return (bool)_size; }
      #pragma endregion charArray
};

#endif //RAW_STRING_SPAN_HPP