#pragma once
#ifndef MCSL_RAW_BUF_STR_HPP
#define MCSL_RAW_BUF_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!owning non-null-terminated string
//!basically only good for data packing
template<uint _capacity, typename size_t>
class mcsl::raw_buf_str : public str_base<raw_buf_str<_capacity, size_t>, char> {
   private:
      char _buf[_capacity];
      uint _size;
   public:
      //constructors
      constexpr raw_buf_str():_buf{0},_size{_capacity} {}
      constexpr raw_buf_str(const char* str, const uint strlen);
      constexpr raw_buf_str(const char* str);
      template<typename s, typename c> constexpr raw_buf_str(const str_base<s,c>& other): raw_buf_str(other.data(),other.size()) {}

      static constexpr raw_str<11> _name = "raw_buf_str";

      //properties
      constexpr uint size() const { return _size; }
      constexpr auto& name() const { return _name; }

      //member access
      constexpr char* const* ptr_to_buf() { return &_buf; }
      constexpr char* data() { return _buf; }
      
      constexpr const char* const* ptr_to_buf() const { return &_buf; }
      constexpr const char* data() const { return _buf; }
};

template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str, const uint strsize): _buf{0},_size(strsize) {
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