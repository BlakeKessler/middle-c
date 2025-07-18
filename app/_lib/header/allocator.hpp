#pragma once
#ifndef CLEF_ALLOCATOR_HPP
#define CLEF_ALLOCATOR_HPP

#include "CLEF.hpp"

#include "arr_list.hpp"
#include "dyn_arr.hpp"

//!allocates and manages dynamic memory blocks
//!HACK: relies on all mcsl::dyn_arr<T> having identical memory layouts
//!TODO: would probably be a good idea to find a way to make this runtime-type-safe (std::type_info* would probably help)
//  should probably also make that only active in debug mode for efficiency
class clef::allocator {
   private:
      mcsl::arr_list<mcsl::dyn_arr<ubyte>> _bufBuf;
   public:
      static allocator* active;
      
      allocator():_bufBuf{} {}
      allocator(allocator&& other):_bufBuf{std::move(other._bufBuf)} { if (this != &other) { other._bufBuf.release(); } }

      inline allocator& merge(allocator&& other);

      inline void free() { _bufBuf.free(); }
      inline void release() { _bufBuf.release(); }

      template<typename T> mcsl::dyn_arr<T>& at(const index<T>);

      template<typename T> index<T> alloc();
      template<typename T> index<T> emplaceBuf(mcsl::is_t<T> auto... initList);
      template<typename T> index<T> takeOwnershipOf(mcsl::dyn_arr<T>&& buf);

      template<typename T> void freeBuf(mcsl::dyn_arr<T>& buf) {
         std::destroy_at(&buf);
         if ((void*)&buf == (void*)&_bufBuf.back()) {
            _bufBuf.pop_back();
         }
      }
};

#include "../src/allocator.cpp"

#endif //CLEF_ALLOCATOR_HPP