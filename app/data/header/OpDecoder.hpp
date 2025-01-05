#pragma once
#ifndef OP_DECODER_HPP
#define OP_DECODER_HPP

#include "CLEF.hpp"
#include "OpData.hpp"

#include "static_arr.hpp"
#include "concepts.hpp"
#include "pair.hpp"

template <uint _size> struct clef::OpDecoder {
   private:
      mcsl::static_arr<mcsl::pair<uint,uint>, OP_DECODER_HASH_BASE> _firstCharBuckets; //start, size
      mcsl::static_arr<OpData, _size> _opBuf;
      uint _opCount;
   public:
      constexpr OpDecoder():_firstCharBuckets{},_opBuf{},_opCount{} {}
      template <mcsl::is_t<clef::OpData>... Argv_t> constexpr OpDecoder(const Argv_t... initList) requires ( sizeof...(Argv_t) == _size );

      constexpr uint size() { return _opCount; } //number of operator groups
      constexpr uint capacity() { return _size; } //number of operators

      template<mcsl::str_t str_t> constexpr OpData operator[](const str_t& str) const;
      template<uint len> constexpr OpData operator[](const char (&str)[len]) const { return self[mcsl::raw_str(str)]; }
      constexpr mcsl::pair<uint,uint> operator[](const char i) const { return _firstCharBuckets[i % OP_DECODER_HASH_BASE]; }
};

#pragma region CTAD
namespace clef {
   template<mcsl::is_t<OpData>... Argv_t> OpDecoder(const Argv_t... arr) -> OpDecoder<sizeof...(arr)>;
}
#pragma endregion CTAD

namespace clef {
   constexpr auto GetAllOplikesData();
};

#include "../src/OpDecoder.cpp"

#endif //OP_DECODER_HPP