#ifndef KEYWORD_DATA_HPP
#define KEYWORD_DATA_HPP

#include "CLEF.hpp"
#include "KeywordDecoder.hpp"

#include "str_slice.hpp"

namespace clef {
   constexpr KeywordID decodeKeyword(const mcsl::str_slice& str) { return KeywordDecoder::keywordID(str); }
};

#endif //KEYWORD_DATA_HPP