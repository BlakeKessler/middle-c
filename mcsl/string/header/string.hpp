#pragma once
#ifndef MCSL_STRING_HPP
#define MCSL_STRING_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "dyn_arr.hpp"

//!null-terminated dynamically-sized string
//!NOTE: tiny inefficienies caused by using a dyn_arr instead of re-implementing it with minor modifications
class mcsl::string : public str_base<string, char> {
   private:
      dyn_arr<char> _buf;
   public:
      //constructors
      constexpr string():_buf() {}
      string(const uint size);
      string(const char* str, const uint strlen);
      string(const char* str);
      template<typename s, typename c> string(const str_base<s,c>& other): string(other.data(),other.size()) {}
      ~string() { this->free(); }
      inline void free() const { this->free(); }

      static constexpr raw_str<6> _name = "string";

      //properties
      inline constexpr uint size() const { return _buf.size() - 1; }
      inline constexpr uint capacity() const { return _buf.capacity() - 1; }
      constexpr auto& name() const { return _name; }

      //element access
      constexpr char* const* ptr_to_buf() { return _buf.ptr_to_buf(); }
      constexpr char* data() { return _buf.data(); }

      constexpr const char* const* ptr_to_buf() const { return _buf.ptr_to_buf(); }
      constexpr const char* data() const { return _buf.data(); }
      
      //MODIFIERS
      //!realloc buffer to at least the specified size
      inline bool realloc(const uint newSize) { return _buf.realloc(newSize + 1); }
      inline bool realloc_exact(const uint newSize) { return _buf.realloc_exact(newSize + 1); }
      char* release() { return _buf.release(); }
      bool push_back(const char ch) { _buf.back() = ch; return _buf.push_back('\0'); }
      char pop_back() { const char tmp = back(); _buf.pop_back(); _buf.back() = '\0'; return tmp; }


      template<typename s, typename c> string& operator+=(const str_base<s,c>& other);
      string& operator*=(const uint repeatCount);
};

#endif //MCSL_STRING_HPP