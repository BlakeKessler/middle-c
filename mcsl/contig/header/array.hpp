#pragma once
#ifndef MCSL_ARRAY_HPP
#define MCSL_ARRAY_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "alloc.hpp"
#include "raw_str.hpp"

#include <memory>
#include <bit>
#include <cstring>
#include <initializer_list>
#include <utility>

//!owning array with automatic scope
template <typename T> class mcsl::array : public contig_base<T> {
   private:
      T* _buf;
      const uint _size;

      static constexpr const raw_str _name = "array";
   public:
      static constexpr const auto& name() { return _name; }

      constexpr array();
      array(const uint size);
      array(T* buf, const uint size);
      array(std::initializer_list<T>);
      array(array&& other);
      array(std::convertible_to<T> auto... initList) requires requires { sizeof...(initList) == _size; }:_buf{std::forward<decltype(initList)>(initList)...} {}
      ~array() { self.free(); }
      void free() const { mcsl::free(_buf); }

      constexpr uint size() const { return _size; }
      
      //member access
      constexpr T* const* ptr_to_buf() { return &_buf; }
      constexpr T* data() { return _buf; }
      constexpr T* begin() { return _buf; }
      constexpr const T* const* ptr_to_buf() const { return &_buf; }
      constexpr const T* data() const { return _buf; }
      constexpr const T* begin() const { return _buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args);
      T* release() { T* temp = _buf; _buf = nullptr; return temp; }
};

#pragma region src
//!default constructor
template<typename T> constexpr mcsl::array<T>::array():
   _buf(nullptr),_size(0) {

}
//!allocate array by size (in elements)
template<typename T> mcsl::array<T>::array(const uint size):
   _buf(mcsl::calloc<T>(size)),_size(size) {

}
//!copy constructor from raw pointer to buffer and size of buffer (in elements)
template<typename T> mcsl::array<T>::array(T* buf, const uint size):
   _buf(mcsl::malloc<T>(size)),_size(size) {
      std::memcpy(_buf,buf,_size * sizeof(T));
}
//!constructor from initializer list
template<typename T> mcsl::array<T>::array(std::initializer_list<T> initPair):
   _buf(mcsl::malloc<T>(initPair.size())),_size(initPair.size()) {
      std::memcpy(_buf,initPair.begin(),_size * sizeof(T));
}
//!move constructor
template<typename T> mcsl::array<T>::array(array&& other):
   _buf(other._buf),_size(other._size) {
      other.release();
}

//!construct in place
template<typename T> constexpr T* mcsl::array<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, self.name(), _size);
      return nullptr;
   }
   std::construct_at(_buf + i, std::forward<decltype(args)>(args)...);
   return _buf + i;
}

#pragma endregion src

#endif //MCSL_ARRAY_HPP