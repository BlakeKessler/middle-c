#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "CLEF.hpp"
#include "MidC_Data.hpp"
#include "StrView.hpp"
#include <iosfwd>

class clef::Token : public clef::StrView {
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
      inline DelimPairType blockDelimEval() const { return _str ? blockDelimType(_str, _size) : DelimPairType::NONE; }
      //!check if a token represents a MiddleC keyword
      inline bool isKeyword() const { return _str ? clef::isKeyword(_str, _size) : false; }

      //comparison
      bool operator==(const std::string& other) const;
      // bool operator==(const char* other) const { return (strlen(other) == _size) && tokcmp(this, other,_size); }
      static bool tokcmp(const Token* tok, const char* str, const uint strlen);

      //token type number calculation
      byte typeNum() const;
      bool isType(const TokenType t) const { return typeNum() & t; }
      NodeType nodeType() const;
      static byte typeNum(const char ch, const bool isstart);
      static uint findTokEnd(char* const tokStr, const uint len);
      static uint findTokEnd(const StrView& str) { return findTokEnd(str.begin(),str.size()); }
      
      auto maxOpLen() { return clef::maxOpLen(_str, _size); }
      auto getOpData() { return clef::getOpData(_str, _size); }


      void throwError(const ErrCode code) const;
};

//token IO
namespace clef {
   std::ostream& operator<<(std::ostream& stream, const clef::Token& token);
   std::ostream& operator<<(std::ostream& stream, const clef::Token* token);
}

#endif //TOKEN_HPP