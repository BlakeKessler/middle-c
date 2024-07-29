#pragma once
#ifndef MCS_ALLOC_HPP
#define MCS_ALLOC_HPP

#include "MCS.hpp"
#include <typeinfo>
#include <cassert>

namespace mcs {
#pragma region def

template<typename T> T* malloc(const uint itemCount);
template<typename T> T* calloc(const uint itemCount);
template<typename T> T* realloc(T* buf, const uint newItemCount);

#pragma endregion def
#pragma region src

//!allocate a contiguous array large enough to hold itemCount items of type T
template<typename T> T* malloc(const uint itemCount) {
   if (!itemCount) { return nullptr; }
   if constexpr (typeid(T) == typeid(void)) {
      return std::malloc(itemCount * sizeof(byte));
   } else {
      return static_cast<T*>(std::malloc(itemCount * sizeof(T)));
   }
}
//!allocate and null-intitialize a contiguous array large enough to hold itemCount items of type T
template<typename T> T* calloc(const uint itemCount) {
   if (!itemCount) { return nullptr; }
   if constexpr (typeid(T) == typeid(void)) {
      return std::calloc(itemCount, sizeof(byte));
   } else {
      return static_cast<T*>(std::calloc(itemCount, sizeof(T)));
   }
}
//!allocate and null-intitialize a contiguous array large enough to hold itemCount items of type T
template<typename T> T* realloc(T* buf, const uint newItemCount) {
   if (!newItemCount) { return nullptr; }
   if constexpr (typeid(T) == typeid(void)) {
      return std::realloc(buf, newItemCount * sizeof(byte));
   } else {
      return static_cast<T*>(std::realloc(buf, newItemCount * sizeof(T)));
   }
}

#pragma endregion src
}
#endif //MCS_ALLOC_HPP