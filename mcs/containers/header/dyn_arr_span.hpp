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
      constexpr dyn_arr_span();
      constexpr dyn_arr_span(T* const* buf, const uint size);
      constexpr dyn_arr_span(T* const* buf, const uint beginIndex, const uint size);
      constexpr dyn_arr_span(T* const* buf, const mcs::pair<uint,uint> bounds);

      //element access
      constexpr uint firstIndex() const { return _beginIndex; }
      constexpr uint lastIndex() const { return _beginIndex + _size; }
      constexpr uint size() const { return _size; }

      constexpr T* const* ptrToBuf() { return _ptrToBuf; }
      constexpr T* begin() { return *_ptrToBuf + _beginIndex; }
      constexpr T* end() { return begin() + _size; }
      constexpr T& operator[](const uint i) { return begin()[i]; }
      constexpr T& at(const uint i);
      constexpr T& front() { return begin()[0]; }
      constexpr T& back() { return begin()[_size - 1]; }

      constexpr const T* const* ptrToBuf() const { return _ptrToBuf; }
      constexpr const T* begin() const { return *_ptrToBuf + _beginIndex; }
      constexpr const T* end() const { return begin() + _size; }
      constexpr const T& operator[](const uint i) const { return begin()[i]; }
      constexpr const T& at(const uint i) const;
      constexpr const T& front() const { return begin()[0]; }
      constexpr const T& back() const { return begin()[_size - 1]; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto... args);
};

#pragma region src
//!default constructor
template<typename T> constexpr mcs::dyn_arr_span<T>::dyn_arr_span():
   _ptrToBuf(nullptr),_beginIndex(0),_size(0) {

}
template<typename T> constexpr mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(0),_size(size) {

}
template<typename T> constexpr mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint beginIndex, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(beginIndex),_size(size) {

}
template<typename T> constexpr mcs::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const mcs::pair<uint,uint> bounds):
   _ptrToBuf(ptrToBuf),_beginIndex(bounds.first),_size(bounds.second - bounds.first) {

}

//!bounds-checked element access
template<typename T> constexpr T& mcs::dyn_arr_span<T>::at(const uint i) {
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
template<typename T> constexpr T* mcs::dyn_arr_span<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in dyn_arr_span of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(begin() + i, args...);
   return begin() + i;
}

#pragma endregion src


#endif //MCS_DYN_ARR_SPAN_HPP