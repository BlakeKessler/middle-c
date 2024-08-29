#pragma once
#ifndef MCSL_RAW_STRING_SPAN_HPP
#define MCSL_RAW_STRING_SPAN_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!non-owning potentially-non-null-terminated string
//!invalidated if the string is reallocated
//!relatively unsafe (even ignoring the invalidation)
//!   nothing stops setting/incrementing _buf or _size beyond the end of the spanned string
class mcsl::raw_str_span : public str_base<raw_str_span, char> {
   private:
      char* _buf;
      uint _size;
   public:
      //constructors
      constexpr raw_str_span(): _buf(nullptr),_size(0) {}
      constexpr raw_str_span(char* str, const uint size):_buf(str),_size(size) {}
      template<typename s, typename c> constexpr raw_str_span(str_base<s,c>& other): raw_str_span(other.data(),other.size()) {}
      template<typename s, typename c> constexpr raw_str_span(str_base<s,c>& other, const uint size): raw_str_span(other.data(),size) { assert(other.size() >= size); }
      template<typename s, typename c> constexpr raw_str_span(str_base<s,c>& other, const uint begin, const uint size): raw_str_span(other.data() + begin, size) { assert(other.size() >= begin + size); }

      static constexpr raw_str<12> _name = "raw_str_span";

      //properties
      constexpr uint size() const { return _size; }
      constexpr auto& name() const { return _name; }
      constexpr raw_str_span& inc_begin(const sint i) { _size -= i; _buf += i; return self; }
      constexpr raw_str_span& set_size(const uint i) { _size = i; return self; }
      constexpr raw_str_span& inc_end(const sint i) { _size += i; return self; }

      //member access
      constexpr char* const* ptr_to_buf() { return &_buf; }
      constexpr char* data() { return _buf; }
      
      constexpr const char* const* ptr_to_buf() const { return &_buf; }
      constexpr const char* data() const { return _buf; }
};

#endif //MCSL_RAW_STRING_SPAN_HPP