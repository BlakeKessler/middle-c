#pragma once
#ifndef MCSL_DYN_ARR_SPAN_HPP
#define MCSL_DYN_ARR_SPAN_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "pair.hpp"
#include "raw_str.hpp"

#include <memory>
#include <utility>

template <typename T> class mcsl::dyn_arr_span : public contig_base<T> {
   private:
      T* const* _ptrToBuf;
      uint _beginIndex;
      uint _size;

      static constexpr const raw_str _nameof = "dyn_arr_span";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr dyn_arr_span();
      constexpr dyn_arr_span(T* const* buf, const uint size);
      constexpr dyn_arr_span(T* const* buf, const uint beginIndex, const uint size);
      constexpr dyn_arr_span(T* const* buf, const mcsl::pair<uint,uint> bounds);

      constexpr uint size() const { return _size; }

      //element access
      constexpr T* const* ptr_to_buf() { return _ptrToBuf; }
      constexpr T* data() { return *_ptrToBuf; }
      constexpr T* begin() { return *_ptrToBuf + _beginIndex; }
      
      constexpr const T* const* ptr_to_buf() const { return _ptrToBuf; }
      constexpr const T* data() const { return *_ptrToBuf; }
      constexpr const T* begin() const { return *_ptrToBuf + _beginIndex; }

      constexpr uint first_index() const { return _beginIndex; }
      constexpr uint last_index() const { return _beginIndex + _size; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args);
};

#pragma region src
//!default constructor
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span():
   _ptrToBuf(nullptr),_beginIndex(0),_size(0) {

}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(0),_size(size) {

}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint beginIndex, const uint size):
   _ptrToBuf(ptrToBuf),_beginIndex(beginIndex),_size(size) {

}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const mcsl::pair<uint,uint> bounds):
   _ptrToBuf(ptrToBuf),_beginIndex(bounds.first),_size(bounds.second - bounds.first) {

}

//!construct in place
template<typename T> constexpr T* mcsl::dyn_arr_span<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i,self.nameof(),_size);
      return nullptr;
   }
   std::construct_at(self.begin() + i, std::forward<decltype(args)>(args)...);
   return self.begin() + i;
}

#pragma endregion src


#endif //MCSL_DYN_ARR_SPAN_HPP