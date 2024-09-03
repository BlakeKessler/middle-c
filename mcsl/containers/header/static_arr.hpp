#pragma once
#ifndef MCSL_STATIC_ARR_HPP
#define MCSL_STATIC_ARR_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "raw_str.hpp"

#include <bit>
#include <memory>
#include <cstring>
#include <cassert>
#include <initializer_list>
#include <utility>

template <typename T, uint _size> class mcsl::static_arr : public contig_base<T> {
   private:
      T _buf[_size];

      static constexpr const raw_str _name = "static_arr";
   public:
      static constexpr const auto& name() { return _name; }

      constexpr static_arr();
      constexpr static_arr(const contig_base<T>& buf) requires requires{ buf.size() >= _size; } { for (uint i = 0; i < _size; ++i) { _buf[i] = buf[i]; }}
      constexpr static_arr(T (&buf)[_size]);
      // constexpr static_arr(std::initializer_list<T> init);
      constexpr static_arr(std::convertible_to<T> auto... initList) requires requires { sizeof...(initList) == _size; }:_buf{std::forward<decltype(initList)>(initList)...} {}
      constexpr ~static_arr() {}

      constexpr uint size() const { return _size; }

      //member access
      constexpr T* const* ptr_to_buf() { return _buf; }
      constexpr T* data() { return _buf; }
      constexpr const T* const* ptr_to_buf() const { return _buf; }
      constexpr const T* data() const { return _buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args);
};

#pragma region CTAD
namespace mcsl {
   template<typename T,std::same_as<T> ...U> static_arr(T, U...) -> static_arr<T,1+sizeof...(U)>;
}
#pragma endregion CTAD

#pragma region src
//!default constructor
//!NOTE: might be unreasonably slow for large _size
template<typename T,uint _size> constexpr mcsl::static_arr<T,_size>::static_arr():_buf{} {
   std::memset(_buf,0,_size*sizeof(T));
}
//!constructor from raw array
template<typename T,uint _size> constexpr mcsl::static_arr<T,_size>::static_arr(T (&buf)[_size]):_buf{} {
   for (uint i = 0; i < _size; ++i) {
      _buf[i] = buf[i];
   }
}

//!construct in place
template<typename T,uint _size> constexpr T* mcsl::static_arr<T,_size>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, self.name(), _size);
      return nullptr;
   }
   std::construct_at(_buf + i, std::forward<decltype(args)>(args)...);
   return _buf + i;
}

#pragma endregion src

#endif //MCSL_STATIC_ARR_HPP