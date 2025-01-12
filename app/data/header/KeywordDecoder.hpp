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
   [[gnu::const]] clef::KeywordDecoder<90> GET_ALL_KEYWORDS();
};

template<uint _size> template <mcsl::is_t<clef::KeywordSpec>... Argv_t>
clef::KeywordDecoder<_size>::KeywordDecoder(const Argv_t... initList)
requires ( sizeof...(Argv_t) == _size ):_buf{initList...},_map{} {
   for (uint i = 0; i < _buf.size(); ++i) {
      _map[_buf[i].toString()] = i;
   }
}

template<uint _size> clef::KeywordID clef::KeywordDecoder<_size>::operator[](const mcsl::raw_str_span& tok) const {
   const auto tmp = _map.find(tok);
   if (tmp == _map.end()) {
      return KeywordID::_NOT_A_KEYWORD;
   }
   return _buf[tmp->second];
}

// #include "../src/KeywordDecoder.cpp"

#endif //KEYWORD_DECODER_HPP