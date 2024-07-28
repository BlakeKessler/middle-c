#pragma once
#ifndef MCS_STATIC_ARR_HPP
#define MCS_STATIC_ARR_HPP

#include <MCS.hpp>
#include <bit>
#include <memory>
#include <cstring>

template <typename T, uint _size> class mcs::static_arr {
   private:
      T _buf[_size];
   public:
      static_arr();
      static_arr(T* buf);
      static_arr(T initList[_size]);
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
      // T* release() { T* temp = _buf; _buf = nullptr; return temp; }

      //typecasts
      operator bool() { return (bool)_size; }
};



#pragma region src
//!default constructor
//!NOTE: might be unreasonably slow for large _size
template<typename T,uint _size> mcs::static_arr<T,_size>::static_arr() {
   for (uint i = 0; i < _size; ++i) {
      _buf[i] = 0;
   }
}
template<typename T,uint _size> mcs::static_arr<T,_size>::static_arr(T* buf) {
   for (uint i = 0; i < _size; ++i) {
      _buf[i] = buf[i];
   }
}
//!constructor from initializer list
template<typename T,uint _size> mcs::static_arr<T,_size>::static_arr(T initList[_size]) {
   for (uint i = 0; i < _size; ++i) {
      _buf[i] = buf[i];
   }
}

//!bounds-checked element access
template<typename T,uint _size> T& mcs::static_arr<T,_size>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "static_arr of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}
//!bounds-checked element access
template<typename T,uint _size> const T& mcs::static_arr<T,_size>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "static_arr of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}

//!construct in place
template<typename T,uint _size> T* mcs::static_arr<T,_size>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in static_arr of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(_buf + i, args...);
   return _buf + i;
}

#pragma endregion src

#endif //MCS_STATIC_ARR_HPP