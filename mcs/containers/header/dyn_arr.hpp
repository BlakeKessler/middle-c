#pragma once
#ifndef MCS_DYN_ARR_HPP
#define MCS_DYN_ARR_HPP

#include "MCS.hpp"
#include "alloc.hpp"
#include <bit>
#include <memory>
#include <initializer_list>
#include <cstring>

template <typename T> class mcs::dyn_arr {
   private:
      uint _bufSize;
      uint _size;
      T* _buf;
   public:
      constexpr dyn_arr();
      dyn_arr(const uint size);
      dyn_arr(const uint size, const uint bufSize);
      dyn_arr(std::initializer_list<T>);
      dyn_arr(dyn_arr&& other);
      ~dyn_arr() { this->free(); }
      void free() const { mcs::free(_buf); }

      //element access
      constexpr uint size() const { return _size; }
      constexpr uint capacity() const { return _bufSize; }
      constexpr T* const* ptrToBuf() { return &_buf; }
      constexpr T* begin() { return _buf; }
      constexpr T* end() { return _buf + _size; }
      constexpr T& operator[](const uint i) { return _buf[i]; }
      constexpr T& at(const uint i);
      constexpr T& front() { return _buf[0]; }
      constexpr T& back() { return _buf[_size - 1]; }

      constexpr const T* const* ptrToBuf() const { return &_buf; }
      constexpr const T* begin() const { return _buf; }
      constexpr const T* end() const { return _buf + _size; }
      constexpr const T& operator[](const uint i) const { return _buf[i]; }
      constexpr const T& at(const uint i) const;
      constexpr const T& front() const { return _buf[0]; }
      constexpr const T& back() const { return _buf[_size - 1]; }

      //MODIFIERS
      //!realloc buffer to at least the specified size
      bool realloc(const uint newSize) { return realloc_exact(std::bit_ceil(newSize)); }
      bool realloc_exact(const uint newSize);
      T* release() { T* temp = _buf; _buf = nullptr; _size = 0; _bufSize = 0; return temp; }
      bool push_back(const T& obj);
      T pop_back();
      T* emplace(const uint i, auto... args);
      T* emplace_back(auto... args);

      //typecasts
      operator bool() { return (bool)_size; }
};

#pragma region src
//!default constructor
template<typename T> constexpr mcs::dyn_arr<T>::dyn_arr():
   _bufSize(0),_size(0),_buf(nullptr) {

}
//!constructor from array size
template<typename T> mcs::dyn_arr<T>::dyn_arr(const uint size):
   _bufSize(std::bit_ceil(size)), _size(size),
   _buf(mcs::calloc<T>(_bufSize)) {

}
//!constructor from array size and buffer size
template<typename T> mcs::dyn_arr<T>::dyn_arr(const uint size, const uint bufSize):
   _bufSize(bufSize), _size(size) {
      if (_bufSize < _size) {
         mcs_throw(ErrCode::SEGFAULT, "cannot construct dyn_arr with array size greater than buffer size (\033[4m%u\033[24m < \033[4m%u\033[24m)",_bufSize,_size);
         _buf = nullptr;
      }
      else {
         _buf = mcs::calloc<T>(_bufSize);
      }
}
//!constructor from initializer list
template<typename T> mcs::dyn_arr<T>::dyn_arr(std::initializer_list<T> initPair):
   _bufSize(initPair.size()),_size(initPair.size()),_buf(mcs::malloc<T>(_bufSize)) {
      std::memcpy(_buf,initPair.begin(),_size * sizeof(T));
}
//!move constructor
template<typename T> mcs::dyn_arr<T>::dyn_arr(dyn_arr&& other):
   _bufSize(other._bufSize),_size(other._size),_buf(other._buf) {
      other.release();
}

//!bounds-checked element access
template<typename T> constexpr T& mcs::dyn_arr<T>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_arr of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}
//!bounds-checked element access
template<typename T> constexpr const T& mcs::dyn_arr<T>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_arr of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}

//!realloc buffer to the specified size
template<typename T> bool mcs::dyn_arr<T>::realloc_exact(const uint newSize) {
   _buf = mcs::realloc<T>(_buf, newSize);;
   _bufSize = newSize;
   return true;
}

//!push to the back of the the array
//!returns if a reallocation was required
template<typename T> bool mcs::dyn_arr<T>::push_back(const T& obj) {
   bool realloced = false;
   if (_size >= _bufSize) {
      realloced = realloc(_size ? std::bit_floor(_size) << 1 : 1);
   }
   _buf[_size++] = obj;
   return realloced;
}
//!remove last element of array
//!returns the removed element
//!zeroes out the index in the array
template<typename T> T mcs::dyn_arr<T>::pop_back() {
   T temp = _buf[--_size];
   std::memset(_buf + _size, 0, sizeof(T));
   return temp;
}
//!construct in place
template<typename T> T* mcs::dyn_arr<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in dyn_arr of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(_buf + i, args...);
   return _buf + i;
}
//!construct in place at back of array
template<typename T> T* mcs::dyn_arr<T>::emplace_back(auto... args) {
   if (_size >= _bufSize) {
      realloc(_size ? std::bit_floor(_size) << 1 : 1);
   }
   return emplace(_size++, args...);
}

#pragma endregion src


#endif //MCS_DYN_ARR_HPP