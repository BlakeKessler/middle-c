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
      constexpr static const bool string_like = true;
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
      #pragma endregion string

      #pragma region charArray
      //constructors
      constexpr raw_str_span(): _buf(nullptr),_size(0) {}
      constexpr raw_str_span(char* str, const uint size):_buf(str),_size(size) {}
      constexpr raw_str_span(string& str);
      constexpr raw_str_span(string& str, const uint size);
      constexpr raw_str_span(string& str, const uint begin, const uint size);

      //properties
      constexpr uint size() const { return _size; }
      constexpr raw_str_span& inc_begin(const sint i) { _size -= i; _buf += i; return *this; }
      constexpr raw_str_span& set_size(const uint i) { _size = i; return *this; }
      constexpr raw_str_span& inc_size(const sint i) { _size += i; return *this; }

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

#endif //RAW_STRING_SPAN_HPP