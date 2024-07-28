#pragma once
#ifndef MCS_ARRAY_HPP
#define MCS_ARRAY_HPP

#include "MCS.hpp"
#include <bit>
#include <memory>
#include <cstring>

#include <initializer_list>

template <typename T> class mcs::array {
   private:
      T* _buf;
      const uint _size;
   public:
      array();
      array(const uint size);
      array(T* buf, const uint size);
      array(std::initializer_list<T>);
      ~array() { this->free(); }
      void free() const { std::free(_buf); }

      //element access
      uint size() const { return _size; }
      T* const* ptrToBuf() { return &_buf; }
      T* begin() { return _buf; }
      T* end() { return _buf + _size; }
      T& operator[](const uint i) { return _buf[i]; }
      T& at(const uint i);
      T& front() { return _buf[0]; }
      T& back() { return _buf[_size - 1]; }

      const T* const* ptrToBuf() const { return &_buf; }
      const T* begin() const { return _buf; }
      const T* end() const { return _buf + _size; }
      const T& operator[](const uint i) const { return _buf[i]; }
      const T& at(const uint i) const;
      const T& front() const { return _buf[0]; }
      const T& back() const { return _buf[_size - 1]; }

      //MODIFIERS
      T* emplace(const uint i, auto... args);
      T* release() { T* temp = _buf; _buf = nullptr; return temp; }

      //typecasts
      operator bool() { return (bool)_size; }
};

#pragma region src
//!default constructor
template<typename T> mcs::array<T>::array():
   _buf(nullptr),_size(0) {

}
//!allocate array by size (in elements)
template<typename T> mcs::array<T>::array(const uint size):
   _buf((T*)malloc(size * sizeof(T))),_size(size) {

}
//!constructor from raw pointer to buffer and size of buffer (in elements)
template<typename T> mcs::array<T>::array(T* buf, const uint size):
   _buf(buf),_size(size) {

}
//!constructor from initializer list
template<typename T> mcs::array<T>::array(std::initializer_list<T> initPair):
   _buf((T*)malloc(initPair.size() * sizeof(T))),_size(initPair.size()) {
      std::memcpy(_buf,initPair.begin(),_size * sizeof(T));
}

//!bounds-checked element access
template<typename T> T& mcs::array<T>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "array of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}
//!bounds-checked element access
template<typename T> const T& mcs::array<T>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "array of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}

//!construct in place
template<typename T> T* mcs::array<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in array of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(_buf + i, args...);
   return _buf + i;
}

#pragma endregion src

#endif //MCS_ARRAY_HPP