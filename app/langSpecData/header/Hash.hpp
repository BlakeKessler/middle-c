#pragma once
#ifndef HASH_HPP
#define HASH_HPP

#include "CLEF.hpp"

class clef::Hash {
   private:
      static const byte keywordCalcTable[];
      static const char* const keywordHashTable[];
      static constexpr byte MAX_KEYWORD_HASH = 122;
      static unsigned int keywordHash(const char* str, uint len);
   public:
      static const char* isKeyword(const char* str, const uint len);
};
#endif //HASH_HPP