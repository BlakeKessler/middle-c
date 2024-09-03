#pragma once
#ifndef MCSL_RAW_BUF_STR_HPP
#define MCSL_RAW_BUF_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!owning non-null-terminated string
//!basically only good for data packing
template<uint _capacity, typename size_t>
class mcsl::raw_buf_str : public str_base<char> {
   private:
      char _buf[_capacity];
      size_t _size;

      // static constexpr raw_str<11> _name = "raw_buf_str";
      static constexpr char _name[] = "raw_buf_str";
   public:
      //constructors
      constexpr raw_buf_str():_buf{0},_size{_capacity} {}
      constexpr raw_buf_str(const char* str, const size_t strlen);
      constexpr raw_buf_str(const char* str);
      constexpr raw_buf_str(const str_base<char>& other): raw_buf_str(other.data(), other.size()) {}

      //properties
      constexpr size_t size() const { return _size; }
      constexpr size_t capacity() const { return _capacity; }

      //member access
      constexpr char* const* ptr_to_buf() { return &_buf; }
      constexpr char* data() { return _buf; }
      constexpr const char* const* ptr_to_buf() const { return &_buf; }
      constexpr const char* data() const { return _buf; }
};

template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str, const size_t strsize): _buf{0},_size(strsize) {
   assert(strsize <= _capacity);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}
template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str): _buf{0}, _size(_capacity) {
   for (uint i = 0; i < _capacity; ++i) {
      if (!str[i]) { _size = i; break; }
      _buf[i] = str[i];
   }
}

#endif //MCSL_RAW_BUF_STR_HPP