#pragma once
#ifndef MCS_ARR_SPAN_HPP
#define MCS_ARR_SPAN_HPP

#include <MCS.hpp>
#include <memory>

template <typename T> class mcs::arr_span {
   private:
      T* _buf;
      uint _size;
   public:
      arr_span();
      arr_span(T* buf, const uint size);
      arr_span(T* buf, T* end);

      //element access
      constexpr uint size() const { return _size; }

      constexpr T* const* ptrToBuf() { return &_buf; }
      T* begin() { return _buf; }
      T* end() { return _buf + _size; }
      T& operator[](const uint i) { return _buf[i]; }
      T& at(const uint i);
      T& front() { return _buf[0]; }
      T& back() { return _buf[_size - 1]; }

      constexpr const T* const* ptrToBuf() const { return &_buf; }
      constexpr const T* begin() const { return _buf; }
      constexpr const T* end() const { return _buf + _size; }
      constexpr const T& operator[](const uint i) const { return _buf[i]; }
      constexpr const T& at(const uint i) const;
      constexpr const T& front() const { return _buf[0]; }
      constexpr const T& back() const { return _buf[_size - 1]; }

      //MODIFIERS
      T* emplace(const uint i, auto... args);
};

#pragma region src
//!default constructor
template<typename T> mcs::arr_span<T>::arr_span():
   _buf(nullptr),_size(0) {

}
template<typename T> mcs::arr_span<T>::arr_span(T* buf, const uint size):
   _buf(buf),_size(size) {

}
template<typename T> mcs::arr_span<T>::arr_span(T* buf, T* end):
   _buf(buf),_size(buf - end) {

}

//!bounds-checked element access
template<typename T> T& mcs::arr_span<T>::at(const uint i) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "arr_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}
//!bounds-checked element access
template<typename T> constexpr const T& mcs::arr_span<T>::at(const uint i) const {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "arr_span of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m");
   }
   return _buf[i];
}

//!construct in place
template<typename T> T* mcs::arr_span<T>::emplace(const uint i, auto... args) {
   if (i >= _size) {
      mcs_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in arr_span of size \033[4m%u\033[24m", i,_size);
      return nullptr;
   }
   std::construct_at(_buf + i, args...);
   return _buf + i;
}

#pragma endregion src


#endif //MCS_ARR_SPAN_HPP