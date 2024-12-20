#ifndef KEYWORD_DATA_HPP
#define KEYWORD_DATA_HPP

#include "CLEF.hpp"
#include "KeywordDecoder.hpp"
#include "KeywordSpec.hpp"

#include "raw_str_span.hpp"

namespace clef {
   const KeywordDecoder ALL_KEYWORDS = GET_ALL_KEYWORDS();

   KeywordID decodeKeyword(const mcsl::raw_str_span& str) { return ALL_KEYWORDS[str]; }
};

#endif //KEYWORD_DATA_HPP