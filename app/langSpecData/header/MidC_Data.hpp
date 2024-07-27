#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"
#include "Hash.hpp"
#include "StrUtils.hpp"
#include <cstring>

namespace clef {
   extern byte tokTypeArr[256];

   extern const Operator OPERATORS[];
   extern const char BLOCK_DELIMS[][2][MAX_DELIM_LEN + 1];
   extern const char KEYWORDS[][MAX_KEYWORD_LEN + 1];
   
   
   //block delims
   DelimPairType blockDelimType(char* str, const uint length);

   //operators
   uint maxOpLen(const char* str, const uint len);
   const Operator* getOpData(const char* str, const uint len);

   //!function to determine if a string is a Middle-C keyword
   inline bool isKeyword(const char* ch, const uint length) {return Hash::isKeyword(ch, length);}
}

//operator string + metadata
class clef::Operator {
   public:
      char opStr[MAX_OP_LEN + 1];
      byte precedence;
      OpType opType; //unary/binary/special, left/right associative

      Operator(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type) {
         memset(opStr,0,sizeof(opStr));   //null-initialize
         strcpy(opStr,str);               //copy string
         precedence = prec;
         opType = type;
      }
};

#endif //DATA_HPP