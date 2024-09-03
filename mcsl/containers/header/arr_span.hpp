#pragma once
#ifndef MCSL_ARR_SPAN_HPP
#define MCSL_ARR_SPAN_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include <memory>
#include <utility>

//!non-owning array
template <typename T> class mcsl::arr_span : public contig_base<T> {
   private:
      T* _buf;
      uint _size;

      // static constexpr raw_str<8> _name = "arr_span";
      static constexpr char _name[] = "arr_span";
   public:
      constexpr arr_span();
      constexpr arr_span(T* buf, const uint size);
      constexpr arr_span(T* buf, T* end);

      constexpr uint size() const { return _size; }

      //member access
      constexpr char* const* ptr_to_buf(this auto&& obj) { return &obj._buf; }
      constexpr char* data(this auto&& obj) { return obj._buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args);
};

#pragma region src
//!default constructor
template<typename T> constexpr mcsl::arr_span<T>::arr_span():
   _buf(nullptr),_size(0) {

}
template<typename T> constexpr mcsl::arr_span<T>::arr_span(T* buf, const uint size):
   _buf(buf),_size(size) {

}
template<typename T> constexpr mcsl::arr_span<T>::arr_span(T* buf, T* end):
   _buf(buf),_size(buf - end) {

}

//!construct in place
template<typename T> constexpr T* mcsl::arr_span<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i,name(),_size);
      return nullptr;
   }
   std::construct_at(_buf + i, std::forward<decltype(args)>(args)...);
   return _buf + i;
}

#pragma endregion src


#endif //MCSL_ARR_SPAN_HPP