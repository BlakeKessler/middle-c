#pragma once
#ifndef MCS_DYN_ARR_SPAN_HPP
#define MCS_DYN_ARR_SPAN_HPP

#include "MCS.hpp"
#include "pair.hpp"
#include <memory>

template <typename T> class mcs::dyn_arr_span {
   private:
      T* const* _ptrToBuf;
      uint _beginIndex;
      uint _size;
   public:
      dyn_arr_span();
      dyn_arr_span(T* const* buf, const uint size);
      dyn_arr_span(T* const* buf, const uint beginIndex, const uint size);
      dyn_arr_span(T* const* buf, const mcs::pair<uint,uint> bounds);

      //element access
      uint firstIndex() const { return _beginIndex; }
      uint lastIndex() const { return _beginIndex + _size; }
      uint size() const { return _size; }

      constexpr T* const* ptrToBuf() { return _ptrToBuf; }
      T* begin() { return *_ptrToBuf + _beginIndex; }
      T* end() { return begin() + _size; }
      T& operator[](const uint i) { return begin()[i]; }
      T& at(const uint i);
      T& front() { return begin()[0]; }
      T& back() { return begin()[_size - 1]; }

      constexpr const T* const* ptrToBuf() const { return _ptrToBuf; }
      constexpr const T* begin() const { return *_ptrToBuf + _beginIndex; }
      constexpr const T* end() const { return begin() + _size; }
      constexpr const T& operator[](const uint i) const { return begin()[i]; }
      constexpr const T& at(const uint i) const;
      constexpr const T& front() const { return begin()[0]; }
      constexpr const T& back() const { return begin()[_size - 1]; }

      //MODIFIERS
      T* emplace(const uint i, auto... args);
};

#pragma region src
//!default constructor
template<typename T> mcs::dyn_arr_span<T>::dyn_arr_span():
   _ptrToBuf(nullptr),_beginIndex(0),_size(0) {

}
template<typename T> mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(0),_size(size) {

}
template<typename T> mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint beginIndex, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(beginIndex),_size(size) {

}
template<typename T> mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const mcs::pair<uint,uint> bounds):
   _ptrToBuf(ptrToBuf),_beginIndex(bounds.first),_size(bounds.second - bounds.first) {

}

//!bounds-checked element access
template<typename T> T& mcs::dyn_arr_span<T>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_arr_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return begin()[i];
}
//!bounds-checked element access
template<typename T> constexpr const T& mcs::dyn_arr_span<T>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "dyn_arr_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return begin()[i];
}

//!construct in place
template<typename T> T* mcs::dyn_arr_span<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in dyn_arr_span of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(begin() + i, args...);
   return begin() + i;
}

#pragma endregion src


#endif //MCS_DYN_ARR_SPAN_HPP