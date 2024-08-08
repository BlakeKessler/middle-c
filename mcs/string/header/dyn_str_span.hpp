#pragma once
#ifndef DYN_STRING_SPAN_HPP
#define DYN_STRING_SPAN_HPP

#include "MCS.hpp"
#include "string.hpp"
#include "string_like.hpp"
#include "char_type.hpp"

class mcs::dyn_str_span {
   private:
      char* const* _ptrToBuf;
      uint _begin;
      uint _size;
   public:
      #pragma region string
      constexpr static const bool string_like = true;
      //case modification
      dyn_str_span& alter(char (*const transformer)(const char));       //maybe replace with a macro
      string altered(char (*const transformer)(const char)) const;//maybe replace with a macro

      dyn_str_span& operator+() { return alter(mcs::to_upper); }
      dyn_str_span& operator-() { return alter(mcs::to_lower); }
      string as_upper() const { return altered(mcs::to_upper); }
      string as_lower() const { return altered(mcs::to_lower); }

      //tokenize dyn_str_span by provided delimiters
      dyn_arr<const dyn_str_span> tokenize(const dyn_str_span& delims) const;
      //dyn_str_span slicing
      string substr(const uint stop) const;
      string substr(const uint start, const uint stop) const;
      string substr(const uint start, const uint stop, const uint step);
      #pragma endregion string

      #pragma region charArray
      //constructors
      constexpr dyn_str_span():_ptrToBuf(nullptr),_begin(0),_size(0) {}
      constexpr dyn_str_span(char* const* str, const uint strlen);
      constexpr dyn_str_span(string& str);
      constexpr dyn_str_span(string& str, const uint size);
      constexpr dyn_str_span(string& str, const uint begin, const uint size);

      //properties
      constexpr uint size() const { return _size; }
      constexpr dyn_str_span& set_begin(const sint i) { _size -= (i - _begin); _begin = i; return *this; }
      constexpr dyn_str_span& inc_begin(const sint i) { _size -= i; _begin += i; return *this; }
      constexpr dyn_str_span& set_size(const uint i) { _size = i; return *this; }
      constexpr dyn_str_span& inc_size(const sint i) { _size += i; return *this; }

      //element access
      constexpr char* const* ptrToBuf() { return _ptrToBuf; }
      constexpr char* begin() { return (*_ptrToBuf) + _begin ; }
      constexpr char* end() { return (*_ptrToBuf) + _begin  + _size; }
      constexpr char& operator[](const uint i) { return (*_ptrToBuf)[i + _begin]; }
      constexpr char& at(const uint i);
      constexpr char& front() { return (*_ptrToBuf)[_begin]; }
      constexpr char& back() { return (*_ptrToBuf)[_begin + _size - 1]; }

      constexpr const char* const* ptrToBuf() const { return _ptrToBuf; }
      constexpr const char* begin() const { return (*_ptrToBuf) + _begin; }
      constexpr const char* end() const { return (*_ptrToBuf) + _begin  + _size; }
      constexpr const char& operator[](const uint i) const { return (*_ptrToBuf)[i + _begin]; }
      constexpr const char& at(const uint i) const;
      constexpr const char& front() const { return (*_ptrToBuf)[_begin]; }
      constexpr const char& back() const { return (*_ptrToBuf)[_begin + _size - 1]; }

      //typecasts
      constexpr operator bool() { return (bool)_size; }
      #pragma endregion charArray
};

#endif //DYN_STRING_SPAN_HPP