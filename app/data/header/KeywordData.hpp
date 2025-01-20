#ifndef KEYWORD_DATA_HPP
#define KEYWORD_DATA_HPP

#include "CLEF.hpp"
#include "KeywordDecoder.hpp"

#include "raw_str_span.hpp"

namespace clef {
   constexpr KeywordID decodeKeyword(const mcsl::raw_str_span& str) { return KeywordDecoder::keywordID(str); }
};

#endif //KEYWORD_DATA_HPP