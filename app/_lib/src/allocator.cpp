#ifndef CLEF_ALLOCATOR_CPP
#define CLEF_ALLOCATOR_CPP

#include "allocator.hpp"

inline clef::allocator& clef::allocator::merge(allocator&& other) {
   for (uint i = 0; i < other._bufBuf.size(); ++i) {
      _bufBuf.push_back(std::move(other._bufBuf[i]));
   }
   other._bufBuf.release();
   return self;
}

template<typename T> mcsl::dyn_arr<T>& clef::allocator::at(const index<T> i) {
   return reinterpret_cast<mcsl::dyn_arr<T>&>(_bufBuf.at(i));
}

template<typename T> clef::index<T> clef::allocator::alloc() {
   index<T> i = _bufBuf.size();
   _bufBuf.emplace_back();
   return i;
}

template<typename T> clef::index<T> clef::allocator::emplaceBuf(mcsl::is_t<T> auto... initList) {
   index<T> i = _bufBuf.size();
   _bufBuf.push_back(reinterpret_cast<mcsl::dyn_arr<ubyte>>(mcsl::dyn_arr<T>{initList...}));
   return i;
}

template<typename T> clef::index<T> clef::allocator::takeOwnershipOf(mcsl::dyn_arr<T>&& buf) {
   index<T> i = _bufBuf.size();
   _bufBuf.push_back(reinterpret_cast<mcsl::dyn_arr<ubyte>&>(buf));
   buf.release();
   return i;
}

#endif //CLEF_ALLOCATOR_CPP