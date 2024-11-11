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

      static constexpr const raw_str _nameof = "arr_span";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr arr_span();
      constexpr arr_span(T* buf, const uint size);
      constexpr arr_span(T* buf, T* end);

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
};

#pragma region src
//!default constructor
template<typename T> constexpr mcsl::arr_span<T>::arr_span():
   _buf(),_size() {

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
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i,nameof(),_size);
      return nullptr;
   }
   std::construct_at(_buf + i, std::forward<decltype(args)>(args)...);
   return _buf + i;
}

#pragma endregion src


#endif //MCSL_ARR_SPAN_HPP