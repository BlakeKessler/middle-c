#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "CLEF.hpp"
#include "MidC_Data.hpp"
#include "raw_str_span.hpp"

class clef::Token : public mcs::raw_str_span {
   private:
      const uint _lineNum;
   public:
      //constructors
      Token();
      Token(char* front, char* back, const uint line = 0);
      Token(char* front, const uint length, const uint line = 0);

      Token split(const uint len);

      //getters
      uint lineNum() const { return _lineNum; }

      //!check if a token represents a block delimiter
      inline DelimPairType blockDelimEval() const { return begin() ? blockDelimType(self) : DelimPairType::NONE; }
      //!check if a token represents a MiddleC keyword
      inline bool isKeyword() const { return begin() ? clef::isKeyword(self) : false; }

      //token type number calculation
      TokenType typeNum() const;
      bool isType(const TokenType t) const { return +(typeNum() & t); }
      NodeType nodeType() const;
      static TokenType typeNum(const char ch, const bool isstart);
      static uint findTokEnd(const char* const tokStr, const uint len);
      static uint findTokEnd(const mcs::raw_str_span& str) { return findTokEnd(str.begin(),str.size()); }
      
      auto maxOpLen() const { return clef::maxOpLen(self); }
      const Operator* getOpData() const { return clef::getOpData(self); }
      const DelimPair* getDelimPairData() const;
      const DelimPair* getPtxtData() const;

      //operators
      template<uint capacity> constexpr bool operator==(const mcs::raw_str<capacity>& str) const { const uint len = std::strlen(str.begin()); return (size() == len) && !std::memcmp(begin(),str.begin(),len*sizeof(char)); }


      void throwError(const ErrCode code) const;

      //token IO
      void printf() const { std::printf("%.*s", size(),begin()); }
};

#endif //TOKEN_HPP