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

//!owning array with automatic scope
template <typename T> class mcsl::array : public contig_base<T, array<T>> {
   private:
      T* _buf;
      const uint _size;
   public:
      constexpr array();
      array(const uint size);
      array(T* buf, const uint size);
      array(std::initializer_list<T>);
      ~array() { this->free(); }
      void free() const { mcsl::free(_buf); }

      static constexpr raw_str<5> _name = "array";

      constexpr uint size() const { return _size; }
      constexpr auto& name() const { return _name; }

      //element access
      constexpr T* const* ptr_to_buf() { return &_buf; }
      constexpr T* data() { return _buf; }

      constexpr const T* const* ptr_to_buf() const { return &_buf; }
      constexpr const T* data() const { return _buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto... args);
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

//!construct in place
template<typename T> constexpr T* mcsl::array<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i,name(),_size);
      return nullptr;
   }
   std::construct_at(_buf + i, args...);
   return _buf + i;
}

#pragma endregion src

#endif //MCSL_ARRAY_HPP