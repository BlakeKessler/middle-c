#pragma once
#ifndef MCSL_RAW_STR_HPP
#define MCSL_RAW_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"

#include <cassert>

//!null-terminated string literal
template<uint _size> class mcsl::raw_str : public str_base<char> {
   private:
      char _buf[_size + 1];

      // static constexpr const raw_str<7> _name = "raw_str";
      static constexpr char _name[] = "raw_str";
   public:
      //constructors
      constexpr raw_str() = default;
      constexpr raw_str(const char* str, const uint strlen);
      constexpr raw_str(const char* str);
      constexpr raw_str(const str_base<char>& other): raw_str(other.data(),other.size()) {}
      constexpr ~raw_str() = default;

      //properties
      constexpr uint size() const { return sizeof(_buf) - 1; }
      constexpr static const auto& name() { return _name; }
      // constexpr uint size() const { return _size; }
      // constexpr auto& name() const { return _name; }

      //member access
      constexpr char* const* ptr_to_buf() { return reinterpret_cast<char**>(&_buf); }
      constexpr char* data() { return _buf; }
      constexpr const char* const* ptr_to_buf() const { return reinterpret_cast<char**>(&_buf); }
      constexpr const char* data() const { return _buf; }
};

template<uint _size> constexpr mcsl::raw_str<_size>::raw_str(const char* str, const uint strsize): _buf{} {
   assert(strsize <= _size);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}
template<uint _size> constexpr mcsl::raw_str<_size>::raw_str(const char* str): _buf{} {
   for (uint i = 0; i < _size; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}

#pragma region CTAD
namespace mcsl {
   template<uint len> requires (len > 0) raw_str(const char (&str)[len]) -> raw_str<len-1>;
}
#pragma endregion CTAD

#endif //MCSL_RAW_STR_HPP