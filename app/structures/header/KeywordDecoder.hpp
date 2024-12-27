#pragma once
#ifndef KEYWORD_DECODER_HPP
#define KEYWORD_DECODER_HPP

#include "CLEF.hpp"
#include "KeywordSpec.hpp"

#include "raw_str_span.hpp"
#include "static_arr.hpp"
#include <unordered_map>

template<uint _size> struct clef::KeywordDecoder {
   private:
      mcsl::static_arr<KeywordSpec, _size> _buf;
      std::unordered_map<mcsl::raw_str_span, uint> _map;
   public:
      KeywordDecoder():_buf{},_map{} {}
      template <mcsl::is_t<KeywordSpec>... Argv_t> KeywordDecoder(const Argv_t... initList) requires ( sizeof...(Argv_t) == _size );

      uint size() const { return _size; }

      KeywordID operator[](const mcsl::raw_str_span& tok) const;
};

#pragma region CTAD
namespace clef {
   template <mcsl::is_t<KeywordSpec>... Argv_t> KeywordDecoder(const Argv_t... arr) -> KeywordDecoder<sizeof...(arr)>;
}
#pragma endregion CTAD

namespace clef {
   auto GET_ALL_KEYWORDS();
};

#include "../src/KeywordDecoder.cpp"

#endif //KEYWORD_DECODER_HPP