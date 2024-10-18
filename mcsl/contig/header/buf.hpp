#pragma once
#ifndef MCSL_BUF_HPP
#define MCSL_BUF_HPP

#include "MCSL.hpp"

#include <memory>
#include <cassert>

template<typename T, uint _capacity> class mcsl::buf : mcsl::contig_base<T> {
   private:
      T _buf[_capacity];
      uint _size;

      static constexpr const char _nameof[] = "buf";
   public:
      static constexpr const auto& nameof() { return _nameof; }
      
      constexpr buf():_buf{0},_size(0) {}
      template<contig_t other_t> constexpr buf(const other_t& other);
      constexpr buf(std::convertible_to<T> auto&&... initList);

      constexpr uint size() const { return _size; }
      constexpr uint capacity() const { return _capacity; }

      //member access
      constexpr T* const* ptr_to_buf() { return &_buf; }
      constexpr T* data() { return _buf; }
      constexpr T* begin() { return _buf; }
      constexpr const T* const* ptr_to_buf() const { return &_buf; }
      constexpr const T* data() const { return _buf; }
      constexpr const T* begin() const { return _buf; }

      //modifiers
      T* push_back(const T& obj);
      bool pop_back();
      T* emplace(const uint i, auto&&... args);
      T* emplace_back(auto&&... args);
};

template<typename T, uint _capacity> template<mcsl::contig_t other_t> constexpr mcsl::buf<T,_capacity>::buf(const other_t& other):
   _buf{0},
   _size(other.size()) {
      _size = _size > _capacity ? _capacity : _size;
      for (uint i = 0; i < _size; ++i) {
         _buf[i] = other[i];
      }
}
template<typename T, uint _capacity> constexpr mcsl::buf<T,_capacity>::buf(std::convertible_to<T> auto&&... initList):
   _buf{0},
   _size(sizeof...(initList)) {
      assert(_size <= _capacity);

      for (uint i = 0; i < _size; ++i) {
         _buf[i] = initList[i];
      }
}

template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::push_back(const T& obj) {
   if (_size >= _capacity) {
      return nullptr;
   }
   _buf[_size] = obj;
   return _buf + (_size++);
}
template<typename T, uint _capacity> bool mcsl::buf<T,_capacity>::pop_back() {
   if (!_size) {
      return false;
   }
   --_size;
   return true;
}
template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, nameof(), _size);
      return nullptr;
   }
   std::construct_at(_buf + i, std::forward<decltype(args)>(args)...);
   return _buf + i;
}
template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::emplace_back(auto&&... args) {
   if (_size >= _capacity) {
      return nullptr;
   }
   std::construct_at(_buf + _size, std::forward<decltype(args)>(args)...);
   return _buf + (_size++);
}


#endif //MCSL_BUF_HPP